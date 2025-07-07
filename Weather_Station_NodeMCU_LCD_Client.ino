#include <LiquidCrystal_I2C.h>

/* 
DHT Weather client with display integrated for NodeMCU esp8266 and 1602A LCD

Please connect 1602A using I2C backpack.
If you feel display is not showing anything, please take a screwdriver and update contrast 
using I2C potentiometer in back of LCD

LCD config
  SDA pin - D2
  SCL pin - D1
  VCC Pin - 5v
  
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //This library you can add via Include Library > Manage Library > 
LiquidCrystal_I2C lcd(0x3F, 16, 2);

/*Put your SSID & Password*/
const char* ssid     = "SSID";
const char* password = "Password";

String serverName = "http://192.168.1.10/api";  // DHT sensor server IP address. In our example Server IP = 192.168.1.10

// Set your Static IP address
IPAddress local_IP(192, 168, 1, 20);  // Example Static IP = 192.168.1.20
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);  // Example Gateway = 192.168.1.1

IPAddress subnet(255, 255, 255, 0); // Example Subnet Mask = 255.255.255.0
IPAddress primaryDNS(8, 8, 8, 8);   // Optional
IPAddress secondaryDNS(8, 8, 4, 4); // Optional

int humidity, temp;  // Values read from sensor

unsigned long lastTime = 0;
unsigned long timerDelay = 15100;

String jsonBuffer;

void setup() {
  lcd.init();   // Initializing the LCD
  lcd.backlight();
  Serial.begin(9600);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
// Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = serverName + "&field1=" + temp + "&field2=" + humidity;
      Serial.println(serverPath);
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
// JSON.typeof(jsonVar) can be used to get the type of the var
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
    }

  if (myObject["Temperature"] == null) {
    String err01="Connection to ";
    String err02="Server lost... ";
    lcd.setCursor(0, 0);
    lcd.print(err01);
    lcd.setCursor(0, 1);
    lcd.print(err02);  
  return;
  }

    Serial.print("JSON object = ");
    Serial.println(myObject);
    Serial.print("Temperature: ");
    Serial.println(myObject["Temperature"]);
    Serial.print("Humidity: ");
    Serial.println(myObject["Humidity"]);
    
    temp = int(myObject["Temperature"]);
    humidity = int(myObject["Humidity"]);

    String ts="Temperat: "+String(temp)+" C ";
    String hs="Humidity: "+String(humidity)+" % ";
    lcd.setCursor(0, 0);
    lcd.print(ts);
    lcd.setCursor(0, 1);
    lcd.print(hs); 
    }
  else {
    Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  HTTPClient http;
  WiFiClient WiFiClient;
    
// Your IP address with path or Domain name with URL path 
  http.begin(WiFiClient, "http://192.168.1.10/api");  // DHT sensor server IP address. In our example Server IP = 192.168.1.10
  
// Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}
