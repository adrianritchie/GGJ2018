#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebSocketServer.h>
#include <U8x8lib.h>
#include <Adafruit_NeoPixel.h>
 
#define NEOPIXEL_DATA_PIN 18
#define NEOPIXEL_NUM_LEDS 60


//define sensors
#define DEFUSE_PIN_SWITCH   34
#define DEFUSE_PIN_TRIMPOT  35
#define DEFUSE_PIN_TRACER   27


WiFiServer server(80);
WebSocketServer webSocketServer;
 
const char *ssid = "gringod-wifi";
const char *password = "helloworldthisismyworld";
 

U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPIXEL_NUM_LEDS, NEOPIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
int delayval = 10; // delay for half a second

long timeLimit;
long timeStart;

uint8_t messagesReceived = 0;

bool playing = false;
bool initialising = false;


void setup() {
 
  pinMode(NEOPIXEL_DATA_PIN, OUTPUT);
  pinMode(DEFUSE_PIN_SWITCH, INPUT);
  pinMode(DEFUSE_PIN_TRIMPOT, INPUT);
  pinMode(DEFUSE_PIN_TRACER, INPUT);

  Serial.begin(9600);
 
  WiFi.begin(ssid, password);

  while (!WiFi.isConnected()) {
    Serial.print('.');
    delay(100);
  }
  Serial.println(WiFi.localIP());
 
  if (!MDNS.begin("da_bomb")) {
      Serial.println("Error setting up MDNS responder!");
      while(1){
          delay(1000);
      }
  }


  server.begin();
  delay(100);

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8.setCursor(0, 0);
  u8x8.print(WiFi.localIP());

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(50);
  timeLimit = 0;
}

void updatePixels() { 
	// For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  
  if (playing) {
    uint8_t lit = 0;
    long elapsed = millis() - timeStart;
    if (elapsed < timeLimit) {
      lit = map(elapsed, 0, timeLimit, 60, 0);

      char output[40];
      sprintf(output, "r:%d", timeLimit - elapsed);
      webSocketServer.sendData(output);
    }
    else {
      webSocketServer.sendData("r:0");
      playing = false;
      lit = 0;
    }
  
    for(int i=0;i<NEOPIXEL_NUM_LEDS;i++){

      // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
      uint32_t color = (i >= lit) ?  pixels.Color(0,0,0) : pixels.Color(0,25,0);
      pixels.setPixelColor(i, color);
    } 
    
  }
  else {
    pixels.clear();
  }
  delay(1);
  portDISABLE_INTERRUPTS();
  pixels.show(); // This sends the updated pixel color to the hardware.
  portENABLE_INTERRUPTS();
}

void processMessage(String data) {
  u8x8.setCursor(0, 2);
  u8x8.print(data.c_str());
        
  Serial.println(data);
  webSocketServer.sendData(data);
  
  
  switch (data.charAt(0)) {
    case 't':
      timeStart = millis();
      timeLimit = (atoi(data.substring(1).c_str())) * 1000;
      initialising = true;
      u8x8.setCursor(0, 5);
      u8x8.print(data.c_str());
      break;
  }


  u8x8.setCursor(0, 7);
  u8x8.print("Messages: ");
  u8x8.print(++messagesReceived);
}

void processInputs() {
  if (initialising || playing) {
    webSocketServer.sendData(digitalRead(DEFUSE_PIN_SWITCH) == HIGH ? "c:s1:on" : "c:s1:off");

    char output[40];
    sprintf(output, "c:t1:%d", analogRead(DEFUSE_PIN_TRIMPOT));
    webSocketServer.sendData(output);

    memset(output, 0, sizeof output);
    sprintf(output, "c:t2:%d", analogRead(DEFUSE_PIN_TRACER));
    webSocketServer.sendData(output);
  }
  if (initialising) {
    webSocketServer.sendData("x:0");
    initialising = false;
    playing = true;
    timeStart = millis();
  }
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
        processMessage(data.c_str());
      }
 
      
      delay(10); // Delay needed for receiving the data correctly
      updatePixels();
      processInputs();
    }

 
  }

  Serial.println("The client disconnected");
  u8x8.setCursor(0, 4);
  u8x8.print("Not connected");
  Serial.println(WiFi.localIP());    
 
  delay(100);
}

