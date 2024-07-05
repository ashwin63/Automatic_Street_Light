/*  Accesspoint - station communication without router
 *  see: https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-class.rst
 *       https://github.com/esp8266/Arduino/blob/master/doc/esp8266wifi/soft-access-point-examples.rst
 *       https://github.com/esp8266/Arduino/issues/504
 *  Works with: station_bare_01.ino
 */ 


#include <ESP8266WiFi.h>
#include<stdio.h>
#include<string.h>
#include<WiFiUdp.h>
#include<ArduinoJson.h>
char const* ssid = "server";
char const* password = "123456789";
char const* ssid_ST = "ntp";
char const* pass_ST = "hareesh241998";
char const* colon = ":";
char const* zero = "0";
int id=1;
String p;
char timeStamp[50];
unsigned int localport = 443;
boolean value=true;
#define BULLTIN_LED 5;
WiFiServer server(80);
WiFiUDP udp;
char minute[50];
char hour[50];
char second[50];

IPAddress timeServerIP;
const char* ntpServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
String stat= "OFF";

void setup() {
  pinMode(BUILTIN_LED, INPUT); 
  Serial.begin(9600);
    WiFi.mode(WIFI_AP_STA);         
  WiFi.softAP(ssid, password);  
  server.begin();
  WiFi.begin(ssid_ST, pass_ST);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
}


void ntp_time()
{
  for(int i = 0 ; i < 20 ; i++)
  {
    timeStamp[i] = '\0';
  }
  udp.begin(localport);
  WiFi.hostByName(ntpServerName, timeServerIP);

  sendNTPpacket(timeServerIP);
  delay(1000);

  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
   // Serial.print("packet received, length=");
    //Serial.println(cb);
    udp.read(packetBuffer, NTP_PACKET_SIZE); 

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    //Serial.print("Seconds since Jan 1 1900 = " );
    //Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    //Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    //Serial.println(epoch);



    sprintf(hour,"%d",((epoch  % 86400L) / 3600)) ; 
    //Serial.print(':');
    strcat(timeStamp , hour);
    strcat(timeStamp , colon);
    if ( ((epoch % 3600) / 60) < 10 ) {
      
      strcat(timeStamp , zero);
      //sprintf(minute,"%d",0)
    }
    sprintf(minute,"%d",((epoch  % 3600) / 60));
    strcat(timeStamp , minute);// print the minute (3600 equals secs per minute)
    //Serial.print(':');
    strcat(timeStamp , colon);
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      //Serial.print('0');
      strcat(timeStamp , zero);
    }
    sprintf(second,"%d",epoch % 60);
    strcat(timeStamp , second);// print the second
  }
  // wait ten seconds before asking for the time again
  delay(100);
}


unsigned long sendNTPpacket(IPAddress& address)
{
  //Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void loop() {
   ntp_time();
   Serial.println(timeStamp);
 WiFiClient client = server.available();
  if (!client) {return;}
  //digitalWrite(ledPin, LOW);
  //String request = client.readStringUntil('\r');
 char *q;
  q = strtok(timeStamp, ":");

    if(q)
    {
    if(strcmp(q,"11") == 0)
    {
       q = strtok(NULL, ":");

    if(q)
         {
          if(strcmp(q,"33") == 0)
          {
            stat = "ON";
          }
           else
         if(strcmp(q , "36") == 0)
         {
           stat = "OFF";
         }
          else
         if(strcmp(q , "37") == 0)
         {
           stat = "OFF";
         }
         }
        
    }
    }
   
 
  p="{\"status\":\""+stat+"\"}";
  //Serial.println("********************************");
  //Serial.println("From the station: " + request);
  //client.flush();
 
 // delay(1000);
  String replyLine=client.readString();
  Serial.println("reply from client "+replyLine);
  DynamicJsonBuffer jsonBuffer(500);
  JsonObject& root=jsonBuffer.parseObject(replyLine);
  const char *fault = root["fault"];
  Serial.println("client fault status ");
  Serial.println(fault);

   Serial.println(stat);
  //Serial.print("Byte sent to the station: "+p);
  client.println(p);
  //digitalWrite(ledPin, HIGH);
  //Serial.print(timeStamp);
}
