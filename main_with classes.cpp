#include <WEMOS_DHT12.h>
#include <ESP8266WiFi.h>

#include <Wire.h>

//change wire pins because relay will be on D1
const int sclPin = D5;
const int sdaPin = D2;

const int relayPin = D1;
const int pollInterval = 5000;
const int desiredTemp = 25;

const char* ssid = "ssidname";
const char* password = "password";


int ledPin = BUILTIN_LED;

unsigned long counter = 1;
unsigned long prevMillis = 0;
unsigned long currentMillis = 0;
float temp = 0;
float humi= 0;
bool ledOn = false;
bool relayOn = false;

WiFiServer server(80);
DHT12 dht12;

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
  //poll the senzor every 5000 seconds
  currentMillis = millis();
  if (currentMillis - prevMillis > pollInterval) {
    prevMillis = currentMillis;
    if (dht12.get() == 0 && counter++) {
      temp = dht12.cTemp;
      humi = dht12.humidity;
      
      
      if (dht12.cTemp > desiredTemp) {
        digitalWrite(ledPin, LOW);
        digitalWrite(relayPin, HIGH);
      }
      else {
        digitalWrite(ledPin, HIGH);
        digitalWrite(relayPin, LOW);
      }
    }  
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
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, LOW);
    digitalWrite(relayPin, HIGH);
    value = HIGH;
  } 
  if (request.indexOf("/LED=OFF") != -1){
    digitalWrite(ledPin, HIGH);
    digitalWrite(relayPin, LOW);
    value = LOW;
  }
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Led pin is now: ");
 
  if(value == LOW) {
    client.print("Off<br>");  
  } else {
    client.print("On<br>");
  }
  
  client.print("Relay is now: ");
  if(value == LOW) {
    client.print("OFF<br>");  
  } else {
    client.print("ON<br>");
  }

  
  client.println("<br><br>");
  client.println("Click <a href=\"/LED=ON\">here</a> turn the LED   ON<br>");
  client.println("Click <a href=\"/LED=OFF\">here</a> turn the LED   OFF<br>");
  client.println(dht12.get());
  client.println("======= Cel mai boss de server cu temperatura/umiditate =======");
  client.println("<br><br>");

  client.println("Temperature in Celsius : ");
    client.println(dht12.cTemp);
    client.println("<br>");
    client.print("Relative Humidity : ");
    client.println(dht12.humidity);
    client.println("<br>END");
    client.println(counter);

  
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");
 
}

class Heater {
    public:
        Heater();
        float getTemp();
        float getHumi();
        void setDesiredTemp(float temp);
        bool heaterOn();
        void turnHeaterOn();

    private:
        int temp;
        int humi;
        bool heaterOn;
        bool ledOn;
}

char ledOn(bool state) {
  if (state == true) digitalWrite(ledPin, LOW) return "ON";
  else digitalWrite(ledPin, HIGH) return "OFF";
}