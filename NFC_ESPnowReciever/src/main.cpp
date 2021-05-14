#include <Arduino.h>

#include <esp_now.h>
#include <WiFi.h>
#include "PubSubClient.h"
#include "Wire.h"
#include "SPI.h"
#include "Adafruit_SPIDevice.h"
#include "ESPAsyncWebServer.h"
#include <Arduino_JSON.h>
using namespace std;

#include <SPI.h>

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
/////////////////////////////////////////////////////// SETUP LEDSTRIP //////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define red 26
#define green 27
#define blue 25

#define inPin 12

// setting PWM properties
const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8; //Resolution 8, 10, 12, 15

//=======================================================================
//                    Power on setup
//=======================================================================
void setupLed()
{

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  // configure LED PWM functionalitites
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);

  // attach the channel to the GPIO2 to be controlled
  ledcAttachPin(red, redChannel);
  ledcAttachPin(green, greenChannel);
  ledcAttachPin(blue, blueChannel);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value)
{
  ledcWrite(redChannel, red_light_value);
  ledcWrite(greenChannel, green_light_value);
  ledcWrite(blueChannel, blue_light_value);
}

//int teller;
int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int kleur = 6;

/*
  green = 0
  red = 1
  blue = 2
  yellow = 3
  purple = 4
  cyan = 5
  black = 6
*/

//Met behulp van de ontvangen kleuren de gemaakt kleur bepalen
int colorMaker(int mColor1, int mColor2)
{
  
  //groen
  if(((mColor1 == 0 || mColor2 == 0) && (mColor1 == 6 || mColor2 == 6)) || (mColor1 == 2 && mColor2 == 2)) {
      kleur=0;
      RGB_color(255, 0, 255);
    }
  //rood
  if(((mColor1 == 1 || mColor2 == 1) && (mColor1 ==6 || mColor2 ==6))|| (mColor1 == 1 && mColor2 == 1)) {
      kleur=1;
      RGB_color(0,255, 255);
    }
  //blauw
  if(((mColor1 == 2 || mColor2 == 2) && (mColor1 == 6 || mColor2 == 6)) || (mColor1 == 2 && mColor2 == 2)) {
      kleur=2;
      RGB_color(255, 255, 0);
    }
  //geel
  if((mColor1 == 0 || mColor2 == 0) && (mColor1 == 1 || mColor2 == 1)){
      kleur=3;
      RGB_color(0, 0, 255);
    }
  //paars
  if((mColor1 == 1|| mColor2 == 1) && (mColor1 == 2|| mColor2 == 2)){
      kleur=4;
      RGB_color(0, 255,0);
    }
  //cyaan
  if((mColor1 == 0|| mColor2 == 0) && (mColor1 == 2|| mColor2 == 2)){
      kleur=5;
      RGB_color(255, 0, 0);
    }
  if((mColor1 == 6)&&(mColor2 == 6)){
      kleur=6;
      RGB_color(0,0,0);
  }/*
  if (mColor1 == 0)
  {
    kleur = 0;
    RGB_color(255, 0, 255);
  }
  if (mColor1 == 1)
  {
    kleur = 1;
    RGB_color(0, 255, 255);
  }
  if (mColor1 == 2)
  {
    kleur = 2;
    RGB_color(255, 255, 0);
  }*/
  return kleur;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// SETUP MQTT /////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int sequence[7];

int reset = 0;
int pauze = 0;
int digitCode;
int color1;
int color2;
String sequentiestring;

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

int pauzeFitness=0;
int pauzeOntsmet=0;

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/vaccin/control")
  {
    //0 -- Reset
    if (messageTemp == "0")
    {
      for (int i = 0; i < 10; i++)
    {
      RGB_color(0, 255, 0);
      myDelay(200);
      RGB_color(255, 255, 255);
      myDelay(200);
    }
      ESP.restart();
      //reset = true;
    }
    //1 -- Pause ontsmetting
    if (messageTemp == "1")
    {
      pauzeOntsmet=1;
    }
    //2 -- Continue ontsmetting
    if (messageTemp == "2")
    {
      pauzeOntsmet=0;
    }
    //3 -- Pause fitness
    if (messageTemp == "3")
    {
      pauzeFitness=1;
    }
    //4 -- Continue ontsmetting
    if (messageTemp == "4")
    {
      pauzeOntsmet=0;
    }
  }

  if (String(topic) == "esp32/vaccin/button1")
  {
    Serial.println("Button1");
    color1 = messageTemp.toInt();
  }

  if (String(topic) == "esp32/vaccin/button2")
  {
    Serial.println("Button2");
    color2 = messageTemp.toInt();
  }

  if (String(topic) == "esp32/vaccin/kast")
  {
    Serial.println("Kast");
    sequentiestring = messageTemp;
    Serial.println(sequentiestring);
    sequence[0] = sequentiestring.substring(0, 1).toInt();
    sequence[1] = sequentiestring.substring(1, 2).toInt();
    sequence[2] = sequentiestring.substring(2, 3).toInt();
    sequence[3] = sequentiestring.substring(3, 4).toInt();
    sequence[4] = sequentiestring.substring(4, 5).toInt();
    sequence[5] = sequentiestring.substring(5, 6).toInt();
    sequence[6] = sequentiestring.substring(6, 7).toInt();

    for (int i = 0; i < 7; i++)
    {
      Serial.println(sequence[i]);
    }
  }
  if (String(topic) == "esp32/vaccin/number")
  {
    Serial.println(messageTemp);
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
    if (client.connect("ESP32vaccin"))
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
//////////////////////////////////////////////////////////// SETUP //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  //Initialize Serial Monitor
  Serial.begin(115200);

  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  setupLed();
}

int val = 0;
int mixedColor;
int correct = 0;
int check = 0;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////// LOOP  //////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  val = digitalRead(inPin); // read input value
  if ((val == HIGH) && (correct < 7) && (pauzeOntsmet == 0) && (pauzeFitness==0))
  {
    Serial.println("push");
    kleur = colorMaker(color2, color1);
    myDelay(1000);
    if (kleur != sequence[correct])
    {
      correct = 0;
      Serial.println(sequence[correct]);
      Serial.println("fout");
      for (int i = 0; i < 3; i++)
      {
        RGB_color(0, 255, 255); //rood laten pinken
        myDelay(350);
        RGB_color(255, 255, 255);
        myDelay(350);
      }
      colorMaker(color2, color1);
    }

    else if (kleur == sequence[correct])
    {
      Serial.println("Juist");
      correct++;
      Serial.println(sequence[correct]);
      for (int i = 0; i < 3; i++)
      {
        RGB_color(255, 0, 255); //groen laten pinken
        myDelay(350);
        RGB_color(255, 255, 255);
        myDelay(350);
      }
      colorMaker(color2, color1);
    }
  }

  if ((correct == 7) && (check == 0))
  {
    //random getal aanmaken
    srand(time(NULL));
    digitCode = rand() % 10;
    Serial.println(digitCode);

    String number = (String)digitCode;
    const char *alohomora = number.c_str();
    client.publish("esp32/vaccin/number", alohomora);
    Serial.println("Alohomora sent");

    //dat getal sturen naar de centrale
    //tonen op de ledstrip door aantal keer te laten pinken in wit
    for (int j = 0; j < 15; j++)
    { //15 keer het juiste getal laten zien afgewisseld met rood pinken
      for (int i = 0; i < digitCode; i++)
      {
        RGB_color(0, 0, 0);
        myDelay(350);
        RGB_color(255, 255, 255);
        myDelay(350);
      }
      for (int i = 0; i < digitCode; i++)
      {
        RGB_color(255, 255, 0);
        myDelay(350);
        RGB_color(255, 255, 255);
        myDelay(350);
      }
    }
    check = 1;
  }
}