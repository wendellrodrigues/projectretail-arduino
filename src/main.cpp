#include <Arduino.h>
#include <ArduinoJson.h> //Make sure to install this library
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip( 192,168,1,10 );
IPAddress gateway( 192,168,1,1 );
IPAddress subnet( 255,255,255,0 );
IPAddress dns( 192,168,1,1 );

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

StaticJsonDocument<256> doc;

const byte BLUE = 6;
const byte GREEN = 5;
const byte RED = 3;

void setup()
{
 Serial.begin(9600);

 pinMode(BLUE,OUTPUT);
 pinMode(GREEN,OUTPUT);
 pinMode(RED,OUTPUT);


 // start the SD interface here if you want.
 // Add the SD.h library above
 // SD.begin(4);

 // start the Ethernet connection and the server:
 Ethernet.begin(mac, ip, dns, gateway, subnet);
 // disable w5100 SPI so SD SPI can work with it
 delay(2000);
 server.begin();

 Serial.println("setup finished");

 
}

void loop()
{

  
  
 // listen for incoming clients
 EthernetClient client = server.available();
 if (client) {
   Serial.println("Client");
   // an http request ends with a blank line
   boolean currentLineIsBlank = true;
   while (client.connected()) {
     while(client.available()) {
       char c = client.read();
       // if you've gotten to the end of the line (received a newline
       // character) and the line is blank, the http request has ended,
       // so you can send a reply
       if (c == '\n' && currentLineIsBlank) {
        
         //GET Data coming in  
         String data;
         while(client.available())
         {
          char command = client.read();
          data+= command;
         }

 
         //Deserialize the JSON
         DeserializationError err = deserializeJson(doc, data);

         if(err) {
          Serial.print("ERROR: ");
          Serial.println(err.c_str());
          return;
         }

         //The incoing jason will be the 
         int shelf = doc["shelf"];

         Serial.println(shelf);

         if(shelf == 1) { 
            digitalWrite(BLUE, HIGH);
            delay(1000);
            digitalWrite(BLUE, LOW); 
          } 

          else if(shelf == 2) { 
            digitalWrite(GREEN, HIGH);
            delay(1000);
            digitalWrite(GREEN, LOW); 
          } 

          else if(shelf == 3) {
            digitalWrite(RED, HIGH);
            delay(1000);
            digitalWrite(RED, LOW);
          }

         Serial.println("Sending response");
         
         // send a standard http response header (200)
         client.println("HTTP/1.0 200 OK");
         client.println("Content-Type: application/json");
         client.println();
         client.stop();
       }
       else if (c == '\n') {
         // you're starting a new line
         currentLineIsBlank = true;
       }
       else if (c != '\r') {
         // you've gotten a character on the current line
         currentLineIsBlank = false;
       }
     }
   }
   Serial.println("Disconnected");
 }
}