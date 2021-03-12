#include <Arduino.h>
#include <ESP8266WiFi.h>

int id = 0;
const char* ssid = "ImperiumDev";
const char* password = "imperium01";

struct Record {
    int id;
    float ax, ay, az, gx, gy, gz, mx, my, mz;
};


void blink() {
    int dd = 20;
    pinMode(LED_BUILTIN, 1);
    delay(dd);
    pinMode(LED_BUILTIN, 0);
    delay(dd);
}

float rndFloat() {
    int x = (rand() % (2<<14)) + 1;
    int y = (rand() % x) + 1;
    return ((float)y)/((float)x);
}

Record rec;
WiFiClient client;
void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("IP ");
    Serial.println(WiFi.localIP());

    int lastDot = WiFi.localIP().toString().lastIndexOf('.');
    id = atoi(WiFi.localIP().toString().substring(lastDot+1).c_str());
    rec.id = id;
}



void serve();

void loop() {
    client.connect("192.168.1.224", 1234);
    blink();
    Serial.println(client.connected());
    while (client.connected()) {
        serve();
    }
}


void serve() {

    rec.ax = rndFloat();
    rec.ay = rndFloat();
    rec.az = rndFloat();    
    
    rec.gx = rndFloat();
    rec.gy = rndFloat();
    rec.gz = rndFloat();

    rec.mx = rndFloat();
    rec.my = rndFloat();
    rec.mz = rndFloat(); 

    client.write((char*)&rec, sizeof(Record));


    client.flush(5);
}
