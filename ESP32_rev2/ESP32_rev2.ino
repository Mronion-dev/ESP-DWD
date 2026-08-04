//Second revision of the drone code
// second rev includes new captive portal to bypass hardcoded ssid an password
// Method:
// Esp32 --- New wifi ap with ssid "DWD-{last 4 chars of mac address}" and hardcoded password --- user connects and configures router ssid and password --- remove ap and store ssid and password permanently
// --- connect to ssid and password --- look for drone servers with udp discovery --- connect --- get inputs and outputs
// New features include: 
// -captive portal for changable wifi ssids and passwords
// -resetable ssid and password for reconfig
// -better display
// -more accurate stablization
// for devs: good luck reading this, youll need it

#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SPI.h>
#include <ICM_20948.h>
#include <MadgwickAHRS.h>
#include <MD_MAX72xx.h>
#include <ESP32Servo.h>
#include <WiFiUdp.h>

Servo servoLeft;
Servo servoRight;
int ServoLeftVal;
int ServoRightVal;

Servo escLeft;
Servo escRight;

ICM_20948_I2C imu;
Madgwick filter;

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define CS_PIN 5
MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

uint32_t lastTime = 0;
float gxr;
float gyr;
float gzr;

using namespace websockets;
WebsocketsClient client;

char* ssid = ""; 
char* password = "";

WiFiUDP udp;

const uint16_t DISCOVERY_PORT = 42100;

const uint16_t Port = 54732; // Port that the godot server uses
bool connected = false;

//I2C com with IMU board
void setup_IMU()
{
    servoLeft.attach(27);
    servoRight.attach(14);
    servoLeft.write(90);
    servoRight.write(90);

    escLeft.attach(25);
    escRight.attach(26);

    escLeft.writeMicroseconds(1000);
    escRight.writeMicroseconds(1000);
    Draw_repetition();
    delay(5000);

    Wire.begin(21, 22);

    bool initialized = false;
    while (!initialized)
    {
        imu.begin(Wire, false);

        Serial.print("Initialization status: ");
        Serial.println(imu.statusString());

        if (imu.status == ICM_20948_Stat_Ok)
        {
            initialized = true;
        }
    }

    Serial.println("IMU connected!");
}

IPAddress discoverServer(uint32_t timeout = 3000)
{
    udp.begin(DISCOVERY_PORT);

    udp.beginPacket("255.255.255.255", DISCOVERY_PORT);
    udp.print("FIND_MY_SERVER");
    udp.endPacket();

    uint32_t start = millis();

    while (millis() - start < timeout)
    {
        int len = udp.parsePacket();

        if (len)
        {
            char buffer[32];
            len = udp.read(buffer, sizeof(buffer) - 1);
            buffer[len] = '\0';

            if (strcmp(buffer, "IM_HERE") == 0)
            {
                return udp.remoteIP();
            }
        }

        delay(10);
    }

    return IPAddress(0,0,0,0);
}


bool connect_to_server()
{
    Serial.print("Connecting to WebSocket server: ");
    Serial.println(server);

    if (client.connect(discoverServer().toString(), Port, "/"))
    {
        Serial.println("WebSocket connected!");
        return true;
    }

    Serial.println("WebSocket connection failed!");
    return false;
}

void Draw_Check(){
  matrix.setPoint(1,5,true);
  matrix.setPoint(2,6,true);
  matrix.setPoint(3,7,true);
  matrix.setPoint(4,6,true);
  matrix.setPoint(5,5,true);
  matrix.setPoint(6,4,true);
  matrix.setPoint(7,3,true);
}

void Draw_repetition(){
  for(int b = 0; b == 7; b++){
    for(int c = 3; c == 5; c++){
      matrix.setPoint(c, b, true);
      delay(40);
      matrix.clear();
    }
  }
}

void flashX(uint8_t flashes = 6, uint16_t onTime = 200, uint16_t offTime = 200)
{
    const uint8_t X[8] =
    {
        B10000001,
        B01000010,
        B00100100,
        B00011000,
        B00011000,
        B00100100,
        B01000010,
        B10000001
    };

    for (uint8_t i = 0; i < flashes; i++)
    {
        // Draw X
        for (uint8_t row = 0; row < 8; row++)
        {
            matrix.setRow(row, X[row]);
        }

        delay(onTime);

        // Clear display
        matrix.clear();

        delay(offTime);
    }
}

bool parseControllerInput(const String& input, float &leftX, float &leftY, float &rightX, int &rightY)
{
    if (input.length() < 12)
        return false;

    leftX  = input.substring(0, 3).toFloat();
    leftY  = input.substring(3, 6).toFloat();
    rightX = input.substring(6, 9).toFloat();
    rightY = input.substring(9, 13).toInt();

    return true;
}

float throttle = 1000.0f; // ESC pulse width in µs

float delta;

void updateThrottle(int rightY)
{
    escLeft.writeMicroseconds(rightY);
    escRight.writeMicroseconds(rightY);
}

float biasX = 0;
float biasY = 0;
float biasZ = 0;

void calibrateGyro() //creates a bias to subtract from the imu values, reduce shaking and drifings
{
    const int samples = 1000;

    for (int i = 0; i < samples; i++)
    {
        while (!imu.dataReady());

        imu.getAGMT();

        biasX += imu.gyrX();
        biasY += imu.gyrY();
        biasZ += imu.gyrZ();

        delay(2);
    }

    biasX /= samples;
    biasY /= samples;
    biasZ /= samples;
}

//This runs when the ESP32 boots
void setup() {
  SPI.begin(18, 19, 23, 5);
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, 5);
  matrix.clear();
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  uint32_t lastAnimation = 0;
  int i;
  int wait_count = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    
    if (millis() - lastAnimation >= 50
    )
    {
      lastAnimation = millis();
      matrix.setPoint(i, 0,true);
      i += 1;
      wait_count += 1;

      // advance animation by ONE step
    }
    if(i == 7){
      matrix.clear();
      i = 0;
    }
    if(wait_count > 100) {
      Serial.println("Wifi failed, restarting...");
      ESP.restart();
    }
  }
  matrix.clear();
  Draw_Check();
  delay(100);
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway (Router IP): ");
  Serial.println(WiFi.gatewayIP()); // Get router's IP address

  setup_IMU();
  filter.begin(100.0f);
  Serial.println("IMU connected succesfully");
  calibrateGyro();
  Serial.println("IMU calibrated!");

  //Sends an http packet to a flask server and asks for the requested IP to connect to
  Serial.println("Sending WHO_AM_I request to flask server on http://192.168.10.35:5000 ");
  
  //Connect to the Websocket safely and handle timeouts
  if(connect_to_server() == false){
    connected = false;
    Serial.println("connection failed, stopping and reseting esp...");
    ESP.restart();
  }
  else{
    connected = true;
  }

  client.onMessage([](WebsocketsMessage msg) {
    String shit = String(msg.data());
    if(shit == "closing") {
      Serial.println("Server is off, pleas restart the esp32 to reconnect");
      while(true) {
        flashX();
        
      }
    }
    if(shit == "rest") {
      Serial.println("requested restart");
      flashX(4,50,50);
      ESP.restart();
    }
    float leftX;
    float leftY;
    float rightX;
    int rightY;
    matrix.setPoint(7,0,true);
    if(parseControllerInput(shit, leftY, rightX, leftX , rightY) == true){
      float servoOffset = (leftY - 1.0f) * 20.0f;
      
      int leftAngle  = (int)(90.0f + servoOffset);
      int rightAngle = (int)(90.0f - servoOffset);

      leftAngle  = constrain(leftAngle, 70, 110);
      rightAngle = constrain(rightAngle, 70, 110);

      updateThrottle(rightY);
      Serial.println(rightY);
      servoLeft.write(leftAngle);
      servoRight.write(rightAngle);
      matrix.setPoint(0,7,true);
    }
      
  });
}


int t = 0;
int angle = 0;

void loop() {
  if(connected == false) {
    return;
  }
    uint32_t now = micros();
    delta = (now - lastTime) / 1000000.0f;
    lastTime = now;
    client.poll();
    if (imu.dataReady())
    {
      imu.getAGMT();

      float gx = imu.gyrX() - biasX;
      float gy = imu.gyrY() - biasY;
      float gz = imu.gyrZ() - biasZ;

      float ax = imu.accX() / 1000.0f;
      float ay = imu.accY() / 1000.0f;
      float az = imu.accZ() / 1000.0f;

      filter.updateIMU(
        gx, gy, gz,
        ax, ay, az
      );
      float qw = filter.q0;
      float qx = filter.q1;
      float qy = filter.q2;
      float qz = filter.q3;
      gxr += gx * delta;
      gyr += gy * delta;
      gzr += gz * delta;

      client.send(String(qw, 3) + " " + String(qx, 3) + " " + String(qy, 3) + " " + String(qz, 3));

      matrix.clear();
      if(t % 2 == 0){
        matrix.clear();
        matrix.setPoint(t/2,t/2,true);
      }
      t += 1;
      if(t == 15){
        t = 0;
      }
    }
}