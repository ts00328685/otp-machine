#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
// #include <SocketIOclient.h>

const char* ssid[] = {"10H5F", "ryan2", "WEBCOMM_HH41"};
const char* password[] = {"0936767285", "123456789", "77015899"};
const int wifiIndex = 0;
const char* host = "heloword.com";

const char* ws_host = "192.168.0.22";
const int ws_port   = 8088;
const char* ws_url   = "/endpointOtp";

int motorTrigerPin = D5;
int wifiLedPin = D8;
//WiFiServer server(80);

WebSocketsClient webSocket;
// SocketIOclient socketIO;

#define USE_SERIAL Serial
// Stomp::StompClient stomper(webSocket, ws_host, ws_port, ws_baseurl, true);

void setup() {
  Serial.begin(115200);

  Serial.setDebugOutput(true);

  Serial.setTimeout(300);
  delay(10);

  pinMode(motorTrigerPin, OUTPUT);
  digitalWrite(motorTrigerPin, LOW);

  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(wifiLedPin, LOW);



// reading wifi name & pwd from tx of camera module
  // String readString = "";
  // while (readString.indexOf("WIFI_OK") < 0)  {
  //   digitalWrite(wifiLedPin, HIGH);
  //   Serial.println("print: ");
  //   readString = Serial.readString();
  //   delay(2000);
  //   digitalWrite(wifiLedPin, LOW);
  // }


  // Serial.println("readString: ");
  // Serial.println(readString);

  // char wifiNameReadFromSdCard[50];
  // char wifiPwdReadFromSdCard[50];
  // int i = 8; // start from WIFI_OK#ssid,pwd, till ,
  // int j = 0;
  // int k = 0;
  // while(readString[i] != ',') {
  //   wifiNameReadFromSdCard[j++] = readString[i++];
  // }
  // wifiNameReadFromSdCard[j] = '\0';
  // i++;
  // while(readString[i] != ',') {
  //   wifiPwdReadFromSdCard[k++] = readString[i++];
  // }
  // // ending character
  // wifiPwdReadFromSdCard[k] = '\0';

  // Serial.println("wifiNameReadFromSdCard: ");
  // Serial.println(wifiNameReadFromSdCard);
  // Serial.println("wifiPwdReadFromSdCard: ");
  // Serial.println(wifiPwdReadFromSdCard);

  // WiFi.begin(wifiNameReadFromSdCard, wifiPwdReadFromSdCard);
// reading wifi name & pwd from tx of camera module ===== end
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid[wifiIndex], password[wifiIndex]);

  // Connect to WiFi network

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(wifiLedPin, HIGH);
    delay(500);
    Serial.print("Connecting to ");
    Serial.println(ssid[wifiIndex]);
    // Serial.println(wifiNameReadFromSdCard);
    digitalWrite(wifiLedPin, LOW);
    delay(500);

    counter++;

    if (counter >= 10) {
      ESP.restart();
    }
  }
  Serial.println("");
  Serial.println("WiFi connected");


  // Start the server
  //  server.begin();
  //  Serial.println("Server started");

  // Print the IP address
  //  Serial.print("Use this URL : ");
  //  Serial.print("http://");
  //  Serial.print(WiFi.localIP());
  //  Serial.println("/");


// server address, port and URL
	webSocket.begin(ws_host, ws_port, ws_url);
	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	// webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	// socketIO.setReconnectInterval(3000);
}


String readString;
void loop() {


  // if (Serial.available())  {
  //   readString = Serial.readString();
  // }

  // Serial.print("rx read: ");
  // Serial.println(readString);

   delay(5000);

  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }
  // socketIO.loop();

  webSocket.loop();

  // webSocket.sendTXT("123123");

  // socketIO.sendTXT("123");

  // pollServer(readString);
}

void pollServer(String readString) {
  Serial.print("connecting to ");
  Serial.println(host); // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 8088;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  if (readString.indexOf("WIFI") > -1 ) {
    readString = "CAMERA_OK";
  } else {
    readString = "CAMERA_ERROR";
  }
  String url = "/otp/poll?cameraStatus=" + readString;

  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();

  while (client.available() == 0) {
    if (millis() - timeout > 7000) {
      Serial.println(">>> Client Timeout !");
      client.stop(); return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available())
  {
    String line = client.readStringUntil('\r');
    Serial.print(line);

    if (line.indexOf("triggered") > -1) {
      digitalWrite(motorTrigerPin, HIGH);
      Serial.println(" !triggered!");
      delay(5000);
      digitalWrite(motorTrigerPin, LOW);
      delay(10000);
      break;
    }

    if (line.indexOf("reset") > -1) {
      reboot();
      break;
    }

  }
  Serial.println();
  Serial.println("closing connection");
}

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: 
			Serial.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("Connected");
			break;

		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);

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

