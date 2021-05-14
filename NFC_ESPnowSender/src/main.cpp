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

using namespace std;

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

unsigned long myPrevMillis;

void myDelay(int del)
{
  myPrevMillis = millis();
  while (millis() - myPrevMillis <= del)
  {
    client.loop();
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// SETUP NFC /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void startListeningToNFC();
static String handleCardDetected();

// Pins used for I2C IRQ
#define PN532_IRQ 4
#define PN532_RESET 5

//Pinnen leds
int red_color = 25;
int blue_color = 26;
int green_color = 27;
int yellow_color = 32;
int purple_color = 33;
int cyan_color = 14;

void setupPins()
{
  //Setup voor aansturen leds
  pinMode(blue_color, OUTPUT);
  pinMode(red_color, OUTPUT);
  pinMode(green_color, OUTPUT);
  pinMode(cyan_color, OUTPUT);
  pinMode(purple_color, OUTPUT);
  pinMode(yellow_color, OUTPUT);
}

const int DELAY_BETWEEN_CARDS = 500;
long timeLastCardRead = 0;
boolean readerDisabled = false;
int irqCurr;
int irqPrev;

//black Peter
string valid_tags = "0X04 0X9C 0X6E 0X6A 0X99 0X5B 0X80";

string reset_tags = "0X83 0X63 0XB3 0X03 0X00 0X00 0X00";

String hexToString(uint8_t *cardid);
boolean control(string kaart);

// This example uses the IRQ line, which is available when in I2C mode.
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void startListeningToNFC()
{
  // Reset our IRQ indicators
  irqPrev = irqCurr = HIGH;

  Serial.println("Present an ISO14443A Card ...");
  nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
}

String handleCardDetected()
{
  uint8_t success = false;
  uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0}; // Buffer to store the returned UID
  uint8_t uidLength;                     // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // read the NFC tag's info
  success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
  Serial.println(success ? "Read successful" : "Read failed (not a card?)");

  if (success)
  {
    timeLastCardRead = millis();
  }

  // The reader will be enabled again after DELAY_BETWEEN_CARDS ms will pass.
  readerDisabled = true;
  return hexToString(uid);
}

String hexToString(uint8_t *cardid)
{

  int length = 7; //zonder +3 worden slechts 4 bytes genomen -> moet opgelost worden
  stringstream ss;
  for (int i = 0; i < length - 1; i++)
  {
    ss << "0x";
    if ((int)cardid[i] <= 15)
    {
      ss << "0";
    }
    ss << hex << (int)cardid[i];
    ss << " ";
  }
  ss << "0x";
  if ((int)cardid[length - 1] <= 15)
  {
    ss << "0";
  }
  ss << hex << (int)cardid[length - 1];
  string mystrC = ss.str();
  String mystr = mystrC.c_str();
  mystr.toUpperCase();
  Serial.println(mystr.c_str());
  return mystr;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// MQTT ///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int send;
int measColor;
String number;
int card = 0;
int volgorde[7];

void callback(char *topic, byte *message, unsigned int length);

void setup_wifi()
{
  myDelay(10);
  Serial.println("Connecting to WiFi..");

  WiFi.begin(SSIDA, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    myDelay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void ledOff()
{
  digitalWrite(green_color, LOW);
  digitalWrite(yellow_color, LOW);
  digitalWrite(red_color, LOW);
  digitalWrite(purple_color, LOW);
  digitalWrite(blue_color, LOW);
  digitalWrite(cyan_color, LOW);
}
void nullSequence()
{
  for (int i = 0; i < 7; i++)
  {
    volgorde[i] = 7;
  }
}

void Ledup(int nrLed, int delay0ff, int delayOn)
{
  digitalWrite(nrLed, HIGH);
  myDelay(1500);
  digitalWrite(nrLed, LOW);
  myDelay(300);
}

void ledReset(){
  Ledup(green_color,50,0);
  Ledup(red_color,50,0);
  Ledup(blue_color,50,0);
  Ledup(yellow_color,50,0);
  Ledup(purple_color,50,0);
  Ledup(cyan_color,50,0);
  Ledup(cyan_color,50,0);
  Ledup(purple_color,50,0);
  Ledup(yellow_color,50,0);
  Ledup(blue_color,50,0);
  Ledup(red_color,50,0);
  Ledup(green_color,50,0);
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

  if (String(topic) == "esp32/vaccin/control")
  {
    if (messageTemp == "0")
    {
      Serial.println("Reset");      
      ledReset();
      card=0;
      ESP.restart();
    }
  }
}

// function to establish MQTT connection
void reconnect()
{
  myDelay(10);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32vaccinkast"))
    {
      Serial.println("connected");

      // ... and resubscribe
      client.subscribe("esp32/vaccin/control");
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
      myDelay(5000);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////// SETUP ///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  Serial.println("Begin NFC532 Scanning Software.");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata)
  {
    Serial.print("Didn't find PN532 board");
    while (1)
      ; // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  Serial.println((versiondata >> 24) & 0xFF, HEX);
  Serial.print("Firmware ver. ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  // configure board to read RFID tags
  nfc.SAMConfig();

  startListeningToNFC();

  setupPins();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////// METHODES /////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//makes sequence
void sequence()
{
  srand(time(NULL));
  for (int i = 0; i < 7; i++)
  {
    volgorde[i] = rand() % 6;
  }
}

void LedupAll(int delay0ff, int delayOn)
{
  myDelay(500);
  digitalWrite(green_color, HIGH);
  digitalWrite(yellow_color, HIGH);
  digitalWrite(red_color, HIGH);
  digitalWrite(purple_color, HIGH);
  digitalWrite(blue_color, HIGH);
  digitalWrite(cyan_color, HIGH);
  myDelay(500);
  digitalWrite(green_color, LOW);
  digitalWrite(yellow_color, LOW);
  digitalWrite(red_color, LOW);
  digitalWrite(purple_color, LOW);
  digitalWrite(blue_color, LOW);
  digitalWrite(cyan_color, LOW);
}


void ledBlink()
{
  Serial.println("Ledblink");
  //De juiste sequentie weergeven via de leds
  /*
  green = 0
  red = 1
  blue = 2
  yellow = 3
  purple = 4
  cyan = 5
  */
  for (int t = 0; t < 7; t++)
  {
    if (volgorde[t] == 0)
    {
      Serial.println("0");
      Ledup(green_color, 300, 1500);
    }
    if (volgorde[t] == 1)
    {
      Serial.println("1");
      Ledup(red_color, 300, 1500);
    }
    if (volgorde[t] == 2)
    {
      Serial.println("2");
      Ledup(blue_color, 300, 1500);
    }
    if (volgorde[t] == 3)
    {
      Serial.println("3");
      Ledup(yellow_color, 300, 1500);
    }
    if (volgorde[t] == 4)
    {
      Serial.println("4");
      Ledup(purple_color, 300, 1500);
    }
    if (volgorde[t] == 5)
    {
      Serial.println("5");
      Ledup(cyan_color, 300, 1500);
    }
  }

  for (int t = 0; t < 2; t++)
  {
    LedupAll(500, 500);
  }
}

//Controleren als juiste kaart
boolean control(string kaart, int check)
{
  if ((kaart.compare(valid_tags.c_str()) == 0) && check == 0)
  {
    number = "";
    sequence();
    card = 1;
    send = true;
    return true;
  }
  if ((kaart.compare(reset_tags.c_str()) == 0))
  {
    number = "";
    client.publish("esp32/vaccin/control", "0");
    nullSequence();
    ledOff();
    card = 0;
    return true;
  }
  else
  {
    return false;
  }
}

int teller;
String id;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// LOOP //////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  if (readerDisabled)
  {
    if (millis() - timeLastCardRead > DELAY_BETWEEN_CARDS)
    {
      readerDisabled = false;
      startListeningToNFC();
    }
  }
  else
  {
    irqCurr = digitalRead(PN532_IRQ);

    // When the IRQ is pulled low - the reader has got something for us.
    if (irqCurr == LOW && irqPrev == HIGH)
    {
      //Serial.println("Got NFC IRQ");
      id = handleCardDetected();
      control(id.c_str(), card);
      teller = 0;
    }

    irqPrev = irqCurr;
  }

  if (send == true && teller < 1)
  {
    {
      for (int i = 0; i < 7; i++)
      {
        number = number + (String)volgorde[i];
      }
      const char *kleurenvolgorde = number.c_str();
      client.publish("esp32/vaccin/kast", kleurenvolgorde);
      Serial.println("Sequence sent");
      send = false;
    }
  }

  if (card == 1)
  {
    ledBlink();
  }
}
