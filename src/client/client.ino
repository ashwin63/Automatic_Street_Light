

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#define acs712 A0

long lastSample = 0;
long sampleSum = 0;
long sampleCount = 0;

float vpc = 4.8828125;
char ssid[] = "server";           // SSID of your AP
char pass[] = "123456789";         // password of your AP
   // IP address of the AP
WiFiClient client;
float power = 0;
String fault = "NO";
String p;
void setup() {
   pinMode(D4, OUTPUT);
   digitalWrite(D4, LOW);
  //Serial.begin(9600);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);           // connects to the WiFi AP
  Serial.println();
  Serial.println("Connection to the AP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

}

void loop() {
  client.connect("192.168.4.1", 80);
  
 
  String answer = client.readString();
  DynamicJsonBuffer jsonBuffer(500);
  JsonObject& root = jsonBuffer.parseObject(answer);
  const char* status = root["status"];
  Serial.println(status);
  if(strcmp(status,"ON")==0)
  {
     digitalWrite(D4, HIGH);
     
  }
  else
  if(strcmp(status,"OFF")==0)
   digitalWrite(D4, LOW); 
power = power_calc();

if(sampleCount == 1000)
{
  if((strcmp(status,"ON") == 0 ) && power == 0)
  {
    fault = "YES";
  }
  else
  if((strcmp(status,"ON") == 0 ) && power > 0)
  {
    fault = "NO";
  }else
  if((strcmp(status,"ON") == 0 ) && power ==null)
  {
    fault = "PRO";
  }
  else
  fault = "NO";
}

 p="{\"status\":\""+fault+"\"}";
  client.println(p);

  
  client.flush();
  client.stop();
  delay(200);
}

float power_calc()
{
  float powe = 0;
  if(millis() > lastSample + 1) {
  sampleSum += sq(analogRead(acs712) - 512);
  sampleCount ++;
  lastSample = millis();
}

if(sampleCount == 1000) {
  float mean = sampleSum / sampleCount;
  float value = sqrt(mean);

  float mv = value * vpc;

  float amps = mv / 100;
  
  //Serial.println("RMS value :" + String(value));
  //Serial.println("RMS voltage :" + String(mv));
  //Serial.println("RMS amps :" + String(amps));
  //Serial.println("RMS watts :" + String(amps * 220));
  powe = amps * 220;
  sampleCount = 0;
  sampleSum = 0;
}
return (powe);
}
