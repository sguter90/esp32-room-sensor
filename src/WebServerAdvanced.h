#ifndef WEBSERVER_ADVANCED_H
#define WEBSERVER_ADVANCED_H

#ifdef ESP8266
#include <ESP8266WebServer.h>
#include <vector>

typedef ESP8266WebServer WebServer;
typedef std::function<void(ESP8266WebServer &, HTTPMethod, String)> TExtendedHandlerFunction;

class ClassRequestHandler : public RequestHandler
{
public:
    ClassRequestHandler(TExtendedHandlerFunction fn, ESP8266WebServer::THandlerFunction ufn, const Uri &uri, HTTPMethod method)
        : _fn(fn), _ufn(ufn), _uri(uri.clone()), _method(method)
    {
    }

    ~ClassRequestHandler()
    {
        delete _uri;
    }

    bool canHandle(HTTPMethod requestMethod, const String& requestUri) override
    {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        return _uri->canHandle(requestUri, pathArgs);
    }

    bool canUpload(const String& requestUri) override
    {
        if (!_ufn || !canHandle(HTTP_POST, requestUri))
            return false;

        return true;
    }

    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, const String& requestUri) override
    {
        (void)server;
        if (!canHandle(requestMethod, requestUri))
            return false;

        _fn(server, requestMethod, requestUri);
        return true;
    }

protected:
    TExtendedHandlerFunction _fn;
    ESP8266WebServer::THandlerFunction _ufn;
    Uri *_uri;
    HTTPMethod _method;
};

class WebServerAdvanced : public ESP8266WebServer::ESP8266WebServerTemplate
{
    using ESP8266WebServer::ESP8266WebServer;

public:
    void on(const Uri &uri, HTTPMethod method, TExtendedHandlerFunction fn, WebServer::THandlerFunction ufn)
    {
        _addRequestHandler(new ClassRequestHandler(fn, ufn, uri, method));
    }

    void on(const Uri &uri, TExtendedHandlerFunction handler)
    {
        on(uri, HTTP_ANY, handler);
    }

    void on(const Uri &uri, HTTPMethod method, TExtendedHandlerFunction fn)
    {
        on(uri, method, fn, _fileUploadHandler);
    }
};

#else
#include <WebServer.h>

typedef std::function<void(WebServer &, HTTPMethod, String)> TExtendedHandlerFunction;

class ClassRequestHandler : public RequestHandler
{
public:
    ClassRequestHandler(TExtendedHandlerFunction fn, WebServer::THandlerFunction ufn, const Uri &uri, HTTPMethod method)
        : _fn(fn), _ufn(ufn), _uri(uri.clone()), _method(method)
    {
        _uri->initPathArgs(pathArgs);
    }

    ~ClassRequestHandler()
    {
        delete _uri;
    }

    bool canHandle(HTTPMethod requestMethod, String requestUri) override
    {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        return _uri->canHandle(requestUri, pathArgs);
    }

    bool canUpload(String requestUri) override
    {
        if (!_ufn || !canHandle(HTTP_POST, requestUri))
            return false;

        return true;
    }

    bool handle(WebServer &server, HTTPMethod requestMethod, String requestUri) override
    {
        (void)server;
        if (!canHandle(requestMethod, requestUri))
            return false;

        _fn(server, requestMethod, requestUri);
        return true;
    }

protected:
    TExtendedHandlerFunction _fn;
    WebServer::THandlerFunction _ufn;
    Uri *_uri;
    HTTPMethod _method;
};

class WebServerAdvanced : public WebServer::WebServer
{
    using WebServer::WebServer;

public:
    void on(const Uri &uri, HTTPMethod method, TExtendedHandlerFunction fn, WebServer::THandlerFunction ufn)
    {
        _addRequestHandler(new ClassRequestHandler(fn, ufn, uri, method));
    }

    void on(const Uri &uri, TExtendedHandlerFunction handler)
    {
        on(uri, HTTP_ANY, handler);
    }

    void on(const Uri &uri, HTTPMethod method, TExtendedHandlerFunction fn)
    {
        on(uri, method, fn, _fileUploadHandler);
    }
};
#endif

#endif