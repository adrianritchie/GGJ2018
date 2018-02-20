#include <WiFi.h>
#include <WebSocketsServer.h>
#include <U8x8lib.h>
#include <Adafruit_NeoPixel.h>
#include <BlynkSimpleEsp32.h>
#include <Preferences.h>

 
#define NEOPIXEL_DATA_PIN 18
#define NEOPIXEL_NUM_LEDS 60


//define sensors
#define DEFUSE_PIN_SWITCH   34
#define DEFUSE_PIN_TRIMPOT  35
#define DEFUSE_PIN_TRACER   27
#define DEFUSE_PIN_KEYPAD   25

struct Triggers {
  uint8_t s1_switch;
  uint16_t k1_keypad;
  uint16_t t1_tracer;
  uint16_t t2_trimpot;
}
oldTriggerStates = { .s1_switch = 0xFF, .k1_keypad = 0xFFFF, .t1_tracer = 0xFFFF, .t2_trimpot = 0xFFFF};


WebSocketsServer webSocket = WebSocketsServer(80);
 
const char auth[] = "091b2133b443485dbb2e71686f361c6d";

WidgetLCD lcd(V0);

Preferences preferences;

U8X8_SSD1306_128X64_NONAME_SW_I2C oled(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

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

  Serial.begin(115200);
  delay(4000);

  oled.begin();
  oled.setFont(u8x8_font_chroma48medium8_r);
 
  preferences.begin("bomb-squad", false);

  if(preferences.getBool("wifi-set", false)) {
    WiFi.begin(preferences.getString("wifi-ssid").c_str(), 
               preferences.getString("wifi-psk").c_str());
    oled.setCursor(0,0);
    oled.print("WiFi connecting");
  }
  else {
    Serial.println("SmartConfig starting");

    oled.setCursor(0,0);
    oled.print("WiFi SmartConfig");

    WiFi.mode(WIFI_AP);
    WiFi.beginSmartConfig();
    while(!WiFi.smartConfigDone()) {
      Serial.print('.');
      delay(250);
    }

  }
  Serial.println("");
  Serial.println("WiFi SmartConfig complete.");


  uint64_t wifi_end =  millis() + 5000;
  while (!WiFi.isConnected()) {
    if (millis() > wifi_end) {
      preferences.putBool("wifi-set", false);
      preferences.end();
      ESP.restart();
    }

    Serial.print('.');
    delay(100);
  }

  preferences.putBool("wifi-set", true);
  preferences.putString("wifi-ssid", WiFi.SSID());
  preferences.putString("wifi-psk", WiFi.psk());
  preferences.end();
  
  Serial.println(WiFi.localIP());
 
  // Setup Blynk
  Blynk.config(auth);
  while (Blynk.connect() == false) {
  }
  lcd.print(0, 0, "IP Address:");
  lcd.print(0, 1, WiFi.localIP().toString());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  oled.clearLine(0);
  oled.setCursor(0, 0);
  oled.print(WiFi.localIP());

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.setBrightness(50);
  timeLimit = 0;

  timer.setInterval(5000L, timerEvent);
}

void updateProgress() { 
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
  delay(2);
  portENABLE_INTERRUPTS();
}

void processMessage(char * message) {
  oled.setCursor(0, 2);
  oled.print(message);

  Serial.println(message);

  if (sizeof(message) > 0) {
    if (message[0] == 't') {
      timeStart = millis();
      timeLimit = (atoi(message+1)) * 1000;
      initialising = true;
      Serial.printf("Time limit: %u\n", timeLimit);
    }
    if (message == "f:1") {
      //todo: something when user defuses bomb
      playing = false;
    }
    if (message == "f:0") {
      //todo: something when user detonates bomb
      playing = false;
    }
  }


  oled.setCursor(0, 7);
  oled.print("Messages: ");
  oled.print(++messagesReceived);
}

void processInputs() {
  if (!initialising && !playing)
    return;

  struct Triggers newTriggerStates;
  newTriggerStates.k1_keypad = analogRead(DEFUSE_PIN_KEYPAD);;
  newTriggerStates.s1_switch = digitalRead(DEFUSE_PIN_SWITCH);
  newTriggerStates.t1_tracer = analogRead(DEFUSE_PIN_TRACER);
  newTriggerStates.t2_trimpot = analogRead(DEFUSE_PIN_TRACER);

  char output[40];
  if (newTriggerStates.k1_keypad != oldTriggerStates.k1_keypad) {
    //todo: replace with characters
  Serial.printf("Keypad new:%d old:%d\n", newTriggerStates.k1_keypad, oldTriggerStates.k1_keypad);
    webSocket.broadcastTXT( newTriggerStates.k1_keypad ? "c:k1:1" : "c:k1:0");
  }

  if (newTriggerStates.s1_switch != oldTriggerStates.s1_switch) {
  Serial.printf("Switch new:%d old:%d\n", newTriggerStates.s1_switch, oldTriggerStates.s1_switch);
    webSocket.broadcastTXT( newTriggerStates.k1_keypad ? "c:s1:1" : "c:s1:0");
  }

  if (newTriggerStates.t1_tracer != oldTriggerStates.t1_tracer) {
  Serial.printf("Tracer new:%d old:%d\n", newTriggerStates.t1_tracer, oldTriggerStates.t1_tracer);
    memset(output, 0, sizeof output);
    sprintf(output, "c:t1:%d", analogRead(DEFUSE_PIN_TRACER));
    webSocket.broadcastTXT(output);
  }

  if (newTriggerStates.t2_trimpot != oldTriggerStates.t2_trimpot) {
  Serial.printf("Trimpot new:%d old:%d\n", newTriggerStates.t2_trimpot, oldTriggerStates.t2_trimpot);
    memset(output, 0, sizeof output);
    sprintf(output, "c:t2:%d", analogRead(DEFUSE_PIN_TRIMPOT));
    webSocket.broadcastTXT(output);
  }

  if (initialising) {
    webSocket.broadcastTXT("x:0");
    initialising = false;
    playing = true;
    timeStart = millis();
    Serial.println("Initialising complete.");
    Serial.println();
  }

  oldTriggerStates = newTriggerStates;
}
 

void loop() {
  webSocket.loop();
  updateProgress();
  processInputs();
  Blynk.run();
  timer.run();
}

