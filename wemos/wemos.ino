#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

const char* ssid[] = { "10H5F", "ryan2", "WEBCOMM_HH41" };
const char* password[] = { "0936767285", "123456789", "77015899" };
const int wifiIndex = 1;

const char* ws_host = "heloword.com";
const int ws_port = 8088;
const char* ws_url = "/endpointOtp";

int motorTrigerPin = D5;
int wifiLedPin = D9;  // D9 ?

WebSocketsClient webSocket;

char wifiNameReadFromSdCard[50];
char wifiPwdReadFromSdCard[50];

void setup() {
  Serial.begin(115200);

  Serial.setDebugOutput(false);

  Serial.setTimeout(300);
  delay(10);

  pinMode(motorTrigerPin, OUTPUT);
  digitalWrite(motorTrigerPin, LOW);

  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(wifiLedPin, LOW);

  readWifiNameAndPwdFromCameraModule();
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifiNameReadFromSdCard, wifiPwdReadFromSdCard);
  // WiFi.begin(ssid[wifiIndex], password[wifiIndex]);

  // Connect to WiFi network
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(wifiLedPin, HIGH);
    delay(500);
    Serial.print("Connecting to ");
    Serial.println(wifiNameReadFromSdCard);
    // Serial.println(ssid[wifiIndex]);
    digitalWrite(wifiLedPin, LOW);
    delay(500);

    counter++;

    if (counter >= 10) {
      ESP.restart();
    }
  }

  Serial.println("WiFi connected\n");

  // server address, port and URL
  webSocket.begin(ws_host, ws_port, ws_url);
  // event handler
  webSocket.onEvent(webSocketEvent);
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }

  webSocket.loop();
}

void readWifiNameAndPwdFromCameraModule() {
  String readString = "";
  while (readString.indexOf("WIFI_OK") < 0) {
    digitalWrite(wifiLedPin, HIGH);
    Serial.println("print: ");
    readString = Serial.readString();
    delay(2000);
    digitalWrite(wifiLedPin, LOW);
  }

  Serial.println("readString: ");
  Serial.println(readString);

  int i = 8;  // start from WIFI_OK#ssid,pwd, till ,
  int j = 0;
  int k = 0;
  while (readString[i] != ',') {
    wifiNameReadFromSdCard[j++] = readString[i++];
  }
  wifiNameReadFromSdCard[j] = '\0';
  i++;
  while (readString[i] != ',') {
    wifiPwdReadFromSdCard[k++] = readString[i++];
  }
  // ending character
  wifiPwdReadFromSdCard[k] = '\0';

  Serial.println("wifiNameReadFromSdCard:");
  Serial.println(wifiNameReadFromSdCard);
  Serial.println("wifiPwdReadFromSdCard:");
  Serial.println(wifiPwdReadFromSdCard);
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      ESP.restart();
      break;
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to url: %s\n", payload);
      // send message to server when Connected
      webSocket.sendTXT("Connected");
      break;
    case WStype_TEXT:
      Serial.printf("[WSc] get text: %s\n", payload);
      // returns 0 if the same
      if (strcmp((char*)payload, "triggered") == 0) {
        Serial.printf("[WSc] triggered!!!!\n");
        triggerMotor();
        break;
      }
      if (strcmp((char*)payload, "reset") == 0) {
        Serial.printf("[WSc] reset!!!!\n");
        reboot();
        break;
      }
      // send message to server
      // webSocket.sendTXT("message here");
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
    case WStype_PING:
      // pong will be send automatically
      Serial.printf("[WSc] get ping\n");
      break;
    case WStype_PONG:
      // answer to a ping we send
      Serial.printf("[WSc] get pong\n");
      break;
  }
}

void triggerMotor() {
  digitalWrite(motorTrigerPin, HIGH);
  Serial.println(" !triggered!");
  delay(5000);
  digitalWrite(motorTrigerPin, LOW);
  delay(10000);
}

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}
