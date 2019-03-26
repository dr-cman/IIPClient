/* 
  IIPClient.h - esp8266 library to access IIP functions

  Copyright (c) 2019 Siemens AG, Claudio Laloni
*/

#ifndef IIPCLIENT_H
#define IIPCLIENT_H

#include <stdint.h>
#include <stddef.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <time.h>


class IIPClient {
  private:
    HTTPClient _iip;
    String _httpMethod;
    String _httpCmd;
    String _IIPServer;
    String _httpReturn;
    String _httpString;
    int _httpCode;

  public:
    IIPClient(String IIPServer);
    ~IIPClient();

    void IIPdsms();
    void IIPSendFile(String FilePathAndName);

  private:
    String IIPCreateArtifactId();
    void HTTPResult();
};

#endif//IIPCLIENT_H


