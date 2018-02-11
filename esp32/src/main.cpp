#include <WiFi.h>
#include <WebSocketsServer.h>
#include <U8x8lib.h>
#include <Adafruit_NeoPixel.h>
#include <BlynkSimpleEsp32.h>

 
#define NEOPIXEL_DATA_PIN 18
#define NEOPIXEL_NUM_LEDS 60


//define sensors
#define DEFUSE_PIN_SWITCH   34
#define DEFUSE_PIN_TRIMPOT  35
#define DEFUSE_PIN_TRACER   27



WebSocketsServer webSocket = WebSocketsServer(80);
 
const char auth[] = "091b2133b443485dbb2e71686f361c6d";

WidgetLCD lcd(V0);

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

uint8_t socketCount = 0;

BlynkTimer timer;
void timerEvent() {
  Serial.printf("Connected sockets: %u\n", socketCount);
}

void processMessage (char * message);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            socketCount--;
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        				// send message to client
				        webSocket.sendTXT(num, "Connected");
                socketCount++;
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            processMessage((char*)payload);

            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            Serial.printf("[%u] get binary length: %u\n", num, length);
            //hexdump(payload, length);

            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    }

}


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
 
  // Setup Blynk
  Blynk.config(auth);
  while (Blynk.connect() == false) {
  }
  lcd.print(0, 0, "IP Address:");
  lcd.print(0, 1, WiFi.localIP().toString());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  u8x8.setCursor(0, 0);
  u8x8.print(WiFi.localIP());

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(50);
  timeLimit = 0;

  timer.setInterval(5000L, timerEvent);
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
      webSocket.broadcastTXT(output);
    }
    else {
      webSocket.broadcastTXT("r:0");
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

void processMessage(char * message) {
  u8x8.setCursor(0, 2);
  u8x8.print(message);


  Serial.println(message);

  if (sizeof(message) > 0 && message[0] == 't') {

    timeStart = millis();
    timeLimit = (atoi(message+1)) * 1000;
    initialising = true;
    Serial.printf("Time limit: %u\n", timeLimit);
  }


  u8x8.setCursor(0, 7);
  u8x8.print("Messages: ");
  u8x8.print(++messagesReceived);
}

void processInputs() {
  if (initialising || playing) {
    webSocket.broadcastTXT(digitalRead(DEFUSE_PIN_SWITCH) == HIGH ? "c:s1:on" : "c:s1:off");

    char output[40];
    sprintf(output, "c:t1:%d", analogRead(DEFUSE_PIN_TRIMPOT));
    webSocket.broadcastTXT(output);

    memset(output, 0, sizeof output);
    sprintf(output, "c:t2:%d", analogRead(DEFUSE_PIN_TRACER));
    webSocket.broadcastTXT(output);
  }
  if (initialising) {
    webSocket.broadcastTXT("x:0");
    initialising = false;
    playing = true;
    timeStart = millis();
  }
}
 

void loop() {
  webSocket.loop();
  updatePixels();
  processInputs();
  Blynk.run();
  timer.run();
}

