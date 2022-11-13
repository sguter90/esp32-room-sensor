#ifndef REMOTE_LOGGING_H
#define REMOTE_LOGGING_H

#include "WebServerAdvanced.h"
#include <WebSocketsServer.h>

#ifdef ESP8266
typedef ESP8266WebServer WebServer;
#endif

class RemoteLogging : public Print
{
private:
    WebServerAdvanced *_web;
    WebSocketsServer *_socket;
    Print *_printer;
    uint8_t* _buffer;
    uint8_t* _bufferEnd;
    uint16_t _bufferCnt = 0, _bufferSize = 0;

    void http_handleLog();
    void sendBuffer();

public:
    RemoteLogging(WebServerAdvanced &web, WebSocketsServer &socket);
    void registerHttpLogHandler(const Uri &route);
    static void http_handleLog(WebServer &server, HTTPMethod requestMethod, String requestUri);
    void registerSerialLog(Print &output);

    virtual size_t write(uint8_t);
    using Print::write;

    uint16_t getBufferSize();
    boolean setBufferSize(uint16_t size);
    void pong(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
};

#endif