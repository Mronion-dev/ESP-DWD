#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-Server";
const char* password = "12345678";

WebServer server(80);

void handleSend() {
    if (server.hasArg("str1") && server.hasArg("str2")) {
        String message = server.arg("str1");
        String message2 = server.arg("str2");

        Serial.print("Received SSID: ");
        Serial.println(message);
        Serial.print("Recieved Password:");
        Serial.println(message2);

        server.send(200, "text/plain", "Received: " + message);
    } else {
        server.send(400, "text/plain", "Missing msg parameter");
    }
}

void setup_AP() {

    // Start Wi-Fi access point
    WiFi.softAP(ssid, password);

    Serial.println("AP started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    // HTTP endpoint
    server.on("/send", handleSend);

    server.begin();

    Serial.println("HTTP server started");
}

void setup() {
    Serial.begin(115200);
    setup_AP();
}

void loop() {
    server.handleClient();
}