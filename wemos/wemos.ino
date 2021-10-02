#include <ESP8266WiFi.h>

const char* ssid[] = {"10H5F", "ryan2", "WEBCOMM_HH41"};
const char* password[] = {"0936767285", "123456789", "77015899"};
const int wifiIndex = 0;
const char* host = "heloword.com";

int motorTrigerPin = D5;
int wifiLedPin = D9;
//WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(300);
  delay(10);

  pinMode(motorTrigerPin, OUTPUT);
  digitalWrite(motorTrigerPin, LOW);

  pinMode(wifiLedPin, OUTPUT);
  digitalWrite(wifiLedPin, LOW);

  String readString = "";
  while (readString.indexOf("WIFI_OK") < 0)  {
    digitalWrite(wifiLedPin, HIGH);
    Serial.println("print: ");
    readString = Serial.readString();
    delay(2000);
    digitalWrite(wifiLedPin, LOW);
  }


  Serial.println("readString: ");
  Serial.println(readString);

  char wifiNameReadFromSdCard[50];
  char wifiPwdReadFromSdCard[50];
  int i = 8; // start from WIFI_OK#ssid,pwd, till ,
  int j = 0;
  int k = 0;
  while(readString[i] != ',') {
    wifiNameReadFromSdCard[j++] = readString[i++];
  }
  wifiNameReadFromSdCard[j] = '\0';
  i++;
  while(readString[i] != ',') {
    wifiPwdReadFromSdCard[k++] = readString[i++];
  }
  wifiPwdReadFromSdCard[k] = '\0';

  Serial.println("wifiNameReadFromSdCard: ");
  Serial.println(wifiNameReadFromSdCard);
  Serial.println("wifiPwdReadFromSdCard: ");
  Serial.println(wifiPwdReadFromSdCard);

  
  
//  WiFi.begin(ssid[wifiIndex], password[wifiIndex]);
  WiFi.begin(wifiNameReadFromSdCard, wifiPwdReadFromSdCard);
  // Connect to WiFi network

  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {

    digitalWrite(wifiLedPin, HIGH);
    delay(500);
    Serial.print("Connecting to ");
//    Serial.println(ssid[wifiIndex]);
    Serial.println(wifiNameReadFromSdCard);
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

}


String readString;
void loop() {


  if (Serial.available())  {
    readString = Serial.readString();
  }

  Serial.print("rx read: ");
  Serial.println(readString);

  delay(1000);

  if (WiFi.status() != WL_CONNECTED) {
    ESP.restart();
  }

  pollServer(readString);
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
  // We now create a URI for the request
  //this url contains the informtation we want to send to the server
  //if esp8266 only requests the website, the url is empty

  if (readString.indexOf("WIFI") > -1 ) {
    readString = "CAMERA_OK";
  } else {
    readString = "CAMERA_ERROR";
  }
  String url = "/otp/poll?cameraStatus=" + readString;
  /* url += "?param1=";
    url += param1;
    url += "?param2=";
    url += param2;
  */
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
