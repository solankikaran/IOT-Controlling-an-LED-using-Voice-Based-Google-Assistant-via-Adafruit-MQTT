
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
//#include <WiFiManager.h> 
/************************* WiFi Access Point *********************************/

const char* ssid = "SolankiFamily";
const char* password = "asdfasdf123";

/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    " "                    // Your Adafruit Username
#define AIO_KEY         " "                    //Your Adafruit AIO_Key

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feeds for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish led1status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led1stat");
Adafruit_MQTT_Publish led2status = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/led2stat");
Adafruit_MQTT_Publish relaystatus = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/relaystat");

// Setup feeds for subscribing to changes.
Adafruit_MQTT_Subscribe led1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led1");
Adafruit_MQTT_Subscribe led2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/led2");
Adafruit_MQTT_Subscribe relay = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/bulb");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() 
{
            pinMode(16, OUTPUT); 
            pinMode(4, OUTPUT); 
            pinMode(0, OUTPUT);
            pinMode(2, OUTPUT);
             
            Serial.println("MQTT demo");
            
            // Initializing serial port for debugging purposes
            Serial.begin(115200);
// WiFiManager wifiManager;
// wifiManager.autoConnect("AutoConnectAP");
 
            // Connecting to WiFi network
            Serial.println();
            Serial.print("Connecting to SSID: ");
            Serial.println(ssid);
      
            // Sending WiFi SSID & Password
            WiFi.begin(ssid, password);
            delay(500);
      
            // Obtaining Status of WiFiconnection
            while (WiFi.status() != WL_CONNECTED) 
            {
              Serial.print(".");
              digitalWrite(16,HIGH);
              delay(100);   
              digitalWrite(16,LOW);
              delay(100);
            }
            //LED at D0 ON always-Denoted that the device is connected to WiFi
            //digitalWrite(16,HIGH); 
            
            //On Successful connection to WiFi
            Serial.println("");
            Serial.println("WiFi successfully connected!");
            delay(500);
            
            // Printing the obtained IP address
            Serial.println("Obtained IP is: ");
            Serial.println(WiFi.localIP());
            delay(500);         
             
            // Setup MQTT subscription for onoff feed.
            mqtt.subscribe(&led1);
            mqtt.subscribe(&led2);
            mqtt.subscribe(&relay);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() 
{
                int8_t ret;
                // Stop if already connected.
                if (mqtt.connected()) 
                {
                  return;
                }
              
                Serial.print("Connecting to MQTT... ");
              
                uint8_t retries = 5;
                while ((ret = mqtt.connect()) != 0) 
                { // connect will return 0 for connected
                     Serial.println(mqtt.connectErrorString(ret));
                     Serial.println("Retrying MQTT connection in 5 seconds...");
                     mqtt.disconnect();
                     delay(5000);  // wait 5 seconds
                     retries--;
                     if (retries == 0) {
                       // basically die and wait for WDT to reset me
                       while (1);
                     }
                }
                
                Serial.println("MQTT Connected!");
                //LED AT D2 WILL BLINK DENOTING SUCCESSFUL MQTT CONNECTION
                for(int i=0;i<3;i++)
                {
                digitalWrite(2,HIGH);
                delay(100);   
                digitalWrite(2,LOW);
                delay(100);
                }
}


void loop() 
{
              // Ensure the connection to the MQTT server is alive (this will make the first
              // connection and automatically reconnect when disconnected).  See the MQTT_connect
              // function definition further below.
              MQTT_connect();
            
              // this is our 'wait for incoming subscription packets' busy subloop
              // try to spend your time here
            
              Adafruit_MQTT_Subscribe *subscription;
              while ((subscription = mqtt.readSubscription(5000))) 
              {
                if (subscription == &led1) 
                {
                  Serial.print(F("LED: "));
                  Serial.println((char *)led1.lastread);
                  char *ledstatus1=(char *)led1.lastread;
                  
                  String ledstatus11=String(ledstatus1);
                  if (ledstatus11=="ON")
                  {
                      digitalWrite(4,HIGH);    
                      led1status.publish("LED 1 TURNED ON");             
                  }
                  else if (ledstatus11=="OFF")
                  {
                      digitalWrite(4,LOW);   
                      led1status.publish("LED 1 TURNED OFF");                  
                  }
                  
                }

                if (subscription == &led2) 
                {
                  Serial.print(F("LED: "));
                  Serial.println((char *)led2.lastread);
                  char *ledstatus2=(char *)led2.lastread;
                  
                  String ledstatus22=String(ledstatus2);
                  if (ledstatus22=="ON")
                  {
                      digitalWrite(16,HIGH);
                      led2status.publish("LED 2 TURNED ON");                   
                  }
                  else if (ledstatus22=="OFF")
                  {
                      digitalWrite(16,LOW);  
                      led2status.publish("LED 2 TURNED OFF");                 
                  }
                }

                if (subscription == &relay) 
                {
                  Serial.print(F("RELAY: "));
                  Serial.println((char *)relay.lastread);
                  char *relaystatus1=(char *)relay.lastread;
                  
                  String relaystatus11=String(relaystatus1);
                  if (relaystatus11=="ON")
                  {
                      digitalWrite(0,HIGH);    
                      relaystatus.publish("RELAY TURNED ON");             
                  }
                  else if (relaystatus11=="OFF")
                  {
                      digitalWrite(0,LOW);   
                      relaystatus.publish("RELAY TURNED OFF");                  
                  }
                  
                }
               
              }
                  
       
            
              // ping the server to keep the mqtt connection alive
              // NOT required if you are publishing once every KEEPALIVE seconds
              /*
              if(! mqtt.ping()) {
                mqtt.disconnect();
              }
              */
}
