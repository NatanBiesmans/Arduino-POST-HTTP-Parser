#ifndef POSTPARSER_h
#define POSTPARSER_h


#include "Arduino.h"
#include "Ethernet2.h"

class PostParser {
  public:
    PostParser(EthernetClient client);
    void addHeaderCharacter(char c);
    String getHeader();
    void grabPayload();
    String getPayload();
    String getField(String key);
  private:
    String _header;
    String _payload;
    EthernetClient _client;

    String getHeaderField(String data, String key);
    String getLine(String data);
    String popLine(String data);
    String getLineFromIndex(String data, int index);
    String extractPayload(String payload, String key);
};

#endif
