#include <WEMOS_DHT12.h>
#include <ESP8266WiFi.h>
#include <settings.h>
#include <Wire.h>

//change wire pins because relay will be on D1
const int sclPin = D5;
const int sdaPin = D2;

const int relayPin = D1;
const int pollInterval = 5000;
const int desiredTemp = 24;

const char* ssid = _ssid;
const char* password = _password;


int ledPin = BUILTIN_LED;

unsigned long counter = 1;
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;
float temp = 0;
float humi = 0;

class Heater {
public:
    Heater();
    void heaterMode(const char* mode);
    float getTemp();
    float getHumi();
    void refresh();
    void setDesiredTemp(float temp);
    void heaterOn(bool on);
    bool heaterState();

private:
    int _temp;
    int _humi;
    bool _isHeaterOn;
};

WiFiServer server(80);
DHT12 dht12;
Heater heater;

void setup() {
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Wire.begin(sdaPin, sclPin);
  Serial.begin(115200);
  delay(10);
 
  digitalWrite(ledPin, HIGH);
 
  // Connect to WiFi network
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
  //poll the sensor every 5000 seconds
  currentMillis = millis();
  if (currentMillis - prevMillis > pollInterval) {
    prevMillis = currentMillis;
    heater.refresh();    
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
    heater.heaterMode("manualOn");
  } 
  if (request.indexOf("/Heater=AUTO") != -1) {
    heater.heaterMode("automode");
  }
  else {
    heater.heaterMode("manualOff");
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Heater is now ");
  client.print(heater.heaterState());
  if(heater.heaterState() == false) {
    client.print(" Off<br>");
  } else {
    client.print(" On<br>");
  }
    
  client.println("<br><br>");
  client.println("Click <a href=\"/Heater=ON\">here to turn heater ON</a><br>");
  client.println("Click <a href=\"/\">here to turn heater OFF</a><br>");
  client.println("Click <a href=\"/Heater=AUTO\">here to set to AUTO mode</a><br>");
  
  client.println("============<br> Temperature Control <br>============");
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

void Heater::heaterMode(const char* mode) {
  if (mode == "manualOn") {
    heaterOn(true);
    return;
  }
  if (mode == "manualOff") {
    heaterOn(false);
    return;
  }
  else {
    if (dht12.cTemp < desiredTemp) {
      heaterOn(true);
    }
    else {
      heaterOn(false);
    }
    return;
  }
}

void Heater::heaterOn(bool on) {
  if (on) {
    digitalWrite(ledPin, LOW);
    digitalWrite(relayPin, HIGH);
    _isHeaterOn = true;
  }
  else {
    digitalWrite(ledPin, HIGH);
    digitalWrite(relayPin, LOW);
    _isHeaterOn = false;
  }
}

bool Heater::heaterState() {
  return _isHeaterOn;
}

void Heater::refresh() {
  if (dht12.get()) {
    _temp = dht12.cTemp;
    _humi = dht12.humidity;
  }  
}