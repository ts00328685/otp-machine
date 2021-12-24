/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"

const char* ssid[] = {"10H5F", "ryan2", "WEBCOMM_HH41"};
const char* password[] = {"0936767285", "123456789", "77015899"};
const int wifiIndex = 0;

char wifiNameReadFromSdCard[50];
char wifiPwdReadFromSdCard[50];


String serverName = "www.heloword.com";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
//String serverName = "example.com";   // OR REPLACE WITH YOUR DOMAIN NAME

String serverPath = "/otp/image-upload";     // The default serverPath should be upload.php

const int serverPort = 8088;
boolean flag = false;

WiFiClient client;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define LED_BUILTIN 4
#define ONBOARD_LED 33

const int timerInterval = 20000;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent

void setup() {

  // read wifi settings from sd card first
  Serial.begin(115200);

  // ONBOARD LED as indicator
  pinMode(ONBOARD_LED, OUTPUT); // Set the pin as output
  digitalWrite(ONBOARD_LED, LOW); //Turn on

  setupSdCard();
  readFile(SD_MMC, "/wifi.txt");


  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);



  pinMode (LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  //  Serial.println(ssid[wifiIndex]);
  Serial.println(wifiNameReadFromSdCard);

  int counter = 0;
  //  WiFi.begin(ssid[wifiIndex], password[wifiIndex]);
  WiFi.begin(wifiNameReadFromSdCard, wifiPwdReadFromSdCard);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ONBOARD_LED, HIGH);
    Serial.print("retry: ");
    Serial.println(counter);
    delay(500);
    counter++;
    if (counter > 10) {
      ESP.restart();
    }
    digitalWrite(ONBOARD_LED, LOW);
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());

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
  // 20000000 to 5000000 ? overheat ?
  config.xclk_freq_hz = 5000000;
  config.pixel_format = PIXFORMAT_JPEG;
  // to prevent upload fail
  config.jpeg_quality = 6;
  config.frame_size = FRAMESIZE_XGA;

  // init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 6;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 6;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_saturation(s, -2);     // -2 to 2
  s->set_brightness(s, 2);
  s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
  s->set_vflip(s, 1);          // 0 = disable , 1 = enable

  //  sendPhoto();

  // GPIO
  pinMode(3, INPUT);
}

void loop() {

  delay(1000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.write("WIFI_OK#");
    Serial.write(wifiNameReadFromSdCard);
    Serial.write(',');
    Serial.write(wifiPwdReadFromSdCard);
    Serial.write(',');
    Serial.flush();
  } else {
    ESP.restart();
  }

  // GPIO 2
  if (digitalRead(3) == 1) {
    //      Serial.println("triggered");
    sendPhoto();
  }

  //  unsigned long currentMillis = millis();
  //  if (currentMillis - previousMillis >= timerInterval) {
  //    sendPhoto();
  //    previousMillis = currentMillis;
  //  }


}

String sendPhoto() {
  String getAll;
  String getBody;

  camera_fb_t * fb = NULL;
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  fb = esp_camera_fb_get();
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  if (!fb) {
    Serial.println("Camera capture failed");
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    ESP.restart();
  }

  //  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    //    Serial.println("Connection successful!");
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        client.write(fbBuf, remainder);
      }
    }
    client.print(tail);

    esp_camera_fb_return(fb);

    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + timoutTimer) > millis()) {
      //      Serial.print(".");
      delay(100);
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length() == 0) {
            state = true;
          }
          getAll = "";
        }
        else if (c != '\r') {
          getAll += String(c);
        }
        if (state == true) {
          getBody += String(c);
        }
        startTimer = millis();
      }
      if (getBody.length() > 0) {
        break;
      }
    }
    //    Serial.println();
    client.stop();
    //    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    //    Serial.println(getBody);
  }
  return getBody;
}

void setupSdCard() {
  if (!SD_MMC.begin()) {
    delay(1000);
    Serial.println("Card Mount Failed");
    ESP.restart();
  }
  uint8_t cardType = SD_MMC.cardType();

  if (cardType == CARD_NONE) {
    delay(1000);
    Serial.println("No SD_MMC card attached");
    ESP.restart();
  }

  Serial.print("SD_MMC Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
    delay(1000);
    ESP.restart();
  }

  uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB\n", cardSize);

}

//Read a file in SD card
void readFile(fs::FS &fs, const char * path) {
  digitalWrite(ONBOARD_LED, HIGH); //Turn off
  
  delay(1000);
  digitalWrite(ONBOARD_LED, LOW);
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    delay(2000);
    ESP.restart();
    return;
  }

  char wifiName[50];
  char wifiPwd[50];
  uint8_t i = 0;
  // note how this also prevents the buffer from overflowing (18 max to leave space for '\0'!)
  while (file.available() && file.peek() != ',' && i < 49)
  {
    digitalWrite(ONBOARD_LED, HIGH);
    wifiNameReadFromSdCard[i] = file.read();
    i++;
    delay(50);
    digitalWrite(ONBOARD_LED, LOW);
  }
  wifiNameReadFromSdCard[i] = '\0';
  file.read();
  int j = 0;
  while (file.available() && file.peek() != '\n' && i < 99)
  {
    wifiPwdReadFromSdCard[j++] = file.read();
    i++;
  }
  wifiPwdReadFromSdCard[j] = '\0';

  Serial.println("wifi ssid read from SD card: ");
  Serial.println(wifiNameReadFromSdCard);
  Serial.println("wifi pwd read from SD card: ");
  Serial.print(wifiPwdReadFromSdCard);
  digitalWrite(ONBOARD_LED, HIGH);
}
