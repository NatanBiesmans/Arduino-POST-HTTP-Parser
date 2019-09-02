#ifndef POSTPARSER_h
#define POSTPARSER_h


#include "Arduino.h"
#include "Ethernet.h"

class PostParser {
  public:
    PostParser(EthernetClient client);
    void addHeaderCharacter(char c);
    String getHeader();
    void grabPayload();
    String getPayload();
    String getField(const String& key);
  private:
    String _header;
    String _payload;
    EthernetClient _client;

    String getHeaderField(const String& data, String key);
    String getLine(const String& data);
    String popLine(const String& data);
    String getLineFromIndex(const String& data, int index);
    String extractPayload(const String& payload, const String& key);
};

#endif
