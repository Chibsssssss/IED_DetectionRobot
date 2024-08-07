#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// Wi-Fi credentials
const char* ssid = "Chibs";
const char* password = "12345678";

// Pin definitions
const int leftMotorPin1 = 13;
const int leftMotorPin2 = 12;
const int rightMotorPin1 = 14;
const int rightMotorPin2 = 27;
const int panServoPin = 25;
const int tiltServoPin = 26;
const int speedPin = 2;  // Dummy pin for speed control

const int ammoniaSensorPin = 34;
const int metalSensorPin = 35;
const int temperatureSensorPin = 36;

Servo panServo;
Servo tiltServo;

WebServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/servo", handleServo);
  server.on("/speed", handleSpeed);
  server.on("/sensor_data", handleSensorData);
  server.begin();

  // Initialize motors
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  // Initialize servos
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);

  // Initialize sensors
  pinMode(ammoniaSensorPin, INPUT);
  pinMode(metalSensorPin, INPUT);
  pinMode(temperatureSensorPin, INPUT);
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
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
    Serial.println("F");
  } else if (direction == "backward") {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    Serial.println("B");
  } else if (direction == "left") {
    digitalWrite(leftMotorPin1, LOW);
    digitalWrite(leftMotorPin2, HIGH);
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
    Serial.println("L");
  } else if (direction == "right") {
    digitalWrite(leftMotorPin1, HIGH);
    digitalWrite(leftMotorPin2, LOW);
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
    Serial.println("R");
  }
  server.send(200, "text/plain", "OK");
}

void handleServo() {
  String servo = server.arg("servo");
  int value = server.arg("value").toInt();
  if (servo == "pan") {
    panServo.write(value);
  } else if (servo == "tilt") {
    tiltServo.write(value);
  }
  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  // Speed control code here
  server.send(200, "text/plain", "OK");
}

void handleSensorData() {
  int ammoniaLevel = analogRead(ammoniaSensorPin);
  int metalDetected = digitalRead(metalSensorPin);
  int temperature = analogRead(temperatureSensorPin);

  String sensorData = "{\"ammonia\": " + String(ammoniaLevel) + ", \"metal\": " + String(metalDetected) + ", \"temperature\": " + String(temperature) + "}";
  server.send(200, "application/json", sensorData);
}
