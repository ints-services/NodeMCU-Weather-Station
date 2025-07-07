#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

/*Put your SSID & Password*/
const char* ssid = "SSID";  // Enter SSID here
const char* password = "Password";  //Enter Password here

ESP8266WebServer server(80);

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 10);  // Example IP = 192.168.1.10
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);  // Example Gateway = 192.168.1.1

IPAddress subnet(255, 255, 255, 0); // Subnet Mask = 255.255.255.0
IPAddress primaryDNS(8, 8, 8, 8);   // Optional
IPAddress secondaryDNS(8, 8, 4, 4); // Optional

// DHT Sensor
uint8_t DHTPin = D7; // Define DHT Sensor Pin = D7
               
// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);                

float Temperature;
float Humidity;

float humidity_percent, temp_celsius;  // Values read from sensor
String webString="";   
unsigned long previousMillis = 0;        // Will store last temp was read
const long interval = 2000;              // Interval at which to read sensor
 
void setup() {
  Serial.begin(9600);
  delay(100);
  
  pinMode(DHTPin, INPUT);

  dht.begin();              

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");

   //Rest API for sensor data
  server.on("/api", [](){  
    gettemperature();           // Read sensor
    String json="{\"Temperature\":"+String((int)temp_celsius)+",\"Humidity\":"+String((int)humidity_percent)+"}";
    Serial.println(json);
    server.send(200, "application/json", json);
  });

}
void loop() {
  
  server.handleClient();
  
}

void handle_OnConnect() {

  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(Temperature,Humidity)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

/*
Creates web page. In this template it is accessible from http://192.168.1.10
*/
String SendHTML(float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"><meta http-equiv=\"refresh\" content=\"60\">\n";
  ptr +="<title>Home Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>Home Weather Report</h1>\n";
  
  ptr +="<p>Temperature: ";
  ptr +=(int)Temperaturestat;
  ptr +="°C</p>";
  ptr +="<p>Humidity: ";
  ptr +=(int)Humiditystat;
  ptr +="%</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;}
  
  void gettemperature() {
  unsigned long currentMillis = millis();
 if(currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   
    humidity_percent = dht.readHumidity();          // Read humidity (percent)
    temp_celsius = dht.readTemperature(false);     // Read temperature as Celsius
    if (isnan(humidity_percent) || isnan(temp_celsius)) {
      humidity_percent=0;
      temp_celsius=0;
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}