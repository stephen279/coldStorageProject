#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>
// include library to read and write from flash memory
#include <EEPROM.h>
#include <Adafruit_Sensor.h>

#include "DHT.h"
#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

const char* host = "www.mystuffsite.com";

#include <DHT.h>
#include <DHT_U.h>

#include <SimpleDHT.h>

// define the number of bytes you want to access
#define EEPROM_SIZE 1

//#define DHTPIN D1
#define DHTTYPE DHT11 



void writeString(char add,String data);
String read_String(char add);

String userState;

String feel;

static const char addonJson[] PROGMEM = R"raw(
{
  "title": "Hello",
  "uri": "/hello",
  "menu": true,
  "element": [
    {
      "name": "feels",
      "type": "ACInput",
      "label": "Your Account Username"
    },
    {
      "name": "send",
      "type": "ACSubmit",
      "value": "Link Your Account",
      "uri": "/feels"
    }
  ]
}
)raw";

WebServer webServer;
AutoConnect portal(webServer);

// Here, /feels handler
void feelsOn() {

  // Retrieve the value of a input-box named "feels"
   feel = webServer.arg("feels");
    writeString(10, feel);  //Address 10 and String type data

  // Echo back the value
  String echo = "<html><p style=\"color:blue;font-family:verdana;font-size:300%;\">" + feel + String(" your account is linked </p></html>");
  webServer.send(200, "text/html", echo);
}

void setup() {
  EEPROM.begin(512);
   Serial.begin(115200);
  delay(1000);
   Serial.println("inside Setup");
    String recivedData;
  recivedData = read_String(10);
  Serial.print("Loop Read Data:");
  Serial.println(recivedData);
  delay(1000);

   
//   userState = EEPROM.read(0);
  webServer.on("/feels", feelsOn);  // Register /feels handler


  portal.load(addonJson);           // Load a custom Web page
  portal.begin();
}

void loop() {


  
  portal.handleClient();
 

// put your main code here, to run repeatedly:
  String recivedData;
  recivedData = read_String(10);
  Serial.print("Loop Read Data:");
  Serial.println(recivedData);
  delay(1000);

  
 float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  
  int i = 12345;
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  
  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  
   WiFiClient client1;
   
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection client  failed");
    return;
  }
   
  if (!client1.connect(host, httpPort)) {
    Serial.println("connection client1 failed");
    return;
  }
  

   String url = "http://www.mystuffsite.com/weather/esp/insert.php?temp=" + String(t) + "&hum="+ String(h)+"&device_id="+ String(i);
   String urlUser = "http://www.mystuffsite.com/weather/esp/insertUserDevice.php?device_id="+ String(i) +"&username="+ String(recivedData);
   
  Serial.print("Requesting usersDevice URL: ");
   Serial.println(urlUser);
  client.print(String("GET ") + urlUser + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +  
               "Connection: close\r\n\r\n");
  Serial.print("Requesting users URL: ");
   Serial.println(url);
  client1.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +  
               "Connection: close\r\n\r\n");
               
     while(client.available()){
    String line = client.readStringUntil('\r');
   // Serial.print("inside while loop");
    Serial.print(line);

  }

   while(client1.available()){
    String line1 = client1.readStringUntil('\r');
   // Serial.print("inside while loop");
    Serial.print(line1);

  }

 /* delay(19500);
//for urlUser
  Serial.print("Requesting New user URL: ");
  Serial.println(urlUser);
  client1.print(String("GET ") + urlUser + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +  
               "Connection: close\r\n\r\n");


 while(client1.available()){
    String line1 = client1.readStringUntil('\r');
    Serial.print(line1);
  }
    delay(19500);
  */ 
}




void writeString(char add,String data)
{
   Serial.print("wrieString:");
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
   Serial.print("Committed success");
}


String read_String(char add)
{
   Serial.print("Read_String:");
  int i;
  char data[100]; //Max 100 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<500)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}
