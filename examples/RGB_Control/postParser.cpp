#include "Arduino.h"
#include "postParser.h"

//private


String getLine(const String& data) {
  int endOfLineIndex = data.indexOf("\r\n");
  return data.substring(0, endOfLineIndex );
}

String popLine(const String& data) {
  int endOfLineIndex = data.indexOf("\r\n");
  //Serial.println(data.substring(endOfLineIndex + 2, data.length() - 1));
  return data.substring(endOfLineIndex + 2, data.length());
}

String getHeaderField(const String& data, String key) {
  const String&  bufferData = data;
  int keyIndex = bufferData.indexOf(key);
  if (keyIndex == -1) {
    return "";
  }
  int startIndex = bufferData.indexOf(": ", keyIndex);
  int stopIndex = bufferData.indexOf("\r\n", keyIndex);
  //	Serial.print("data: ");
	//Serial.println(bufferData.substring(startIndex + 2, stopIndex));
  return bufferData.substring(startIndex + 2, stopIndex);
}

String getLineFromIndex(const String& data, int index) {
  String bufferData = data;
  for (int i = 0; i < index; i++) {
    bufferData = popLine(bufferData);
  }
  return getLine(bufferData);
}

String getContentType(const String& header) {
  String contentType = getHeaderField(header, "content-type");
  if (contentType == "") {
    contentType = getHeaderField(header, "Content-Type");
  }
  return contentType;
}

String readPayLoad(EthernetClient client, int payLoadSize) {
  String payload = String("");
  for (int i = 0; i < payLoadSize; i++) {
    char c = client.read();
    payload += String(c);
  }
  return payload;
}


int getPayLoadSize(const String& header) {
  String contentLength = getHeaderField(header, "content-length");
  if (contentLength == "") {
    contentLength = getHeaderField(header, "Content-Length");
  }
  return contentLength.toInt();
}


//public
PostParser::PostParser(EthernetClient client) {
  //save connection
  _client = client;


  //clear data
  _header = String("");
  _payload = String("");
}

void PostParser::addHeaderCharacter(char c) {
  _header += String(c);
}

String PostParser::getHeader() {
  return _header;
}

void PostParser::grabPayload() {
  //if (getContentType(_header) == "application/x-www-form-urlencoded") {
    _payload = readPayLoad(_client, getPayLoadSize(_header));
    
  //}
}

String PostParser::getPayload() {
  return _payload;
}

String PostParser::getField(const String& key) {

  int keyIndex = _payload.indexOf(key);
  int startIndex = _payload.indexOf("=", keyIndex);
  int stopIndex = _payload.indexOf("&", keyIndex);

  return _payload.substring(startIndex + 1, stopIndex);

}





