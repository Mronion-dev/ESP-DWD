#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-Server";
const char* password = "12345678";

WebServer server(80);

void handleSend() {
    if (server.hasArg("msg")) {
        String message = server.arg("msg");

        Serial.print("Received: ");
        Serial.println(message);

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

void loop() {
    server.handleClient();
}