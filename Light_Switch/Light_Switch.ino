#include <Ethernet2.h>
#include <SPI.h>
#include "postParser.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

int numberOfChannels = 4;

int LedPin = 13;
boolean LedState = false;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(1000000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  pinMode(LedPin, OUTPUT);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    PostParser postParser = PostParser(client); // create our parser
    
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        postParser.addHeaderCharacter(c); // compose the header

        if (c == '\n' && currentLineIsBlank) { // end of header
          postParser.grabPayload();

          if(postParser.getField("Action") == "Toggle"){
            LedState = !LedState;
            digitalWrite(LedPin, LedState);
          }

          // Send a nice message back telling the browser/application that we received and processed the data
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Access-Control-Allow-Origin: *"); // usefull to get all the responses back
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println();
          client.println("Done");
          
          Serial.println(postParser.getHeader()); // print the header for debugging
          delay(10); //used to make sure the 2 serial prints don't overlap each other
          Serial.println(postParser.getPayload()); // print the payload for debugging
          break;
        }
        
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }


    delay(1);
    client.stop();
  }
}

/*
<!DOCTYPE html>
<html>
<head>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js"></script>
<script>
$(document).ready(function(){
    $("button").click(function(){
        $.post("http://192.168.1.177:80",
        {
          Action: "Toggle"
        },
        function(data,status){
            alert("Data: " + data + "\nStatus: " + status);
        });
    });
});
</script>
</head>
<body>

<button>Send an HTTP POST request to a page and get the result back</button>

</body>
</html>
*/

