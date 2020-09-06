#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"
#include <ArduinoJson.h>

DHT dht;

#define DHT11_PIN 2

ESP8266WebServer server(80);

const char* ssid = "Xfinity15";
const char* password = "20162016";

int red = D0;
int green = D1;
int blue = D2;

int redC = 1023;
int greenC = 1023;
int blueC = 1023;

long cumulativeTime = 0;
long timeOn = 0;
boolean maskOn = false;

float temp = 0;
float humid = 0;
float baseHumid = 60;
boolean baseSet = false;

void setup() {
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  analogWrite(red, redC);
  analogWrite(green, greenC);
  analogWrite(blue, blueC);
  dht.setup(D4);

  Serial.begin(115200);
  WiFi.begin(ssid, password);  //Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
    delay(500);
    Serial.println("Waiting to connect...");
  }
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP

  server.on("/body", HTTP_POST, handleBody); //Associate the handler function to the path
  server.on("/sensor", HTTP_GET, handleSensor);
  server.onNotFound(handleNotFound);
  server.begin(); //Start the server
  Serial.println("Server listening");

  //Get sensor data
  baseHumid = updateHumid();
  humid = baseHumid;
  updateTemp();
}
void updateState() {
  if (humid > baseHumid * 1.2) {
    if (!maskOn) {
      redC = 0;
      timeOn = millis();
      maskOn = true;
    }
  }
  else {
    if (maskOn) {
      redC = 1023;
      cumulativeTime += millis() - timeOn;
      timeOn = 0;
      maskOn = false;
    }
  }
}
void updateMetrics() {
  float humid =  updateHumid();
  if (!baseSet) {
    baseHumid = humid;
    baseSet = true;
  }
  updateTemp();
}

float updateTemp() {
  float pastTemp = temp;
  float newTemp = dht.getTemperature();
  if (isnan(newTemp)) {
    return pastTemp;
  }
  temp = newTemp;
  return temp;
}

float updateHumid() {
  float pastHumid = humid;
  float newHumid = dht.getHumidity();
  if (isnan(newHumid)) {
    return pastHumid;
  }
  humid = newHumid;
  return humid;
}

void loop() {
  analogWrite(red, redC);
  analogWrite(green, greenC);
  analogWrite(blue, blueC);
  updateMetrics();
  updateState();
  server.handleClient();
  delay(dht.getMinimumSamplingPeriod());
}

void handleSensor() {
  String data = "";
  data += "Temperature: " + String(updateTemp()) + "\n";
  data += "Humidity: " + String(updateHumid()) + "\n";
  data += "Time worn " + String(cumulativeTime) + "\n";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Max-Age", "10000");
  server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
  server.send(200, "text/plain", data);
}

void handleBody() { //Handler for the body path
  if (server.hasArg("plain") == false) { //Check if body received
    server.send(200, "text/plain", "Body not received");
    return;
  }
  String data = server.arg("plain");
  String message = "Body received:\n";
  message += data;
  message += "\n";

  if (data.indexOf("led-on") != -1) {
    redC = 0;
    greenC = 0;
    blueC = 1023;
  }
  if (data.indexOf("led-off") != -1) {
    redC = 1023;
    greenC = 1023;
    blueC = 0;
  }
  server.send(200, "text/plain", message);
  Serial.println(message);
}
void handleNotFound()
{
  if (server.method() == HTTP_OPTIONS)
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Max-Age", "10000");
    server.sendHeader("Access-Control-Allow-Methods", "POST,GET,OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "*");
    server.send(204);
  }
  else
  {
    server.send(404, "text/plain", "");
  }
}
