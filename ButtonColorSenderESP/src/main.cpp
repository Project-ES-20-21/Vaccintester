#include <Arduino.h>
#include <SPI.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#include <bits/stdc++.h>
#include <vector>
#include <algorithm>

#include "PubSubClient.h"
#include "Wire.h"
#include "SPI.h"

//Mqtt constants
#define SSIDA "NETGEAR68"
#define PWD "excitedtuba713"
#define MQTT_SERVER "192.168.1.2" // could change if the setup is moved
#define MQTT_PORT 1883

WiFiClient espClient;
PubSubClient client(espClient);

#define S0 15
#define S1 2
#define S2 22
#define S3 4
#define EO 21
#define sensorOut 5

#define inPin 12

int frequency = 0;
int kleur = 6;

int val = 0;

//Omzetten gemeten RGB-waarden naar een kleur.
int colorChecker(int redFrequency, int greenFrequency, int blueFrequency)
{
  kleur = 6;

  if (redFrequency < 420 && redFrequency > 290 && greenFrequency > 300 && greenFrequency < 360 && blueFrequency > 360 && blueFrequency < 430)
  {
    Serial.println("Groen");
    kleur = 0;
  }

  if (redFrequency < 360 && redFrequency > 250 && greenFrequency > 600 && greenFrequency < 7500 && blueFrequency > 460 && blueFrequency < 560)
  {
    Serial.println("Rood");
    kleur = 1;
  }

  if (redFrequency < 720 && redFrequency > 630 && greenFrequency > 480 && greenFrequency < 570 && blueFrequency > 290 && blueFrequency < 370)
  {
    Serial.println("Blauw");
    kleur = 2;
  }

  return kleur;
}

int measure()
{
  digitalWrite(EO, LOW);
  // Setting red filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Reading the output frequency
  int red = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //int red = map(frequency, 25,72,255,0);
  // Printing the value on the serial monitor
  Serial.print("R= "); //printing name
  Serial.print(red);   //printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Green filtered photodiodes to be read
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  int green = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //int green = map(frequency, 30,90,255,0);
  // Printing the value on the serial monitor
  Serial.print("G= "); //printing name
  Serial.print(green); //printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Reading the output frequency
  int blue = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  //int blue = map(frequency, 25,70,255,0);
  // Printing the value on the serial monitor
  Serial.print("B= "); //printing name
  Serial.print(blue);  //printing RED color frequency
  Serial.println("  ");
  delay(100);
  digitalWrite(EO, HIGH);
  return colorChecker(red, green, blue);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int send = 0;
int measColor;

void callback(char *topic, byte *message, unsigned int length);

void setup_wifi()
{
  delay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSIDA, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println();
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

// function to establish MQTT connection
void reconnect()
{
  delay(10);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32vaccinbutton1"))
    {
      Serial.println("connected");

      // ... and resubscribe
      client.subscribe("esp32/vaccin/pause");
      client.subscribe("esp32/vaccin/reset");
      client.subscribe("esp32/vaccin/number");
      client.subscribe("esp32/vaccin/button1");
      client.subscribe("esp32/vaccin/button2");
      client.subscribe("esp32/vaccin/kast");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{

  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  pinMode(inPin, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  val = digitalRead(inPin); // read input value
  if (val == HIGH)
  {
    measColor = measure();
    send = true;
  }

  if ((send == true))
  {
    String number = (String)measColor;
    const char *sendColor = number.c_str();
    client.publish("esp32/vaccin/button1", sendColor);
    Serial.println("Color sent");
    send = false;
  }
}
