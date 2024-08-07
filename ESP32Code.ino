#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* ssid = "Chibs";
const char* password = "12345678";

WebServer server(80);

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

// HTML page to serve
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>IED Detection Robot</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
    }
    h1 {
      color: #333;
    }
    img {
      width: 60%;
      height: auto;
    }
    form {
      margin: 20px;
    }
    button {
      margin: 5px;
      padding: 10px 20px;
      font-size: 16px;
    }
    input[type="range"] {
      margin: 10px;
      padding: 5px;
      font-size: 14px;
      width: 70%;
    }
    .control-container {
      display: flex;
      justify-content: center;
      align-items: center;
    }
    .control-container div {
      margin: 10px;
    }
    .sensor-bars {
      display: flex;
      justify-content: space-around;
      align-items: center;
      margin: 20px 0;
    }
    .sensor-bar {
      width: 20%;
      height: 200px;
      border: 1px solid #ccc;
      border-radius: 10px;
      position: relative;
    }
    .sensor-bar span {
      position: absolute;
      bottom: 0;
      width: 100%;
      text-align: center;
      font-weight: bold;
      color: #fff;
    }
  </style>
</head>
<body>
  <h1>IED Detection Robot</h1>
  
  <h2>Camera Feed</h2>
  <img class="video" src="http://192.168.5.98/" alt="Camera Feed">
  
  <h2>Car Control</h2>
  <div class="control-container">
    <div>
      <form action="/move" method="get">
        <button name="direction" value="forward">Forward</button><br>
        <button name="direction" value="left">Left</button>
        <button name="direction" value="right">Right</button><br>
        <button name="direction" value="backward">Backward</button>
      </form>
    </div>
  </div>

  <h2>Camera Control</h2>
  <label for="pan">Pan:</label>
  <input type="range" id="pan" name="pan" min="0" max="180" oninput="updatePan(this.value)">
  <output id="pan-output">90</output><br>
  
  <label for="tilt">Tilt:</label>
  <input type="range" id="tilt" name="tilt" min="0" max="180" oninput="updateTilt(this.value)">
  <output id="tilt-output">90</output><br>
  
  <h2>Speed Control</h2>
  <label for="speed">Speed:</label>
  <input type="range" id="speed" name="speed" min="0" max="100" oninput="updateSpeed(this.value)">
  <output id="speed-output">50</output><br>
  
  <h2>Sensor Data</h2>
  <div class="sensor-bars">
    <div class="sensor-bar" id="ammonia-bar">
      <span id="ammonia-level">0</span>
    </div>
    <div class="sensor-bar" id="metal-bar">
      <span id="metal-level">No Metal</span>
    </div>
    <div class="sensor-bar" id="temperature-bar">
      <span id="temperature-level">0°C</span>
    </div>
  </div>

  <script>
    function updatePan(value) {
      document.getElementById('pan-output').innerText = value;
      fetch(`/servo?servo=pan&value=${value}`);
    }

    function updateTilt(value) {
      document.getElementById('tilt-output').innerText = value;
      fetch(`/servo?servo=tilt&value=${value}`);
    }

    function updateSpeed(value) {
      document.getElementById('speed-output').innerText = value;
      fetch(`/speed?value=${value}`);
    }

    function updateSensorData() {
      fetch('/sensor_data').then(response => response.json()).then(data => {
        document.getElementById('ammonia-level').innerText = data.ammonia;
        document.getElementById('metal-level').innerText = data.metal ? 'Metal Detected' : 'No Metal';
        document.getElementById('temperature-level').innerText = `${data.temperature}°C`;

        document.getElementById('ammonia-bar').style.height = `${data.ammonia}%`;
        document.getElementById('metal-bar').style.backgroundColor = data.metal ? 'red' : 'green';
        document.getElementById('temperature-bar').style.height = `${data.temperature}%`;
      });
    }

    setInterval(updateSensorData, 1000);
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", html);
}

void handleMove() {
  String direction = server.arg("direction");
  if (direction == "forward") {
    // Move forward
  } else if (direction == "backward") {
    // Move backward
  } else if (direction == "left") {
    // Move left
  } else if (direction == "right") {
    // Move right
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
  //json += "\"front\": " + String(getDistance(trigFrontPin, echoFrontPin)) + ",";
  //json += "\"back\": " + String(getDistance(trigBackPin, echoBackPin)) + ",";
  //json += "\"left\": " + String(getDistance(trigLeftPin, echoLeftPin)) + ",";
  //json += "\"right\": " + String(getDistance(trigRightPin, echoRightPin));
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

  // Set up server routes
  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/servo", handleServo);
  server.on("/speed", handleSpeed);
  server.on("/sensor_data", handleSensorData);

  server.begin();
  Serial.println("HTTP server started");
  Serial.print(WiFi.localIP());

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
