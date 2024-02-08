#include <SPI.h>
#include <PN532_SPI.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
PN532_SPI interface(SPI, 4); // create a PN532 SPI interface with the SPI CS terminal located at digital pin 10
NfcAdapter nfc = NfcAdapter(interface); // create an NFC adapter object
String tagId = "None";

String wifipsw = "Password";
String wifiname = "Coop1";
String brokerIp = "";
String brokerPort = "";
String idKey = "MMZLSJ";
String deviceId = "-01";
String splitKey = "+";
const int ledPin = 0;

String serverName = "http://192.168.5.177:8080/send";

const int magnetSensorPin = A0;

// Connect to Wifi
void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiname.c_str(), wifipsw.c_str());

  int counter = 0;
  bool wifiFound = true;
  while (WiFi.status() != WL_CONNECTED)
  {
     delay(500);
     Serial.print(".");
     counter++;
     if (counter >= 10) {
        wifiFound = false;
        break;
     }
  }

  randomSeed(micros());
  if (wifiFound) {
    Serial.println("");
    Serial.println("WiFi connected :)");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(ledPin, HIGH);
    delay(4000);
    digitalWrite(ledPin, LOW);
    delay(2000);
    digitalWrite(ledPin, HIGH);
    delay(2000);
    digitalWrite(ledPin, LOW);
    delay(2000);
  } else {
    Serial.println("");
    Serial.println("WiFi not connected :(");
    digitalWrite(ledPin, HIGH);
    delay(4000);
    digitalWrite(ledPin, LOW);
    delay(2000);
    digitalWrite(ledPin, HIGH);
    delay(2000);
    digitalWrite(ledPin, LOW);
    delay(2000);
    digitalWrite(ledPin, HIGH);
    delay(2000);
    digitalWrite(ledPin, LOW);
    delay(2000);
  }
  
}

void postStatus(String stat, String servNam) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, servNam.c_str());

      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.addHeader("User-Agent", "Mozilla/5.0'");

      String httpRequestData = "api_key=MMZLSJ&sensor_id=1&status=" + stat;           

      int httpResponseCode = http.POST(httpRequestData);
      
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      
      http.end();
}


void readNFC() 
{
 if (nfc.tagPresent())
 {  
    Serial.println("oh no");
    // digitalWrite(ledPin, HIGH);
    NfcTag tag = nfc.read();
    NdefMessage message = tag.getNdefMessage();
    String payloadStr = "";
    // If you have more than 1 Message then it wil cycle through them
    int recordCount = message.getRecordCount();
    for (int i = 0; i < recordCount; i++)
    {
      payloadStr = "";
      NdefRecord record = message.getRecord(i);
      int payloadLength = record.getPayloadLength();
      byte payload[payloadLength];
      record.getPayload(payload);


      String payloadAsString = ""; // Processes the message as a string vs as a HEX value
      for (int c = 3; c < payloadLength; c++) 
      {
        payloadAsString += (char)payload[c];
      }
      payloadStr = payloadAsString;      
    }
   
    if (payloadStr.substring(0,6) == idKey) {
      Serial.println("1");
      String action = payloadStr.substring(7,8);
      if (action == "F") {
        Serial.println("2");
        String payloadContent = payloadStr.substring(9);
        int charPos;
        String tempContent;
        charPos = payloadContent.indexOf(splitKey);
        deviceId = payloadContent.substring(0,charPos);
        tempContent = payloadContent.substring(charPos+1);

        charPos = tempContent.indexOf(splitKey);
        brokerIp = tempContent.substring(0,charPos);
        tempContent = tempContent.substring(charPos+1);
   
          
        charPos = tempContent.indexOf(splitKey);
        brokerPort = tempContent.substring(0,charPos);
        tempContent = tempContent.substring(charPos+1);
      
        serverName = "http://" + brokerIp + ":" + brokerPort + "/send";  
        charPos = tempContent.indexOf(splitKey);
        wifiname = tempContent.substring(0,charPos);
        tempContent = tempContent.substring(charPos+1);
        
        charPos = tempContent.indexOf(splitKey);
        wifipsw = tempContent.substring(0,charPos);
        
        Serial.println(payloadContent);
        Serial.println(wifiname);
        Serial.println(wifipsw);
        Serial.println("3");

        const char* ssidname = wifiname.c_str();
        const char* ssidpsw = wifipsw.c_str();
        WiFi.mode(WIFI_STA);
        if (wifiname.length() >= 1) {
          WiFi.begin(ssidname, ssidpsw);
        }
        int counter = 0;
        bool wifiFound = true;
        while (WiFi.status() != WL_CONNECTED)
        {
          delay(500);
          Serial.print(".");
          counter++;
          if (counter >= 50) {
            wifiFound = false;
            break;
          }
        }
        if (wifiFound) {
          Serial.println("");
          Serial.println("connected :)");
          digitalWrite(ledPin, HIGH);
          delay(4000);
          digitalWrite(ledPin, LOW);
          delay(2000);
          digitalWrite(ledPin, HIGH);
          delay(2000);
          digitalWrite(ledPin, LOW);
          delay(2000);
        } else {
          Serial.println("not connected :(");
          digitalWrite(ledPin, HIGH);
          delay(4000);
          digitalWrite(ledPin, LOW);
          delay(2000);
          digitalWrite(ledPin, HIGH);
          delay(2000);
          digitalWrite(ledPin, LOW);
          delay(2000);
          digitalWrite(ledPin, HIGH);
          delay(2000);
          digitalWrite(ledPin, LOW);
          delay(2000);
        }
        delay(5000);
      }
    } 
  }
}

void updateStatus() {
  int senLevel = analogRead(magnetSensorPin);
  Serial.println(senLevel);
  if (senLevel < 700 && senLevel > 500) {
    postStatus("0", serverName);
  } else {
    postStatus("1", serverName);
  }
}

void setup(void) 
{
 Serial.begin(115200);
 Serial.println("System initialized");
 // EEPROM.begin(512);
 nfc.begin();
 setup_wifi();
}

void loop() {
  Serial.println("oke");
  for (int i = 1; i < 10; i = i + 1) {
    readNFC();
  }
  updateStatus();
}