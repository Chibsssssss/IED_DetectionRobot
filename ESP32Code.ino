#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <NewPing.h>
#include <DHT.h>

#define DHTTYPE DHT11

// Wi-Fi credentials
const char* ssid = "Goldmindz_Electronics";
const char* password = "Godisgreat24";

//  *** Pin definitions ***

// Motor control pins
int IN1 = 13;
int IN2 = 12; 
int IN3 = 27;
int IN4 = 14;
int enA = 33;
int enB = 15;

int panServoPin = 25; // Servo motor pins
int tiltServoPin = 26;

// Ultrasonic Sensor Pins
const int trigPinBack = 16;
const int echoPinBack = 17;
const int trigPinLeft = 18;
const int echoPinLeft = 19;
const int trigPinRight = 21;
const int echoPinRight = 22;

// Sensor Pins
int ammoniaSensorPin = 34; // Analog input
int metalSensorPin = 35;   // Analog input
int temperatureSensorPin = 23; // DHT11 sensor pin
int rainSensorPin = 32;    // Analog input

// Radio Frequency Detector
int radioFrequencyLevel_3 = 4;

// Initialize NewPing objects for each ultrasonic sensor
NewPing sonarBack(trigPinBack, echoPinBack, 200);
NewPing sonarLeft(trigPinLeft, echoPinLeft, 200);
NewPing sonarRight(trigPinRight, echoPinRight, 200);

DHT dht(temperatureSensorPin, DHTTYPE);

const int ledPin = 2; // LED pin

Servo panServo;
Servo tiltServo;

int speedValue = 255;
bool ledState = false;

WebServer server(80);

// Define PWM channels and frequency
//const int pwmChannelA = 0;
//const int pwmChannelB = 1;
//const int pwmFreq = 5000; // 5 kHz
//const int pwmResolution = 8; // 8-bit resolution (0-255)

void setup() {
  Serial.begin(115200);
  dht.begin();
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
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);

  // Initialize PWM channels
  //ledcSetup(pwmChannelA, pwmFreq, pwmResolution);
  //ledcSetup(pwmChannelB, pwmFreq, pwmResolution);

  // Attach the PWM channels to the GPIO pins
  //ledcAttachPin(enA, pwmChannelA);
  //ledcAttachPin(enB, pwmChannelB);

  // Initialize servos
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);

  // Initialize sensors
  pinMode(ammoniaSensorPin, INPUT);
  pinMode(metalSensorPin, INPUT);
  pinMode(temperatureSensorPin, INPUT);
  pinMode(rainSensorPin, INPUT);

  pinMode(trigPinBack, OUTPUT);
  pinMode(echoPinBack, INPUT);
  pinMode(trigPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(trigPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  pinMode(radioFrequencyLevel_3, INPUT);

  pinMode(ledPin, OUTPUT);

  Serial.println("Setup complete");
}

void loop() {
  handleSensorData();
  server.handleClient();
}

void handleRoot() {
  Serial.println("Root endpoint hit");
  server.send(200, "text/html", "Hello, this is the main ESP32.");
}

void handleMove() {
  String direction = server.arg("direction");
  Serial.print("Move command received: ");
  Serial.println(direction);

  if (direction == "forward") {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(enA, speedValue);
    analogWrite(enB, speedValue);
    //ledcWrite(pwmChannelA, speedValue);
    //ledcWrite(pwmChannelB, speedValue);

    Serial.print("Moving forward with speed: ");
    Serial.println(speedValue);
  } else if (direction == "backward") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(enA, speedValue);
    analogWrite(enB, speedValue);
    //ledcWrite(pwmChannelA, speedValue);
    //ledcWrite(pwmChannelB, speedValue);
    Serial.print("Moving backward with speed: ");
    Serial.println(speedValue);
  } else if (direction == "left") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(enA, speedValue);
    analogWrite(enB, speedValue);
    //ledcWrite(pwmChannelA, speedValue);
    //ledcWrite(pwmChannelB, speedValue);
    Serial.print("Turning left with speed: ");
    Serial.println(speedValue);
  } else if (direction == "right") {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(enA, speedValue);
    analogWrite(enB, speedValue);
    //ledcWrite(pwmChannelA, speedValue);
    //ledcWrite(pwmChannelB, speedValue);
    Serial.print("Turning right with speed: ");
    Serial.println(speedValue);
  } else if (direction == "stop") {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    //ledcWrite(pwmChannelA, 0);
    //ledcWrite(pwmChannelB, 0);
    Serial.println("Stopping");
  }

  server.send(200, "text/plain", "OK");
}

void handleServo() {
  String servo = server.arg("servo");
  int value = server.arg("value").toInt();
  Serial.print("Servo command received: ");
  Serial.print(servo);
  Serial.print(" with value: ");
  Serial.println(value);

  if (servo == "pan") {
    panServo.write(value);
    Serial.print("Pan servo set to: ");
    Serial.println(value);
  } else if (servo == "tilt") {
    tiltServo.write(value);
    Serial.print("Tilt servo set to: ");
    Serial.println(value);
  }

  server.send(200, "text/plain", "OK");
}

void handleSpeed() {
  speedValue = server.arg("value").toInt();
  Serial.print("Speed value set to: ");
  Serial.println(speedValue);
  server.send(200, "text/plain", "OK");
}

void handleToggleLED() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  Serial.print("LED toggled to: ");
  Serial.println(ledState ? "ON" : "OFF");
  server.send(200, "text/plain", "OK");
}

void handleSensorData() {
  int ammoniaValue = analogRead(ammoniaSensorPin);
  int metalValue = digitalRead(metalSensorPin);
  float temperatureValue = dht.readTemperature();
  int rainValue = digitalRead(rainSensorPin);

  float ammoniaPPM = (ammoniaValue / 4095.0) * 100; // Example conversion, adjust as needed
  float temperature = temperatureValue; // Already in degrees Celsius

  int distanceBack = sonarBack.ping_cm();
  int distanceLeft = sonarLeft.ping_cm();
  int distanceRight = sonarRight.ping_cm();

  Serial.print("Ammonia: ");
  Serial.print(ammoniaPPM);
  Serial.print(" ppm, Metal: ");
  Serial.print(metalValue);
  Serial.print(", Temperature: ");
  Serial.print(temperature);
  Serial.print(" C, Rain: ");
  Serial.print(rainValue);
  Serial.print(", Distance Back: ");
  Serial.print(distanceBack);
  Serial.print(" cm, Distance Left: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Distance Right: ");
  Serial.println(distanceRight);

  String json = "{";
  json += "\"ammonia\":" + String(ammoniaPPM) + ",";
  json += "\"metal\":" + String(metalValue) + ",";
  json += "\"temperature\":" + String(temperature) + ",";
  json += "\"rain\":" + String(rainValue) + ",";
  json += "\"distanceBack\":" + String(distanceBack) + ",";
  json += "\"distanceLeft\":" + String(distanceLeft) + ",";
  json += "\"distanceRight\":" + String(distanceRight);
  json += "}";

  server.send(200, "application/json", json);
}
