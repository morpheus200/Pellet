
//#include <util.h>
#include <ctype.h>
#include "ESP8266.h"
#include <SoftwareSerial.h>





//#define pinnumber   "1111"
#define rxPin       6  //TX on RS232
#define txPin       5  //RX on RS232

#define rxWlanPin   3 //TX on ESP8266
#define txWlanPin   2  //RX on ESP8266

#define SSID        "Hamm_Unten"
#define PASSWORD    "mod4@PeKouj5*#7@Tg!v"
#define UDP_NAME    "192.168.177.36"
#define UDP_PORT    (5416)

int valueTX;
int valueRX;
bool dataWrite = 0;
bool textMode = 0;
String txString ="";
String rxString="";
byte SWval;
bool ignit = false;
int heat = 30;
boolean modemReady = false;
boolean wifiState = false;
boolean UDPState = false;

SoftwareSerial rs232(rxPin, txPin); //Rs232
SoftwareSerial wlanSerial(rxWlanPin, txWlanPin); //ESP8266
ESP8266 wifi(wlanSerial);

void setup() 
{
  rs232.begin(38400);//38400 for PC 
  Serial.begin(9600); //ARduino Monitor, local
  Serial.println("RS232 Serial Ready"); 
  wlanSerial.begin(115200); //Star Serial WLan
  //wlanTest(); //Debug
  wlanReset();
  setWifiIP();
 // joinWifi();

}

//////////// Read //////////////////////////////////////

String serRead()   //Daten lesen
{
  char data;
  String rx ="";
  while (rs232.available() > 0)
  {
    data = rs232.read(); 
    Serial.println(byte (data));
    if (byte(data) != 13)
      if (byte(data) != 10)
        rx.concat(data);
  }
   return rx;
}

//////////////// Write ////////////////////////

void serWrite(String outMessage)   //Daten schreiben
{
  char* cString = (char*) malloc(sizeof(char)*(outMessage.length() + 1));
  outMessage.toCharArray(cString, outMessage.length() + 1);
  rs232.write(cString);
}

///////////////////// RX interpreter ////////////////////////

void interpretRX(String rx)
{
  if (rxString.equals("AT")) //Ping
  {
    Serial.println("AT-Command");
    txString = "OK\r\n";
    dataWrite =1;
  }

  if (rx.equals("ATE0")) //E0 Echo dissable / E1 Echo Enable
  {
    Serial.println("ATE0-Command");
    txString = "OK\r\n"; 
    dataWrite = 1;
  }
  
  if (rx.equals("AT+CGMM"))  //Modell Abfrage
  {
    txString = "TC35i\r\n"; 
    dataWrite = 1;  
  }

  if (rx.equals("AT+CPIN"))  // PIN Abfrage
  {
    txString = "+CPIN: READY\r\n";  //Kein PIN gesetzt
    dataWrite = 1;
  }

  if (rx.equals("AT+CMGF=1")) //UMschalten auf Textmode
  {
    txString = "OK\r\n";
    dataWrite = 1;
    textMode = 1;
  }

  if (rx.equals("AT+CNMI=2,1,0,0,1") || rx.equals("AT+CNMI=0,0,0,0,1"))
  {
    txString = "OK\r\n";
    dataWrite = 1;
  }

  if (rx.equals("AT+CMGS=?"))
  {
    txString = "OK\r\n";
    dataWrite = 1;
  }

  if (rx.indexOf("AT+CMGS=\"+49612341234\"")>=0)
  {
    receivedStoven(rx);
  }

  if (rx.equals("AT+CMGF=0")) //UMschalten auf Textmode
  {
    txString = "OK\r\n";
    dataWrite = 1;
    textMode = 0;
  }

  if (rx.equals("AT+CMGR=1")) //Fragen nach Nachricht
  {
    txString = "OK\r\n";
    dataWrite = 1;
    modemReady = true;
  }

  if (rx.equals("AT+CMGD=1") || rx.equals("AT+CMGD=2") || rx.equals("AT+CMGD=3")) //Lösche Nachricht
  {
    txString = "OK\r\n";
    dataWrite = 1;
  }  
}

void receivedStoven(String SMS)
{
  
}

/////////////////// WLAN ///////////////////////////////

void wlanTest()
{
  wlanSerial.listen();
  wlanSerial.println("AT");  //Gerät vorhanden
  Serial.println("Wlan AT");
  
  if(wlanSerial.find("OK"))
  {  
    Serial.println("OK");
  }
  Serial.print("Wlan FW Version: ");
  wlanSerial.println("AT+GMR");
  delay(500);
  while (wlanSerial.available()>0)
  {
    Serial.print((char) wlanSerial.read());
  }    
}

void wlanReset()
 {
  wlanSerial.listen();
  wlanSerial.println("AT+RST"); //Reset 
  if (wlanSerial.find("ready"))
  {  
    Serial.println("Reset ready");
  };
  wlanSerial.println("AT+CWMODE=1"); //2 = Accespoint (host); 1 = Station (client)
  delay(500);
  
  //////Debug
  /*while(wlanSerial.available()>0)
  {
    Serial.print((char)wlanSerial.read());
  }*/
  //////Debug
  
  if (wlanSerial.find("OK"))
  {
    Serial.println("Client Mode");
  }
  
  wlanSerial.println("AT+CIPMUX=0"); //0 = Single; 1 = Mehrere (Max 4)
  if (wlanSerial.find("OK"))
  {
    Serial.println("Einzelverbindung");
  }
  delay(500);  
 }


void setWifiIP()
{
  wlanSerial.listen();
  wlanSerial.println("AT+CWDHCP=1,0"); //mode (1=station; 0=AP), en (0=enable; 1=disable)
  wlanSerial.flush();
  if (wlanSerial.find("OK"))
  {
    Serial.println("DHCP Disabled");
  }
  delay(500);

  //wlanSerial.println("AT+CIPSTA=\"192.168.177.66\",\"255.255.255.0\",\"192.168.177.1\"");
  wlanSerial.println("AT+CIPSTA=\"192.168.177.66\",\"192.168.177.1\"");
  //wlanSerial.println("AT+CIPSTA=\"192.168.177.66\"");
  wlanSerial.flush();
  if (wlanSerial.find("OK"))
  {
    Serial.println("IP set");
  }

   
  /*if(wlanSerial.find("OK"))
  {
  // wlanSerial.println("AT+CIPSTA?");
   */
   /*if (wlanSerial.find("192.168.177.66"))
   {
    Serial.println("IP set"); 
   }*/
  //}

  
}

void joinWifi()
{
  wlanSerial.print("AT+CWJAP=\"");
  wlanSerial.print(SSID);
  wlanSerial.print("\",\"");
  wlanSerial.print(PASSWORD);
  wlanSerial.println("\"");
  delay(3500);
  if(wlanSerial.find("OK"))
    Serial.println("Wlan Verbunden");
    else
    Serial.println("Wifi Error");
  /*
    if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }
    
    if (wifi.joinAP(SSID, PASSWORD)) 
    {
        Serial.print("Join AP success\r\n");
        Serial.print("IP: ");       
        Serial.println(wifi.getLocalIP().c_str());
        wifiState =true;
    } else 
    {
        Serial.print("Join AP failure\r\n");
        wifiState = false;
    }

    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");*/
}

/////////////// UDP Send /////////////////////////////////
/*
void wifiTX(String data)
 {
    uint8_t buffer[128] = {0};
    
    char* cString = (char*) malloc(sizeof(char)*(data.length() + 1));
    data.toCharArray(cString, data.length() + 1);
    
    //char *hello = data;
    wifi.send((const uint8_t*)cString, strlen(cString));
 }
*/
//////////////////// UDP Receive //////////////////////////
/*
 String wifiRX()
 {
    uint8_t buffer[128] = {0};
    
    String wifiRXMessage = "";  
    uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
    if (len > 0) {
        Serial.print("Received:[");
        for(uint32_t i = 0; i < len; i++) {
            Serial.print((char)buffer[i]);
            wifiRXMessage.concat((char)buffer[i]);
        }
        Serial.print("]\r\n");
    }

    Serial.println(wifiRXMessage);
    return wifiRXMessage;
 }
*/
//////////////// UDP Port Register ////////////////////
/*
bool registerUDP()
{
  if (UDPState = false)
  {
    if (wifi.registerUDP(UDP_NAME, UDP_PORT)) {
          Serial.print("register udp ok\r\n");
          UDPState = true;
      } else {
          Serial.print("register udp err\r\n");
      }
  }
}*/

 /////////////// UDP Port Unregister //////////////////
/*
bool unregisterUDP()
{
  if (UDPState == true)
   {
      if (wifi.unregisterUDP()) {
          Serial.print("unregister udp ok\r\n");
          UDPState = false;
      } else {
          Serial.print("unregister udp err\r\n");
      }
   }
}*/

//////////////////// Main Loop /////////////////////////////////

void loop()
{
  
  rs232.listen();  
  if (rs232.available()>0)  //Buffer gefüllt
  {
    //Serial.println("Soft Serial available");
    rxString = serRead(); //Lesen aus Puffer
    Serial.print("Received: ");
    Serial.println(rxString);    //Wert im Serial Monitor
    interpretRX(rxString);
    rxString ="";
  }

  if (dataWrite == 1 && rs232.available() == 0)  //Wenn daten zu senden und Buffer leer
  {
    Serial.println("Write Data");
    serWrite(txString);
    Serial.println(txString);
    dataWrite = 0;
  }
  
  //if (modemReady)
  //{
    wlanSerial.listen();  
    if (!wifiState)
    {
      /*Serial.println(wifi.kick());
      Serial.println("Start WiFi");
      joinWifi();*/
    }
    
  //}
}

