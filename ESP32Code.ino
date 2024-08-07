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
  <img src="http://192.168.5.98" />
  
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
      <span id="metal-status">No Metal</span>
    </div>
    <div class="sensor-bar" id="temperature-bar">
      <span id="temperature-level">0</span>
    </div>
  </div>

  <script>
    function fetchSensorData() {
      fetch('/sensor_data')
        .then(response => response.json())
        .then(data => {
          updateAmmoniaBar(data.ammonia);
          updateMetalBar(data.metal);
          updateTemperatureBar(data.temperature);
        })
        .catch(error => console.error('Error fetching sensor data:', error));
    }

    function updateAmmoniaBar(level) {
      const ammoniaBar = document.getElementById('ammonia-bar');
      const ammoniaLevel = document.getElementById('ammonia-level');
      const height = Math.min(level / 1000 * 200, 200);
      ammoniaBar.style.background = `linear-gradient(to top, red ${height}px, transparent ${height}px)`;
      ammoniaLevel.innerText = level;
      ammoniaLevel.style.bottom = `${height}px`;
    }

    function updateMetalBar(status) {
      const metalBar = document.getElementById('metal-bar');
      const metalStatus = document.getElementById('metal-status');
      metalBar.style.background = status ? 'red' : 'green';
      metalStatus.innerText = status ? 'Metal Detected' : 'No Metal';
    }

    function updateTemperatureBar(level) {
      const temperatureBar = document.getElementById('temperature-bar');
      const temperatureLevel = document.getElementById('temperature-level');
      const height = Math.min(level / 100 * 200, 200);
      temperatureBar.style.background = `linear-gradient(to top, blue ${height}px, transparent ${height}px)`;
      temperatureLevel.innerText = level;
      temperatureLevel.style.bottom = `${height}px`;
    }

    function updatePan(value) {
      document.getElementById('pan-output').innerText = value;
      fetch(`/servo?pan=${value}`)
        .catch(error => console.error('Error updating pan:', error));
    }

    function updateTilt(value) {
      document.getElementById('tilt-output').innerText = value;
      fetch(`/servo?tilt=${value}`)
        .catch(error => console.error('Error updating tilt:', error));
    }

    function updateSpeed(value) {
      document.getElementById('speed-output').innerText = value;
      fetch(`/speed?value=${value}`)
        .catch(error => console.error('Error updating speed:', error));
    }

    setInterval(fetchSensorData, 5000);
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", html);
}

void handleMove() {
  String direction = server.arg("direction");
  // Add motor control code based on direction
  server.send(204, "text/plain", "Moved");
}

void handleServo() {
  if (server.hasArg("pan")) {
    panPos = server.arg("pan").toInt();
    panServo.write(panPos);
  }
  if (server.hasArg("tilt")) {
    tiltPos = server.arg("tilt").toInt();
    tiltServo.write(tiltPos);
  }
  server.send(204, "text/plain", "Servo Moved");
}

void handleSpeed() {
  if (server.hasArg("value")) {
    speed = server.arg("value").toInt();
    // Add motor speed control code
  }
  server.send(204, "text/plain", "Speed Set");
}

void handleSensorData() {
  String json = "{";
  json += "\"ammonia\": " + String(analogRead(34)) + ",";
  json += "\"metal\": " + String(digitalRead(35)) + ",";
  json += "\"temperature\": " + String(analogRead(36));
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/servo", handleServo);
  server.on("/speed", handleSpeed);
  server.on("/sensor_data", handleSensorData);

  server.begin();
  Serial.println("HTTP server started");

  panServo.attach(18); // Adjust the pin numbers as needed
  tiltServo.attach(19); // Adjust the pin numbers as needed
  panServo.write(panPos);
  tiltServo.write(tiltPos);
}

void loop() {
  server.handleClient();
}
