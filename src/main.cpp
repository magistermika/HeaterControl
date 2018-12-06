#include <WEMOS_DHT12.h>
#include <ESP8266WiFi.h>
#include <settings.h>
#include <Wire.h>
#include <Heater.h>
//#include <EEPROM.h>
//#include <FS.h>
#include <ESP8266mDNS.h>

//change wire pins because relay will be on D1
const int sclPin = D5;
const int sdaPin = D2;

const int pollInterval = 60000;

const char* ssid = _ssid;
const char* password = _password;

unsigned long counter = 0;
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;

String response;
const char* button1 = "Manual ON";
const char* button2 = "Manual OFF";
const char* button3 = "Auto Mode";    

WiFiServer server(80);
Heater heater;

void setup() {
  Wire.begin(sdaPin, sclPin);
  Serial.begin(9600); 
  delay(10);
 
  // Connect to WiFi network
  WiFi.hostname("summer");//why is this not working?
  WiFi.mode(WIFI_STA);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  int checks = 0;
  while (WiFi.status() != WL_CONNECTED  || checks < 10) {
    delay(500);
    checks++;
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  //mDNS to have the local hostname work
  MDNS.begin("summer");
  MDNS.addService("http", "tcp", 80);

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {
  //poll the sensor every 60 seconds
  currentMillis = millis();
  if (currentMillis - prevMillis > pollInterval) {
    prevMillis = currentMillis;
    heater.refresh();    
    if (heater.isInAutoMode() == true) {
      heater.heaterMode(2);
    }
    counter++;
  }  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client. waiting");
  while(!client.available()){
    Serial.print(".");
    delay(10);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
  if (request.indexOf("/Heater=ON") != -1) {
    heater.heaterMode(1);
  } 
  else if (request.indexOf("/Heater=AUTO") != -1) {
    heater.heaterMode(2);
  }
  else if (request.indexOf("/Heater=OFF") != -1) {
    heater.heaterMode(0);
  }
  else if (request.indexOf("/temp-plus") != -1) {
    heater.setDesiredTempPlus();
  }
  else if (request.indexOf("/temp-minus") != -1) {
    heater.setDesiredTempMinus();
  }
 
  // Construct the response
  response = "HTTP/1.1 200 OK\n";
  response += "Content-Type: text/html \n\n";
  response += "<!DOCTYPE HTML>\n";
  response += "<html>\n";
  
  response += "<h1><a href=\"/\">Temperature Control</h1></a> \n";
  response += "Heater is now   ";
  response += heater.heaterState();
  if(heater.heaterState() == false) {
    response += "<off style=\"color:red\"> OFF</off><br>";
    } else {
      response += " ON<br>";
    }
  response += "Mode set to: ";
  if(heater.isInAutoMode() == false) {
    response += " MANUAL<br>";
    } else {
      response += " AUTO<br>";
      button3 = ">>AUTO MODE<<";        
    }
  response += "<br>";
  response += "<a href=\"/Heater=ON\"><button>";
  response += button1;
  response += "</button></a>";
  response += "<a href=\"/Heater=AUTO\"><button>";
  response += button3;
  response += "</button></a>";
  response += "<a href=\"/Heater=OFF\"><button>";
  response += button2;
  response += "</button></a><br>";
  response += "<a href=\"/temp-minus\"><button>MINUS--</button></a> Temp: ";
  response += heater.getDesiredTemp();
  response += " C <a href=\"/temp-plus\"><button>PLUS++</button></a> <br><br>";

  response += "Temperature in Celsius : ";
  response += heater.getTemp();
  response += " C<br>";
  response += "Relative Humidity : ";
  response += heater.getHumi();
  response += " %<br> Refresh counter: ";
  response += counter;

  response += "</html>";
  
  client.print(response);

  delay(100);
  Serial.println("Client disconnected");
  Serial.println("");
  client.stop();
}