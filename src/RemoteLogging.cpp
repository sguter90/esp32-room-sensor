#include "RemoteLogging.h"
#include "WebServerAdvanced.h"

RemoteLogging::RemoteLogging(WebServerAdvanced &web, WebSocketsServer &socket)
{
    _web = &web;
    _socket = &socket;
}

void RemoteLogging::registerHttpLogHandler(const Uri &route)
{
    std::function<void(WebServer &, HTTPMethod, String)> handler = [](WebServer &server, HTTPMethod requestMethod, String requestUri)
    {
        RemoteLogging::http_handleLog(server, requestMethod, requestUri);
    };
    this->_web->on(route, HTTP_GET, handler);
}

void RemoteLogging::registerSerialLog(Print &output)
{
    this->_printer = &output;
}

boolean RemoteLogging::setBufferSize(uint16_t size)
{
    if (size == 0)
    {
        return false;
    }
    if (this->_bufferSize == 0)
    {
        this->_buffer = (uint8_t *)malloc(size);
        this->_bufferEnd = this->_buffer;
    }
    else
    {
        uint8_t *newBuffer = (uint8_t *)realloc(this->_buffer, size);
        if (newBuffer != NULL)
        {
            this->_buffer = newBuffer;
            this->_bufferEnd = this->_buffer;
        }
        else
        {
            return false;
        }
    }
    this->_bufferSize = size;
    return (this->_buffer != NULL);
}

// send & reset current buffer
void RemoteLogging::sendBuffer()
{
    if (this->_bufferCnt > 0)
    {
        this->_socket->broadcastTXT(this->_buffer, this->_bufferCnt, false);
        this->_bufferCnt = 0;
    }
    this->_bufferEnd = this->_buffer;
}

size_t RemoteLogging::write(uint8_t character)
{
    if (character == '\n') // when newline is printed we send the buffer
    {
        this->sendBuffer();
    }
    else
    {
        if (this->_bufferCnt < this->_bufferSize) // add char to end of buffer
        {
            *(this->_bufferEnd++) = character;
            this->_bufferCnt++;
        }
        else // buffer is full, first send&reset buffer and then add char to buffer
        {
            this->sendBuffer();
            *(this->_bufferEnd++) = character;
            this->_bufferCnt++;
        }
    }

    if (this->_printer != NULL)
    {
        this->_printer->print((char) character);
    }
    return 1;
}

void RemoteLogging::http_handleLog(WebServer &server, HTTPMethod requestMethod, String requestUri)
{
    String response = R"RESPONSE(<!DOCTYPE html>
<html>
   <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
   </head>
 
   <body>
      <div>
         <textarea id="log" style="width: 100%; height: 90vh"></textarea>
      </div>

      <script type = "text/javascript">
        let log = document.getElementById("log");
        let ws;
        let tmPing = null;
        let tmPong = null;
        /*
         * 0 = disconnected
         * 1 = connected
         * 3 = connecting
         */
        var connectionState = 0;

        function ping() {
            if (tmPong !== null) {
                return;
            }
            
            ws.send('__ping__');
            tmPong = setTimeout(function () {
                console.log("Connection stale, reconnecting...");
                log.style.borderColor = 'orange';
                ws.close(); // onclose will reconnect
            }, 3000);
        }

        function pong() {
            if (tmPong === null) {
                return;
            }

            clearTimeout(tmPong);
            tmPong = null;
        }

        function connectWebsocket() {
            if (connectionState !== 0) {
                return;
            }
            connectionState = 3;

            ws = new WebSocket("ws://" + window.location.hostname + ":81/ws");
    
            ws.onopen = function() {
                console.log("Connected");
                connectionState = 1;
                log.style.borderColor = 'green';
                tmPing = setInterval(ping, 5000);
            };

            ws.onclose = function() {
                console.log("Disconnected, retrying...");
                connectionState = 0;
                clearTimeout(tmPong);
                tmPong = null;
                clearTimeout(tmPing);
                tmPint = null;
                log.style.borderColor = 'red';
                setTimeout(connectWebsocket, 500);
            };

            ws.onerror = function(err) {
                log.style.borderColor = 'orange';
                console.log(err);
                ws.close();
            };
    
            ws.onmessage = function(event) {
                if (event.data == '__pong__') {
                    pong();
                    return;
                }

                log.innerHTML += event.data;
                log.selectionEnd = log.innerHTML.length;
            };

        }
 
        connectWebsocket();
      </script>
   </body>
</html>
  )RESPONSE";

    server.send(200, "text/html", response);
}

// log view sends a ping request via socket to check if connection is up
void RemoteLogging::pong(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT && strcmp((const char *)payload, "__ping__") == 0)
  {
    this-_socket->sendTXT(num, "__pong__");
  }
}