// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid     = "***********"; //Change this to ssid
const char* password = "***********"; //Change this to password

// Set web server port number to 80
WiFiServer server(80);

//For Reading JSON
StaticJsonDocument<256> doc;

// Variable to store the HTTP request
String header;

//LEDs
const byte RED = 16;
const byte GREEN = 5;
const byte BLUE = 4;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs

  pinMode(RED,OUTPUT);  //RED corresponds to bottom shelf
  pinMode(GREEN,OUTPUT); //GREEN corresponds to middle shelf
  pinMode(BLUE,OUTPUT);  //BLUE corresponds to top shelf

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {

   //Initialize
   boolean currentLineIsBlank = true;

   //Client requeuest (from API)
   while (client.connected()) {
     while(client.available()) {
       char c = client.read();

       //Receive signal and light LED
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

         //The incoming jason will be the shelf # to light up. 
         int shelf = doc["shelf"];

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

         
         //send a standard http response header (200)
         client.println("HTTP/1.0 200 OK");
         client.println("Content-Type: application/json");
         client.println();
         client.stop();
       }

       //Handle end of line
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
 }
}