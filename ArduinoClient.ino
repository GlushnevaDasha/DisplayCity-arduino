#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <iarduino_Pressure_BMP.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <SPI.h>
#include <float.h>

#define AIn A0
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 103 };// IP ардуинк
byte server[] = { 192, 168, 0, 101 }; // Мой сервер { 192, 168, 0, 101 };
float tempStart, temp, gasStart, humStart, pressStart, gas, hum, pres;
String macStr,data = "";

EthernetClient client;

void addinfo(String data)
{
  Serial.println("connecting...");
  if (client.connected()) {
    Serial.println("connected");
    client.println("POST /temp HTTP/1.1");
    client.println("Host: 192.168.0.101:3000");
    client.println("User-Agent: Arduino/1.6.4");
    client.println("Accept: application/json");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println(data);
  } else {
    Serial.println("connection failed");
  }
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  /*  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }*/
  // delay (2000);
}

void setup()
{
  pinMode(AIn, INPUT);
  dht.begin();
  bmp.begin();
  pinMode(12, OUTPUT);
  pinMode(12, LOW);
  Ethernet.begin(mac, ip);
  Serial.begin(9600);
  client.connect(server, 3000);
  delay(2000);
  humStart = dht.readHumidity();
  tempStart = (dht.readTemperature() + bmp.readTemperature()) / 2;
  pressStart = 0.0075 * bmp.readPressure();
  gasStart = analogRead(AIn);
  macStr = retMac(mac);
  data = "{\"mac\":" + macStr + "{\"temp\":" + (String)tempStart + ",\"hum\":" + (String)humStart + ",\"press\":" + (String)pressStart + ",\"gas\":" + (String)gasStart + "}";
  addinfo(data);
}

void loop()
{
  delay(2000);
  hum = dht.readHumidity();
  pres = 0.0075 * bmp.readPressure();
  temp = (dht.readTemperature() + bmp.readTemperature()) / 2;
  int gass = analogRead(AIn);
  if (temp != tempStart || pres != pressStart || hum != humStart || gas != gasStart)
  {
    data = "{\"mac\":" + macStr +"\"temp\":" + (String)temp + ",\"hum\":" + (String)hum + ",\"press\":" + (String)pres + ",\"gas\":" + (String)gas + "}";
    addinfo(data);
    humStart = hum;
    tempStart = temp;
    pressStart = pres;
    gasStart = gas;
  }
}

String retMac(byte mac[])
{
  String macString = "";
  for(int i=0;i<6;i++)
  {
    macString += (String)mac[i];
  }
  return macString;
}

