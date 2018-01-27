#include <WiFi.h>
#include <WebSocketServer.h>
#include <U8x8lib.h>
 
WiFiServer server(80);
WebSocketServer webSocketServer;
 
const char *ssid = "Digital GH Members";
const char *password = "DigiGHmembers1";
 

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);


void setup() {
 
  Serial.begin(9600);
 
  delay(4000);
 
  WiFi.begin(ssid, password);

  delay(4000);
  Serial.println(WiFi.localIP());
 
  server.begin();
  delay(100);


  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

        u8x8.setCursor(0, 0);
        u8x8.print(WiFi.localIP());
}
 
void loop() {
 
  WiFiClient client = server.available();
 
  if (client.connected() && webSocketServer.handshake(client)) {
 
    
    String data;      
 
    while (client.connected()) {
      u8x8.setCursor(0, 4);
      u8x8.print("Connected    ");

      data = webSocketServer.getData();
 
      if (data.length() > 0) {
        u8x8.setCursor(0, 2);
        u8x8.print(data);
        
        Serial.println(data);
        webSocketServer.sendData(data);
      }
 
      delay(10); // Delay needed for receiving the data correctly
   }
 
   Serial.println("The client disconnected");
    u8x8.setCursor(0, 4);
    u8x8.print("Not connected");
  }
 
  delay(100);
}

