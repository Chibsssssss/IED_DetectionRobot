#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "Chibs";
const char* password = "12345678";

// Create WebServer object
WebServer server(80);

// Servo objects
Servo panServo;
Servo tiltServo;
int panPos = 90;
int tiltPos = 90;
int speed = 50;

// Ultrasonic sensor pins
const int trigFrontPin = 5;
const int echoFrontPin = 18;
const int trigBackPin = 19;
const int echoBackPin = 21;
const int trigLeftPin = 22;
const int echoLeftPin = 23;
const int trigRightPin = 32;
const int echoRightPin = 33;

// Sensor pins
const int ammoniaSensorPin = 34;
const int metalSensorPin = 35;
const int temperatureSensorPin = 36;

// Servo pins
const int panServoPin = 13;
const int tiltServoPin = 12;

void handleRoot() {
  server.send(200, "text/plain", "Main ESP32 is working");
}

void handleMove() {
  String direction = server.arg("direction");
  if (direction == "forward") {
    // Move forward
    Serial.println("F");
  } else if (direction == "backward") {
    // Move backward
    Serial.println("B");
  } else if (direction == "left") {
    // Move left
    Serial.println("L");
  } else if (direction == "right") {
    // Move right
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
  speed = server.arg("value").toInt();
  server.send(200, "text/plain", "OK");
}

void handleSensorData() {
  int ammonia = analogRead(ammoniaSensorPin);
  int metal = digitalRead(metalSensorPin);
  int temperature = analogRead(temperatureSensorPin);

  String json = "{";
  json += "\"ammonia\": " + String(ammonia) + ",";
  json += "\"metal\": " + String(metal) + ",";
  json += "\"temperature\": " + String(temperature) + ",";
  json += "\"front\": " + String(getDistance(trigFrontPin, echoFrontPin)) + ",";
  json += "\"back\": " + String(getDistance(trigBackPin, echoBackPin)) + ",";
  json += "\"left\": " + String(getDistance(trigLeftPin, echoLeftPin)) + ",";
  json += "\"right\": " + String(getDistance(trigRightPin, echoRightPin));
  json += "}";
  server.send(200, "application/json", json);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);

  // Set up Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Set up server routes
  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/servo", handleServo);
  server.on("/speed", handleSpeed);
  server.on("/sensor_data", handleSensorData);

  server.begin();
  Serial.println("HTTP server started");

  // Attach servos
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);

  // Set up ultrasonic sensors
  pinMode(trigFrontPin, OUTPUT);
  pinMode(echoFrontPin, INPUT);
  pinMode(trigBackPin, OUTPUT);
  pinMode(echoBackPin, INPUT);
  pinMode(trigLeftPin, OUTPUT);
  pinMode(echoLeftPin, INPUT);
  pinMode(trigRightPin, OUTPUT);
  pinMode(echoRightPin, INPUT);

  // Set up other sensors
  pinMode(ammoniaSensorPin, INPUT);
  pinMode(metalSensorPin, INPUT);
  pinMode(temperatureSensorPin, INPUT);
}

void loop() {
  server.handleClient();
}
