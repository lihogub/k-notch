#include <Arduino.h>
#include <LittleFS.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define BUF_SIZE 1
#define RECORD_LEN 64

struct Record {
    int timestamp;
    float ax, ay, az, gx, gy, gz, mx, my, mz;
};

const char* ssid = "ImperiumDev";
const char* password = "imperium01";

File file;
ESP8266WebServer server(80);
Record buffer[BUF_SIZE];

inline void writeBuffer() {
  for (int i = 0; i < BUF_SIZE; i++) {
    file.print(buffer[i].timestamp);
    // file.print(',');
    // file.print(buffer[i].ax);
    // file.print(',');
    // file.print(buffer[i].ay);
    // file.print(',');
    // file.print(buffer[i].az);
    // file.print(',');
    // file.print(buffer[i].gx);
    // file.print(',');
    // file.print(buffer[i].gy);
    // file.print(',');
    // file.print(buffer[i].gz);
    // file.print(',');
    // file.print(buffer[i].mx);
    // file.print(',');
    // file.print(buffer[i].my);
    // file.print(',');
    // file.print(buffer[i].mz);
    file.print('\r');
  }
    file.print('\n');
    //file.flush();
}

void showSize() {
  server.send(200, "text/plain", String(file.size()));
}

void showData() {
    int page = (server.hasArg("page")) ? atoi(server.arg("page").c_str()) : 0;
    int size = (server.hasArg("size")) ? atoi(server.arg("size").c_str()) : BUF_SIZE;
    file.seek(0);
    for (int i = 0; i < page*size; i++)
        if (file.available()) 
            file.readStringUntil('\n');
    String SS;
    for (int i = 0; i < size; i++)
        if (file.available()) 
            SS.concat(file.readStringUntil('\n'));
    server.send(200, "text/plain", SS);
    file.seek(0, SeekEnd);
}

void deleteData() {
    file.close();
    LittleFS.remove("/storage.txt");
    file = LittleFS.open("/storage.txt", "a+");
    server.send(200, "text/plain", "DELETED");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP");
  Serial.println(WiFi.localIP());
  server.on("/show", showData);
  server.on("/size", showSize);
  server.on("/delete", deleteData);
  server.on("/write", writeBuffer);
  server.begin();
  LittleFS.begin();
  file = LittleFS.open("/storage.txt", "a+");
}

float rndFloat() {
    int x = (rand() % (2<<14)) + 1;
    int y = (rand() % x) + 1;
    return ((float)y)/((float)x);
}
int last = 0;
int i = 0;
void loop() {
    server.handleClient();
    int cur = millis();
    if (cur - last < 10) return; 
    if (i%BUF_SIZE == 0) {
        writeBuffer();
        i = 0;
    }
    buffer[i].timestamp = cur - last;
    last = cur;
    buffer[i].ax = rndFloat();
    buffer[i].ay = rndFloat();
    buffer[i].az = rndFloat();
    buffer[i].gx = rndFloat();
    buffer[i].gy = rndFloat();
    buffer[i].gz = rndFloat();
    buffer[i].mx = rndFloat();
    buffer[i].my = rndFloat();
    buffer[i].mz = rndFloat();
    i++;
}