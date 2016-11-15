#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

/****************************** Neo Pixel ******************************************/

#define PIN            2//ESPr Developer開発ボードの何番ピンを使うか
#define NUMPIXELS      12//Neo Pixel RingのLEDが何個あるか

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "...your SSID..."//ここにWiFiのSSIDを入れる
#define WLAN_PASS       "...your password..."//ここにWiFiのパスワードを入れる

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "...your AIO username..."//ここにUSERNAMEを入れる。ログインIDではないです。アカウント情報を確認してください。
#define AIO_KEY         "...your AIO key..."//ここにAIO keyを入れる。AdafruitのFeedのVIEW AIO KEYから確認してください。

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'lamp' for subscribing to changes.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe AssistiveCallButtons = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/...your feed name...");
//ここに参照するFeedの名前を入れる

/*************************** Sketch Code ************************************/
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int delayval = 500; // delay for half a second
int current = 0;

void setup() {
  
  // This initializes the NeoPixel library.
  pixels.begin();
  
  // set power switch tail pin as an output
  Serial.begin(115200);
  Serial.println(F("Adafruit IO Example"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  delay(10);
  Serial.print(F("Connecting to "));
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());

  // listen for events on the lamp feed
  mqtt.subscribe(&AssistiveCallButtons);

  // connect to adafruit io
  connect();
}

void loop() {

  Adafruit_MQTT_Subscribe *subscription;

  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      // listen for events on the lamp feed
     // mqtt.subscribe(&AssistiveCallButtons);
      
      // connect to adafruit io
      connect();
  }

  // this is our 'wait for incoming subscription packets' busy subloop
  while (subscription = mqtt.readSubscription(1000)) {

    // we only care about the lamp events
    if (subscription == &AssistiveCallButtons) {

      // convert mqtt ascii payload to int
      char *value = (char *)AssistiveCallButtons.lastread;
      Serial.print(F("Received: "));
      Serial.println(value);
      current = atoi(value);
    }
      //Serial.println(current);
  // write the current state to the power switch tail
  // do something different depending on the
  // range value:
  switch (current) {
    case 0:    // オフ
      for(int i=0;i<NUMPIXELS;i++){
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(0,0,0));
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(delayval); // Delay for a period of time (in milliseconds).
      }
      break;
    case 1:    // 赤
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(150,0,0));
        pixels.show();
        delay(delayval);
      }
      break;
    case 2:    // 緑
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,150,0));
        pixels.show();
        delay(delayval);
      }
      break;
    case 3:    // 青
      for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,150));
        pixels.show();
        delay(delayval);
      }
      break;
    case 4:    // 虹
        pixels.setPixelColor(0, pixels.Color(150,0,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(1, pixels.Color(150,150,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(2, pixels.Color(0,150,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(3, pixels.Color(0,150,150));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(4, pixels.Color(0,0,150));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(5, pixels.Color(150,0,150));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(6, pixels.Color(150,0,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(7, pixels.Color(150,150,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(8, pixels.Color(0,150,0));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(9, pixels.Color(0,150,150));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(10, pixels.Color(0,0,150));
        pixels.show();
        delay(delayval);
        pixels.setPixelColor(11, pixels.Color(150,0,150));
        pixels.show();
        delay(delayval);        
      break;
    case 5:    // 予備（好きな色を設定してみましょう。）
      for(int i=0;i<NUMPIXELS;i++){
        // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
        pixels.setPixelColor(i, pixels.Color(150,150,150));//これだと白
        pixels.show(); // This sends the updated pixel color to the hardware.
        delay(delayval); // Delay for a period of time (in milliseconds).
      }
      break;
  }
  delay(1);        // delay in between reads for stability

  }

}

// connect to adafruit io via MQTT
void connect() {

  Serial.print(F("Connecting to Adafruit IO... "));

  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {

    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);

  }

  Serial.println(F("Adafruit IO Connected!"));

}
