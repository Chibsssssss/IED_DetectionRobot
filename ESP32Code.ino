#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <ESP32Ping.h>
#include <NewPing.h>

#include <DHT.h>
#define DHTTYPE DHT11

// Wi-Fi credentials
const char* ssid = "CHIBS-MIFI";
const char* password = ".........";


// Pin definitions
int directionPin = 13;
int speedPin = 12;
int leftRightServoPin = 14; // Servo for left and right turning
int panServoPin = 25;
int tiltServoPin = 26;

const int trigPinFront = 15;
const int echoPinFront = 4;
const int trigPinBack = 16;
const int echoPinBack = 17;
const int trigPinLeft = 18;
const int echoPinLeft = 19;
const int trigPinRight = 21;
const int echoPinRight = 22;

int ammoniaSensorPin = 34;
int metalSensorPin = 35;
int temperatureSensorPin = 23;
int rainSensorPin = 32;

// Initialize NewPing objects for each ultrasonic sensor
NewPing sonarFront(trigPinFront, echoPinFront, 200);
NewPing sonarBack(trigPinBack, echoPinBack, 200);
NewPing sonarLeft(trigPinLeft, echoPinLeft, 200);
NewPing sonarRight(trigPinRight, echoPinRight, 200);

DHT dht(temperatureSensorPin, DHTTYPE);

const int ledPin = 2; // LED pin

Servo leftRightServo;
Servo panServo;
Servo tiltServo;

int speedValue = 255;
int leftrightValue = 90;
bool ledState = false;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  //dht.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected at IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/servo", handleServo);
  server.on("/speed", handleSpeed);
  server.on("/toggleLED", handleToggleLED);
  server.on("/sensor_data", handleSensorData);
  server.begin();

  // Initialize motor control pins
  pinMode(directionPin, OUTPUT);
  pinMode(speedPin, OUTPUT);

  // Initialize servos
  leftRightServo.attach(leftRightServoPin);
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);

  // Initialize sensors
  pinMode(ammoniaSensorPin, INPUT);
  pinMode(metalSensorPin, INPUT);
  pinMode(temperatureSensorPin, INPUT);
  pinMode(rainSensorPin, INPUT);

  pinMode(trigPinFront, OUTPUT);
  pinMode(echoPinFront, INPUT);
  pinMode(trigPinBack, OUTPUT);
  pinMode(echoPinBack, INPUT);
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  server.send(200, "text/html", "Hello, this is the main ESP32.");
}

void handleMove() {
  String direction = server.arg("direction");
  if (direction == "forward") {
    digitalWrite(directionPin, HIGH);
    analogWrite(speedPin, speedValue);
    Serial.print("F - ");Serial.println(speedValue);
  } else if (direction == "backward") {
    digitalWrite(directionPin, LOW);
    analogWrite(speedPin, speedValue);
    Serial.print("B - ");Serial.println(speedValue);
  } else if (direction == "left") {
    leftrightValue += 10;
    if(leftrightValue >= 180){
      leftrightValue = 180;
      }
    leftRightServo.write(leftrightValue); // Adjust as needed
    Serial.print("L - ");
    Serial.println(leftrightValue);
  } else if (direction == "right") {
    leftrightValue -= 10;
    if(leftrightValue <= 0){
      leftrightValue = 0;
      }
    leftRightServo.write(leftrightValue); // Adjust as needed
    Serial.print("R - ");
    Serial.println(leftrightValue);
  } else if (direction == "stop") {
    analogWrite(speedPin, 0);
    leftrightValue = 90;
    leftRightServo.write(leftrightValue); // Center position
    Serial.println("STOP");
  }
  server.send(200, "text/plain", "OK");
}

void handleServo() {
  String servo = server.arg("servo");
  int value = server.arg("value").toInt();
  if (servo == "pan") {
    panServo.write(value);
    Serial.println(value);
  } else if (servo == "tilt") {
    tiltServo.write(value);
    Serial.println(value);
  }
  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  speedValue = server.arg("value").toInt();
  Serial.println(speedValue);
  server.send(200, "text/plain", "OK");
}

void handleToggleLED() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  Serial.print("LED ");
  Serial.println(ledState);
  server.send(200, "text/plain", "OK");
}

void handleSensorData() {
  int ammoniaValue = analogRead(ammoniaSensorPin);
  int metalValue = digitalRead(metalSensorPin);
  float temperatureValue = dht.readTemperature();
  int rainValue = digitalRead(rainSensorPin);

  float ammoniaPPM = (ammoniaValue / 4095.0) * 100; // Example conversion, adjust as needed
  float temperature = temperatureValue*5/4095; // Already in degrees Celsius

  /*
  Serial.print("Ammonia Conc: ");
  Serial.print(ammoniaPPM);
  Serial.print("ppm");
  Serial.print(" || ");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" degree Celsis");
  */

  // Simulate ultrasonic sensor readings
  int distanceFront = 30; //sonarFront.ping_cm();
  int distanceBack = 40;  //sonarBack.ping_cm();
  int distanceLeft = 50;  //sonarLeft.ping_cm();
  int distanceRight = 60; //sonarRight.ping_cm();

  String json = "{";
  json += "\"ammonia\":" + String(ammoniaPPM) + ",";
  json += "\"metal\":" + String(metalValue) + ",";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"rain\":" + String(rainValue) + ",";
  json += "\"distanceFront\":" + String(distanceFront) + ",";
  json += "\"distanceBack\":" + String(distanceBack) + ",";
  json += "\"distanceLeft\":" + String(distanceLeft) + ",";
  json += "\"distanceRight\":" + String(distanceRight);
  json += "}";

  server.send(200, "application/json", json);
}
