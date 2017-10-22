#include <WEMOS_DHT12.h>
#include <ESP8266WiFi.h>
#include <settings.h>
#include <Wire.h>
#include <Heater.h>
//#include <FS.h>
//#include <ESP8266mDNS.h>

//change wire pins because relay will be on D1
const int sclPin = D5;
const int sdaPin = D2;

const int pollInterval = 50000;

const char* ssid = _ssid;
const char* password = _password;

unsigned long counter = 1;
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;

WiFiServer server(80);
DHT12 dht12;
Heater heater(dht12);

void setup() {
  Wire.begin(sdaPin, sclPin);
  Serial.begin(115200);
  delay(10);
 
  // Connect to WiFi network
  //WiFi.mode(WIFI_AP_STA);
  WiFi.hostname("summer");//why is this not working? LE: works with host.local
  
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}
 
void loop() {
  //poll the sensor every 50000 seconds
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
  Serial.println("new client");
  while(!client.available()){
    Serial.println("client wait");
    delay(1);
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
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  
  client.println("<h1><a href=\"/\">Temperature Control <br>================</a><br></h1>");  
  client.print("Heater is now   ");
  client.print(heater.heaterState());
  if(heater.heaterState() == false) {
    client.print("<off style=\"color:red\"> OFF</off><br>");
    } else {
      client.print(" ON<br>");
    }
  client.print("Mode set to: ");
  if(heater.isInAutoMode() == false) {
    client.print(" MANUAL<br>");
    } else {
      client.print(" AUTO<br>");
    }



  client.println("<br>");
  client.println("Click <a href=\"/Heater=ON\">here to turn heater ON</a><br>");
  client.println("Click <a href=\"/Heater=OFF\">here to turn heater OFF</a><br><br>");
  client.println("Click <a href=\"/Heater=AUTO\">here to set to AUTO mode</a><br>");
  client.println("<a href=\"/temp-minus\">MINUS--</a> Temp: ");
  client.println(heater.getDesiredTemp());
  client.println(" C <a href=\"/temp-plus\"</a> PLUS++");
  client.println("<br>");
  client.println("<br><br>");

  client.println("Temperature in Celsius : ");
    client.println(heater.getTemp());
    client.println("<br>");
    client.print("Relative Humidity : ");
    client.println(heater.getHumi());
    client.println("<br>END");
    client.println(counter);

  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
}