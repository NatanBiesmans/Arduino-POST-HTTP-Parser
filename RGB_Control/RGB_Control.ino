/*

A simple example of the HTTP POST parser. 

You can use this to create a RGB LED controller that can be controlled by any browser.

HTML code provided on the bottom of the file
*/
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

int rgbPins[4][3] = {
  {5, 6, 3},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};
int rgb[4][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};
int new_rgb[4][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

int delay_index = 0;
int delay_time = 20;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(1000000);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


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
          // handle data
          postParser.grabPayload(); // Use the header to get the payload in memory
          String type = postParser.getField("Type"); // Get the operation we want to do
          
          if (type == "Set") { // Set RGB values
            int channel = postParser.getField( "Channel").toInt();
            new_rgb[channel][0] = postParser.getField( "Red").toInt();
            new_rgb[channel][1] = postParser.getField( "Green").toInt();
            new_rgb[channel][2] = postParser.getField( "Blue").toInt();
          }

          if (type == "Delay") { // Set transition delay
            delay_time = postParser.getField("Wait").toInt();
            Serial.println(delay_time);
          }

          if (type == "Config") { // Change the channel pins
            int channel  = postParser.getField( "Channel").toInt();
            rgbPins[channel][0] = postParser.getField( "Red").toInt();
            rgbPins[channel][1] = postParser.getField( "Green").toInt();
            rgbPins[channel][2] = postParser.getField( "Blue").toInt();
            Serial.println(channel);
            Serial.println(rgbPins[channel][0]);
            Serial.println(rgbPins[channel][1]);
            Serial.println(rgbPins[channel][2]);
          }

          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Access-Control-Allow-Origin: *"); // usefull to get all the responses
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
  update_colors();
}

void update_colors() {
  delay_index++;
  if (delay_index >= delay_time) {
    delay_index = 0;

    for (int i = 0; i < numberOfChannels; i++) {
      for (int j = 0; j < 3; j++ ) {
        if (rgb[i][j] < new_rgb[i][j]) {
          rgb[i][j]++;
        }
        if (rgb[i][j] > new_rgb[i][j]) {
          rgb[i][j]--;
        }
        analogWrite(rgbPins[i][j], rgb[i][j]);
      }
    }
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
          Type: "Set",
          Channel: "0",
      Red: "255",
      Green: "255",
      Blue: "255"
      ,
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

