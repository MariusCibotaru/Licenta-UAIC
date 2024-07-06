#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include "DistanceSensor.h"
#include "MotorControl.h"
#include "Sensor.h"
#include "Autopilot.h"
#include "Pilot.h"

const char* ssid = "ESP32_Access_Point";
const char* password = "12345678";

WebServer server(80);

extern float distance; 
extern float roll, pitch, yaw;
extern bool takeoffCommand;
int motorSpeeds[4] = {0, 0, 0, 0}; 

const char* htmlContent = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 Horizon Display</title>
  <meta charset="UTF-8">
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      display: flex;
      flex-direction: column;
      align-items: center;
      height: 100vh;
      position: relative;
      padding: 10px;
      box-sizing: border-box;
    }
    #headerRow {
      display: flex;
      justify-content: space-between;
      align-items: center;
      width: 100%;
      flex-shrink: 0; 
    }
    #headerRow .section {
      flex: 1;
      display: flex;
      justify-content: center;
      align-items: center;
    }
    #header {
      font-size: 24px;
    }
    #heightContainer {
      background-color: rgba(255, 255, 255, 0.8);
      padding: 10px;
      border-radius: 5px;
      box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);
      display: flex;
      flex-direction: column;
      align-items: center;
    }
    #sliderContainer {
      background-color: rgba(255, 255, 255, 0.8);
      padding: 10px;
      border-radius: 5px;
      box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);
      display: flex;
      flex-direction: column;
      align-items: center;
    }
    #mainContainer {
      display: flex;
      flex-direction: column;
      justify-content: center;
      align-items: center;
      flex: 1;
      width: 100%;
      overflow-y: auto;
      padding-bottom: 1%;
    }
    #orientationContainer {
      display: flex;
      flex-direction: column;
      align-items: center;
      width: 100%;
      height: 100%;
    }
    #orientationHeader {
      font-size: 32px;
      margin-bottom: 10px;
    }
    #paramData {
      display: flex;
      flex-direction: row;
      justify-content: center;
      width: 100%;
      gap: 20px;
    }
    #horizonRow {
      display: flex;
      justify-content: space-around;
      align-items: center;
      flex: 1;
      width: 100%;
    }
    .horizon-column {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      width: 33.33%;
    }
    .control-buttons, .arrow-buttons {
      display: flex;
      flex-direction: column;
      align-items: center;
    }
    .control-buttons-row, .arrow-buttons-row {
      display: flex;
      justify-content: center;
      margin-top: 10px;
    }
    .control-button, .arrow-button {
      width: 60px;
      height: 60px;
      font-size: 24px;
      margin: 5px;
      text-align: center;
      background-color: #f0f0f0;
      border: 1px solid #ccc;
      cursor: pointer;
    }
    .control-button:active, .arrow-button:active, .pressed {
      background-color: yellow;
    }
    #horizonContainer {
      position: relative;
      width: 450px;
      height: 450px;
      margin: 0 10px;
    }
    #outerCircle {
      position: absolute;
      top: 0;
      left: 0;
      z-index: 1;
    }
    #horizonCanvas {
      position: absolute;
      top: 0;
      left: 0;
      z-index: 2;
    }
    #controls {
      background-color: rgba(255, 255, 255, 0.8);
      padding: 20px;
      border-radius: 5px;
      box-shadow: 0 0 5px rgba(0, 0, 0, 0.2);
      display: flex;
      flex-direction: row;
      justify-content: space-between;
      align-items: center;
      width: 90%;
      flex-shrink: 0; 
    }
    .motor-control {
      display: flex;
      flex-direction: column;
      align-items: center;
      background-color: white;
      padding: 10px;
      border-radius: 5px;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
      flex: 1;
      margin: 5px;
    }
    .vertical-slider {
      writing-mode: bt-lr; 
      -webkit-appearance: slider-vertical;
      width: 8px;
      height: 200px;
    }
    .disabled-slider {
      pointer-events: none; 
      background-color: #d3d3d3; 
    }
    .full-width-button {
      width: calc(60px * 3 + 20px * 2);
    }
  </style>
</head>
<body>

  <div id="headerRow">
    <div class="section" id="heightContainer">
      <div>Inaltime:</div>
       <input type="range" min="0" max="200" step="1" class="vertical-slider disabled-slider" id="heightSliderContainer">
      <div> <span id="height_cm">0</span> cm</div>
    </div>
    <div id="header">Drona Esp32 (Marius)</div>
    <div class="section" id="sliderContainer">
      <div>Height Slider:</div>
      <input type="range" min="0" max="200" step="10" class="vertical-slider" id="heightSlider" oninput="updateHeight(this.value)">
      <div>Selected Height: <span id="selectedHeight">0</span> cm</div>
    </div>
  </div>
  
  <div id="mainContainer">
    <div id="orientationContainer">
      <div id="orientationHeader">Orientation Data</div>
      <div id="horizonRow">
        <div class="horizon-column">
          <div class="control-buttons">
            <button class="control-button" id="btnW" onmousedown="moveForward()" onmouseup="stopMovement()">W</button>
            <div class="control-buttons-row">
              <button class="control-button" id="btnA" onmousedown="moveLeft()" onmouseup="stopMovement()">A</button>
              <button class="control-button" id="btnS" onmousedown="moveBackward()" onmouseup="stopMovement()">S</button>
              <button class="control-button" id="btnD" onmousedown="moveRight()" onmouseup="stopMovement()">D</button>
            </div>
          </div>
        </div>
        <div class="horizon-column">
          <div id="horizonContainer">
            <canvas id="outerCircle" width="450" height="450"></canvas>
            <canvas id="horizonCanvas" width="450" height="450"></canvas>
          </div>
        </div>
        <div class="horizon-column">
          <div class="arrow-buttons">
            <button class="arrow-button" id="btnUp" onclick="highlightButton('Up')">↑</button>
            <div class="arrow-buttons-row">
              <button class="arrow-button" id="btnLeft" onmousedown="rotateLeft()" onmouseup="stopMovement()">←</button>
              <button class="arrow-button" id="btnDown" onclick="highlightButton('Down')">↓</button>
              <button class="arrow-button" id="btnRight" onmousedown="rotateRight()" onmouseup="stopMovement()">→</button>
            </div>
            <button class="arrow-button full-width-button" id="btnSpace" onclick="toggleTakeoff()">Space</button> 
          </div>
        </div>
      </div>
      <div id="paramData">
        <div>Roll: <span id="roll">0</span>°</div>
        <div>Pitch: <span id="pitch">0</span>°</div>
        <div>Yaw: <span id="yaw">0</span>°</div>
      </div>
    </div>
  </div>
  
  <div class="section" id="controls">
    <div class="motor-control">
      <div>Motor 1</div>
      <input type="range" id="motor1Speed" min="0" max="255" value="0" class="disabled-slider">
      <div>Speed: <span id="motor1SpeedValue">0</span>%</div>
    </div>
    <div class="motor-control">
      <div>Motor 2</div>
      <input type="range" id="motor2Speed" min="0" max="255" value="0" class="disabled-slider">
      <div>Speed: <span id="motor2SpeedValue">0</span>%</div>
    </div>
    <div class="motor-control">
      <div>Motor 3</div>
      <input type="range" id="motor3Speed" min="0" max="255" value="0" class="disabled-slider">
      <div>Speed: <span id="motor3SpeedValue">0</span>%</div>
    </div>
    <div class="motor-control">
      <div>Motor 4</div>
      <input type="range" id="motor4Speed" min="0" max="255" value="0" class="disabled-slider">
      <div>Speed: <span id="motor4SpeedValue">0</span>%</div>
    </div>
  </div>
  
  <script>
    let isTakingOff = false;
    let targetHeight = 0;
    let isFetchingData = false;

    async function fetchData() {
      if (isFetchingData) return; 
      isFetchingData = true;
    
      try {
        const response = await fetch('/data');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
    
        document.getElementById('height_cm').innerText = data.height_cm.toFixed(2);
        document.getElementById('heightSliderContainer').value = data.height_cm; 
        document.getElementById('selectedHeight').innerText = data.targetAltitude;
        document.getElementById('heightSlider').value = data.targetAltitude;
        document.getElementById('roll').innerText = data.roll.toFixed(2);
        document.getElementById('pitch').innerText = (data.pitch).toFixed(2);
        document.getElementById('yaw').innerText = data.yaw.toFixed(2);
        isTakingOff = data.takeoffCommand;
    
        document.getElementById('motor1Speed').value = data.motor1Speed;
        document.getElementById('motor1SpeedValue').innerText = (data.motor1Speed / 2.55).toFixed(0);
        document.getElementById('motor2Speed').value = data.motor2Speed;
        document.getElementById('motor2SpeedValue').innerText = (data.motor2Speed / 2.55).toFixed(0);
        document.getElementById('motor3Speed').value = data.motor3Speed;
        document.getElementById('motor3SpeedValue').innerText = (data.motor3Speed / 2.55).toFixed(0);
        document.getElementById('motor4Speed').value = data.motor4Speed;
        document.getElementById('motor4SpeedValue').innerText = (data.motor4Speed / 2.55).toFixed(0);
    
        drawHorizon(data.pitch, data.roll);
    
        if (isTakingOff) {
          document.getElementById('btnSpace').classList.add('pressed');
        } else {
          document.getElementById('btnSpace').classList.remove('pressed');
        }
    
      } catch (error) {
        console.error('Error fetching data:', error);
      } finally {
        isFetchingData = false;
      }
    
      setTimeout(fetchData, 100);
    }

    function updateHeight(value) {
      targetHeight = value;
      document.getElementById('selectedHeight').innerText = value;
      setTargetHeight(value);
    }

    async function setTargetHeight(height) {
      try {
        await fetch(`/setTargetHeight?height=${height}`);
      } catch (error) {
        console.error('Error setting target height:', error);
      }
    }

    async function takeoff() {
      console.log('Takeoff initiated'); 
      try {
        const response = await fetch('/takeoff');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        console.log('Takeoff response:', response.status); 
        isTakingOff = true;
        document.getElementById('btnSpace').classList.add('pressed');
      } catch (error) {
        console.error('Error during takeoff:', error);
      }
    }

    async function land() {
      console.log('Landing initiated'); 
      try {
        const response = await fetch('/cancelTakeoff');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        console.log('Cancel Takeoff response:', response.status); 
        isTakingOff = false;
        document.getElementById('btnSpace').classList.remove('pressed');
      } catch (error) {
        console.error('Error during landing:', error);
      }
    }

    async function toggleTakeoff() {
      if (isTakingOff) {
        await land();
      } else {
        await takeoff();
      }
    }

    function drawOuterCircle(ctx, radius, skyColor, groundColor) {
      ctx.translate(radius, radius);

      ctx.beginPath();
      ctx.arc(0, 0, radius, 0, Math.PI, true);
      ctx.fillStyle = skyColor;
      ctx.fill();
      ctx.closePath();

      ctx.beginPath();
      ctx.arc(0, 0, radius, 0, Math.PI, false);
      ctx.fillStyle = groundColor;
      ctx.fill();
      ctx.closePath();

      ctx.globalCompositeOperation = 'destination-out';
      ctx.beginPath();
      ctx.arc(0, 0, radius - 25, 0, 2 * Math.PI);
      ctx.fill();
      ctx.closePath();

      ctx.globalCompositeOperation = 'source-over';
    }

    function drawHorizon(pitch, roll) {
      const radius = horizonCanvas.width / 2;

      horizonCtx.clearRect(0, 0, horizonCanvas.width, horizonCanvas.height);

      horizonCtx.save();
      horizonCtx.translate(radius, radius);

      horizonCtx.beginPath();
      horizonCtx.arc(0, 0, radius, 0, 2 * Math.PI);
      horizonCtx.clip();

      horizonCtx.translate(0, pitch * 2);

      horizonCtx.fillStyle = 'skyblue';
      horizonCtx.fillRect(-horizonCanvas.width, -horizonCanvas.height, horizonCanvas.width * 2, horizonCanvas.height);

      horizonCtx.fillStyle = 'brown';
      horizonCtx.fillRect(-horizonCanvas.width, 0, horizonCanvas.width * 2, horizonCanvas.height);

      horizonCtx.strokeStyle = 'white';
      horizonCtx.lineWidth = 1;
      for (let i = -90; i <= 90; i += 10) {
        horizonCtx.beginPath();
        horizonCtx.moveTo(-radius, i * 2);
        horizonCtx.lineTo(radius, i * 2);
        horizonCtx.stroke();
      }

      horizonCtx.restore();

      horizonCtx.save();
      horizonCtx.translate(radius, radius);
      horizonCtx.beginPath();
      horizonCtx.arc(0, 0, radius, 0, 2 * Math.PI);
      horizonCtx.stroke();
      horizonCtx.restore();

      horizonCtx.save();
      horizonCtx.translate(radius, radius);
      horizonCtx.rotate(roll * Math.PI / 180);
      horizonCtx.strokeStyle = 'white';
      horizonCtx.lineWidth = 2;

      horizonCtx.beginPath();
      horizonCtx.moveTo(-20, 0);
      horizonCtx.lineTo(20, 0);
      horizonCtx.moveTo(0, -20);
      horizonCtx.lineTo(0, 20);
      horizonCtx.stroke();

      horizonCtx.beginPath();
      horizonCtx.moveTo(-radius / 2, 0);
      horizonCtx.lineTo(radius / 2, 0);
      horizonCtx.stroke();

      horizonCtx.restore();
    }

    function highlightButton(buttonId) {
      const button = document.getElementById(`btn${buttonId}`);
      if (button) {
        button.classList.add('pressed');
        setTimeout(() => {
          button.classList.remove('pressed');
        }, 200);
      }
    }

    async function moveForward() {
      highlightButton('W');
      try {
        await fetch('/moveForward');
      } catch (error) {
        console.error('Error moving forward:', error);
      }
    }

    async function moveBackward() {
      highlightButton('S');
      try {
        await fetch('/moveBackward');
      } catch (error) {
        console.error('Error moving backward:', error);
      }
    }

    async function moveLeft() {
      highlightButton('A');
      try {
        await fetch('/moveLeft');
      } catch (error) {
        console.error('Error moving left:', error);
      }
    }

    async function moveRight() {
      highlightButton('D');
      try {
        await fetch('/moveRight');
      } catch (error) {
        console.error('Error moving right:', error);
      }
    }

    async function rotateLeft() {
      highlightButton('Left');
      try {
        await fetch('/rotateLeft');
      } catch (error) {
        console.error('Error rotating left:', error);
      }
    }

    async function rotateRight() {
      highlightButton('Right');
      try {
        await fetch('/rotateRight');
      } catch (error) {
        console.error('Error rotating right:', error);
      }
    }

    async function stopMovement() {
      try {
        await fetch('/stopMovement');
      } catch (error) {
        console.error('Error stopping movement:', error);
      }
    }

    document.getElementById('heightSlider').value = 0;
    document.getElementById('heightSliderContainer').value = 0;

    document.addEventListener('keydown', function(event) {
      const key = event.key.toUpperCase();
      const buttonMap = {
        'W': 'btnW',
        'A': 'btnA',
        'S': 'btnS',
        'D': 'btnD',
        'ARROWUP': 'btnUp',
        'ARROWLEFT': 'btnLeft',
        'ARROWDOWN': 'btnDown',
        'ARROWRIGHT': 'btnRight',
        ' ': 'btnSpace' 
      };
      const buttonId = buttonMap[key];
      if (buttonId) {
        console.log(`Key pressed: ${key} -> Button ID: ${buttonId}`); 
        if (buttonId === 'btnSpace') {
          toggleTakeoff();
        } else if (buttonId === 'btnW') {
          moveForward();
        } else if (buttonId === 'btnS') {
          moveBackward();
        } else if (buttonId === 'btnA') {
          moveLeft();
        } else if (buttonId === 'btnD') {
          moveRight();
        } else if (buttonId === 'btnLeft') {
          rotateLeft();
        } else if (buttonId === 'btnRight') {
          rotateRight();
        }
        document.getElementById(buttonId).classList.add('pressed');
      }
    });

    document.addEventListener('keyup', function(event) {
      const key = event.key.toUpperCase();
      const buttonMap = {
        'W': 'btnW',
        'A': 'btnA',
        'S': 'btnS',
        'D': 'btnD',
        'ARROWUP': 'btnUp',
        'ARROWLEFT': 'btnLeft',
        'ARROWDOWN': 'btnDown',
        'ARROWRIGHT': 'btnRight',
        ' ': 'btnSpace'
      };
      const buttonId = buttonMap[key];
      if (buttonId) {
        document.getElementById(buttonId).classList.remove('pressed');
        stopMovement();
      }
    });

    const outerCanvas = document.getElementById('outerCircle');
    const outerCtx = outerCanvas.getContext('2d');
    const horizonCanvas = document.getElementById('horizonCanvas');
    const horizonCtx = horizonCanvas.getContext('2d');

    drawOuterCircle(outerCtx, outerCanvas.width / 2, 'skyblue', 'brown');

    fetchData();
  </script>

</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlContent);
}

void handleData() {
  String json = "{\"height_cm\":" + String(distance) + 
                ",\"roll\":" + String(roll) + 
                ",\"pitch\":" + String(-pitch) +  
                ",\"yaw\":" + String(yaw) + "," +
                "\"motor1Speed\":" + String(motorSpeeds[0]) + 
                ",\"motor2Speed\":" + String(motorSpeeds[1]) + 
                ",\"motor3Speed\":" + String(motorSpeeds[2]) + 
                ",\"motor4Speed\":" + String(motorSpeeds[3]) + "," +
                "\"targetAltitude\":" + String(targetAltitude) + "," +
                "\"takeoffCommand\":" + String(takeoffCommand ? "true" : "false") + "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  Wire.begin(22, 23);

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/setTargetHeight", handleSetTargetHeight);
  server.on("/allmotors", HTTP_GET, handleAllMotors); 
  server.on("/takeoff", HTTP_GET, handleTakeoff);
  server.on("/cancelTakeoff", HTTP_GET, handleCancelTakeoff);
  server.on("/moveForward", HTTP_GET, []() {
    moveForward();
    server.send(200, "text/plain", "Moving forward");
  });
  server.on("/moveBackward", HTTP_GET, []() {
    moveBackward();
    server.send(200, "text/plain", "Moving backward");
  });
  server.on("/moveLeft", HTTP_GET, []() {
    moveLeft();
    server.send(200, "text/plain", "Moving left");
  });
  server.on("/moveRight", HTTP_GET, []() {
    moveRight();
    server.send(200, "text/plain", "Moving right");
  });
  server.on("/rotateLeft", HTTP_GET, []() {
    rotateLeft();
    server.send(200, "text/plain", "Rotating left");
  });
  server.on("/rotateRight", HTTP_GET, []() {
    rotateRight();
    server.send(200, "text/plain", "Rotating right");
  });
  server.on("/stopMovement", HTTP_GET, []() {
    stopMovement();
    server.send(200, "text/plain", "Stopping movement");
  });
  server.begin();
  Serial.println("HTTP server started");

  setupSensors();
  delay(100); 
  setupDistanceSensor();
  setupMotors();
  setupAutopilot();
  setupPilot();
}

void loop() {
  updateDistanceSensor();
  updateSensors();
  server.handleClient();
  updateAutopilot(); 

  delay(100);
}
