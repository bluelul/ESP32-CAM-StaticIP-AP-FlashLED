#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//
// modified by lttung1197 - bluelul.com
// Arduino 1.8.13 , esp32 1.0.4

// Flash Wiring
// U0R <-> TX
// U0T <-> RX
// IO0 <-> GND
// 5V  <-> 5V (better with external power supply)
// GND <-> GND

// Board: ESP32 Wrover Module
// Upload Speed: 921600
// Flash Frequency: 80 MHz
// Flash Mode: QIO
// Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
// Core Debug Level: None

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

const char* ssid     = "ESP32-CAM";
const char* password = "123456789";

// Set your Static IP address
IPAddress local_IP(10, 10, 1, 1);
// Set your Gateway IP address
IPAddress gateway(10, 10, 1, 1);
IPAddress subnet(255, 255, 255, 0);

////////////////////////// WIFI AUTO CONNECT VARIABLE ////////////////////////
const char* passWifiAutoConnectAP = "c12345678";

////////////////////////// WIFI AUTO CONNECT FUNC ////////////////////////
void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void startCameraServer();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif

  /////// WiFiManager config ///////
  WiFiManager wifiManager; //Local init, got free after setup() done
  
  // wifiManager.resetSettings(); //reset settings - for testing

  wifiManager.setCountry("JP"); // choose "JP" or "US" or "CN"
  wifiManager.setTimeout(120); // AP timeout: shorter ~ quicker reboot after wifi lost
  wifiManager.setClass("invert"); // web server with dark theme
  
  //set callback that gets called when connecting to previous WiFi fails, 
  // and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  /////// WiFiManager connect ///////
  //fetches ssid and pass and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  char ssidWifiAutoConnectAP[9] = {0};
  sprintf(ssidWifiAutoConnectAP,"CLOG%04d",0);
  if (wifiManager.autoConnect(ssidWifiAutoConnectAP, passWifiAutoConnectAP)) {
    Serial.println("");
    Serial.print("WiFi connected, Connect to your wifi network and go to: http://");  
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("failed to connect wifi and hit timeout");
    Serial.println("Setting AP (Access Point)…");
    // Remove the password parameter, if you want the AP (Access Point) to be open
    WiFi.softAP(ssid, password);
    Serial.println("Wait 100 ms for AP_START...");
    delay(100);
    
    // Configures static IP address
    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }
  
    IPAddress IP = WiFi.softAPIP();
    Serial.print("Camera Stream Ready! Connect to the ESP32 AP and go to: http://");
    Serial.println(IP);   
  }

  startCameraServer();  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10000);
}
