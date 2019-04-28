#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <Time.h>
#include <TimeLib.h>
#include "HX711.h"
#include <Wire.h>
#include <SparkFunMLX90614.h>




// How many bits to use for temperature values: 9, 10, 11 or 12
#define SENSOR_RESOLUTION 9
// Index of sensors connected to data pin, default: 0
#define SENSOR_INDEX 0

#define DOUT1  2
#define CLK1  3
#define DOUT2  4
#define CLK2  5
#define DOUT3 6 
#define CLK3 7 
#define DOUT4 8 
#define CLK4  9

#define ALTITUDE 110.0

HX711 scale1(DOUT1, CLK1);
HX711 scale2(DOUT2, CLK2);
HX711 scale3(DOUT3, CLK3);
HX711 scale4(DOUT4, CLK4);

float calibration_factor1 = -865;
float calibration_factor2 = -900;
float calibration_factor3 = -830;
float calibration_factor4 = -980;

 float ambienttherm = 0;
 float objecttherm = 0;
 float weight1 = 0;
 float weight2 = 0;
 float weight3 = 0;
 float weight4 = 0;

char TEMPERATURE = 'C';

IRTherm therm;

byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

const char* server = "your_own_server";
const char* username = "your_own_username";


EthernetClient ethClient;
PubSubClient client(ethClient);



// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  Serial.print("test");
  //Ethernet.begin(mac);
   // print your local IP address:
  //Serial.print("My IP address: ");
  //IPAddress ip = Ethernet.localIP();
  /*
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
     print the value of each byte of the IP address:
     Serial.print(ip[thisByte], DEC);
     Serial.print(".");
  }
  */
  delay(1500);

  client.setServer(server, 1883);
  
   if (client.connect("AD M 2560", username, NULL)) {
    // connection succeeded
    Serial.print("\nmqttClient connection success!");
  } else {
    Serial.print("\nmqttClient connection ERROR! ");
    Serial.print(client.state());
  }

  therm.begin(); // Initialize thermal IR sensor
  therm.setUnit(TEMP_C); //C for celsius F for fahrenheit
  delay(3000);

  scale1.set_scale();
  scale1.tare(); //Reset the scale to 0
  scale2.set_scale();
  scale2.tare();
  scale3.set_scale();
  scale3.tare();
  scale4.set_scale();
  scale4.tare();

  long zero_factor1 = scale1.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor1);
  long zero_factor2 = scale2.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor2);
  long zero_factor3 = scale3.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor3);
  long zero_factor4 = scale4.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor4);
}

// the loop function runs over and over again forever
void loop() {
  Serial.print("\n-------------------");
 
  
  scale1.set_scale(calibration_factor1);
  scale2.set_scale(calibration_factor2); 
  scale3.set_scale(calibration_factor3); 
  scale4.set_scale(calibration_factor4);

  if (therm.read()) // On success, read() will return 1, on fail 0.
  {
    // Use the object() and ambient() functions to grab the object and ambient
  // temperatures.
  // They'll be floats, calculated out to the unit you set with setUnit().
  
    
    Serial.print("Object: " + String(therm.object(), 2));
    Serial.write("degrees "); // Degree Symbol
    Serial.println("F");
    Serial.print("Ambient: " + String(therm.ambient(), 2));
    Serial.write("degrees "); // Degree Symbol
    Serial.println("F");
    Serial.println();
  }
    float ambienttherm = therm.ambient();
    float objecttherm = therm.object();
    float weight1 = scale1.get_units();
    float weight2 = scale2.get_units();
    float weight3 = scale3.get_units();
    float weight4 = scale4.get_units();
    
  delay(500);

  String payload = "{";
  payload += "\"at\":"; payload += String(ambienttherm); payload += ",";
  payload += "\"ot\":"; payload += String(objecttherm); payload += ",";
  payload += "\"w1\":"; payload += String(weight1); payload += ",";
  payload += "\"w2\":"; payload += String(weight2); payload += ",";
  payload += "\"w3\":"; payload += String(weight3); payload += ",";
  payload += "\"w4\":"; payload += String(weight4); 
  payload += "}";
  
  char attributes[120];
  payload.toCharArray( attributes, 120 );
  Serial.print("\nIs connected? ");
  Serial.print(client.connected());
  Serial.print("\nPublishing: ");
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
  
  Serial.print("\nHour ");
  Serial.print(hour());
  Serial.print("\nMin ");
  Serial.print(minute());
  Serial.print("\nSec ");
  Serial.print(second());
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  Serial.print("\nLed up ");
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  Serial.print("\nLed down ");
  delay(1000);                       // wait for a second
}
