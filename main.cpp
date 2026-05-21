#include "WiFi.h"
#include "WebServer.h"
#include "DHT.h"
#include <ESP32Servo.h>

// Define Hardware Pins
#define DHTPIN 25
#define DHTTYPE DHT22
#define GASPIN 32
#define SERVO_PIN 17
#define RED_LED_PIN 19  
#define BLUE_LED_PIN 18
#define FAN_LED_PIN 16

// Initialize objects
DHT dht(DHTPIN, DHTTYPE);
Servo roofServo;

// Wokwi's virtual network credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

WebServer server(80);

// ---- HTML AND CSS PART (DONT DELETE) -----
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8"/>
<meta name="viewport" content="width=device-width, initial-scale=1.0"/>
<title>ClimaCast GH Dashboard</title>
<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/@tabler/icons-webfont@2.44.0/tabler-icons.min.css"/>
<style>
  * { margin: 0; padding: 0; box-sizing: border-box; }
  body { background: #f5f0e8; color: #2d2d2d; font-family: 'Segoe UI', system-ui, sans-serif; min-height: 100vh; }
  .topbar { background: #fff; border-bottom: 1px solid #e8e0d0; padding: 0 24px; height: 56px; display: flex; align-items: center; justify-content: space-between; }
  .logo { display: flex; align-items: center; gap: 10px; }
  .logo-icon { width: 34px; height: 34px; background: #1a6b3a; border-radius: 10px; display: flex; align-items: center; justify-content: center; font-size: 18px; }
  .logo-name { font-size: 17px; font-weight: 800; color: #1a6b3a; letter-spacing: -0.5px; }
  .logo-sub { font-size: 11px; color: #8a7e6e; }
  .topbar-right { display: flex; align-items: center; gap: 14px; }
  .online-pill { background: #e8f5ee; border: 1px solid #b8ddc8; color: #1a6b3a; font-size: 11px; font-weight: 600; padding: 4px 12px; border-radius: 20px; display: flex; align-items: center; gap: 6px; }
  .dot { width: 6px; height: 6px; border-radius: 50%; background: #1a6b3a; animation: pulse 2s infinite; }
  @keyframes pulse { 0%,100% { opacity:1; } 50% { opacity:0.4; } }
  .notif-btn { width: 34px; height: 34px; border-radius: 8px; background: #f5f0e8; border: 1px solid #e8e0d0; display: flex; align-items: center; justify-content: center; cursor: pointer; color: #8a7e6e; font-size: 16px; position: relative; }
  .badge { position: absolute; top: -4px; right: -4px; width: 16px; height: 16px; background: #e05a2b; border-radius: 50%; font-size: 9px; display: flex; align-items: center; justify-content: center; color: white; font-weight: 700; }
  .avatar { width: 34px; height: 34px; border-radius: 50%; background: #1a6b3a; display: flex; align-items: center; justify-content: center; font-size: 12px; font-weight: 700; color: white; }
  .main {height: calc(100vh - 56px); }
  .sidebar { background: #fff; border-right: 1px solid #e8e0d0; padding: 20px 12px; }
  .nav-section { font-size: 10px; color: #b0a898; text-transform: uppercase; letter-spacing: 1px; margin: 16px 0 8px 8px; }
  .nav-item { display: flex; align-items: center; gap: 10px; padding: 8px 10px; border-radius: 8px; font-size: 13px; color: #6e6258; cursor: pointer; margin-bottom: 2px; }
  .nav-item:hover { background: #f5f0e8; }
  .nav-item.active { background: #e8f5ee; color: #1a6b3a; font-weight: 600; }
  .nav-item i { font-size: 16px; }
  .content { overflow-y: auto; padding: 24px; }
  .page-header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 20px; }
  .page-title { font-size: 20px; font-weight: 800; color: #1a2d1e; }
  .page-sub { font-size: 12px; color: #8a7e6e; margin-top: 2px; }
  .week-wrap { display: flex; flex-direction: column; align-items: flex-start; } 
  .week-label { font-size: 10px; color: #b0a898; font-weight: 700; text-transform: uppercase; letter-spacing: 0.8px; margin-bottom: 6px; margin-left: 4px; } 
  .week-select { 
    background-color: #fff; 
    border: 1px solid #e8e0d0; 
    border-radius: 12px; 
    padding: 8px 36px 8px 14px; 
    width: 160px; 
    font-size: 13px; 
    color: #1a6b3a; 
    font-weight: 700; 
    cursor: pointer; 
    outline: none; 
    box-shadow: 0 2px 4px rgba(0,0,0,0.02); 
    transition: all 0.2s ease;
    appearance: none; 
    -webkit-appearance: none;
    background-image: url("data:image/svg+xml;charset=UTF-8,%3csvg xmlns='http://www.w3.org/2000/svg' width='16' height='16' viewBox='0 0 24 24' fill='none' stroke='%231a6b3a' stroke-width='2.5' stroke-linecap='round' stroke-linejoin='round'%3e%3cpolyline points='6 9 12 15 18 9'%3e%3c/polyline%3e%3c/svg%3e");
    background-repeat: no-repeat;
    background-position: right 14px center; 
  }
  .week-select:hover { border-color: #b8ddc8; background-color: #e8f5ee; }
  .ts { font-size: 11px; color: #b0a898; background: #fff; border: 1px solid #e8e0d0; border-radius: 6px; padding: 4px 10px; }
  .alert-banner { background: #fff8e6; border: 1px solid #f5c842; border-left: 4px solid #f5c842; border-radius: 8px; padding: 10px 14px; margin-bottom: 20px; display: flex; align-items: center; gap: 10px; }
  .alert-banner i { color: #d97706; font-size: 18px; }
  .alert-text { font-size: 12px; color: #92400e; flex: 1; }
  .alert-text strong { display: block; font-size: 13px; margin-bottom: 2px; }
  .view-link { font-size: 11px; color: #1a6b3a; font-weight: 600; cursor: pointer; text-decoration: underline; }
  .cards-row { display: grid; grid-template-columns: repeat(3, 1fr); gap: 14px; margin-bottom: 20px; }
  .card { background: #fff; border: 1px solid #e8e0d0; border-radius: 12px; padding: 16px; position: relative; }
  .card-label { font-size: 11px; color: #b0a898; font-weight: 800; text-transform: uppercase; letter-spacing: 0.8px; display: flex; align-items: center; gap: 6px; }
  .card-label i { font-size: 14px; }
  .card-value { font-size: 30px; font-weight: 800; margin: 8px 0 0; letter-spacing: -1px; }
  .card-unit { font-size: 13px; color: #8a7e6e; font-weight: 400; }
  .card-thresh { font-size: 11px; color: #b0a898; margin-top: 4px; }
  .card-status { position: absolute; top: 16px; right: 16px; font-size: 10px; text-transform: uppercase; letter-spacing: 1px; padding: 4px 10px; border-radius: 20px; font-weight: 800; }
  .s-ok { background-color: #e8f5ee; color: #1a6b3a; }
  .s-warn { background-color: #fff8e6; color: #d97706; }
  .s-bad { background-color: #fef2f2; color: #dc2626; }
  .cv-temp { color: #e05a2b; }
  .cv-hum { color: #2563eb; }
  .cv-co2 { color: #7c3aed; }
  .cv-light { color: #d97706; }
  .mid-row { display: grid; grid-template-columns: 2fr 1fr; gap: 14px; margin-bottom: 20px; }
  .section-title { font-size: 13px; font-weight: 700; color: #1a2d1e; margin-bottom: 14px; display: flex; align-items: center; gap: 8px; }
  .section-title i { font-size: 15px; color: #1a6b3a; }
  .chart-wrap { position: relative; height: 180px; }
  .actuator-list { display: flex; flex-direction: column; gap: 10px; }
  .actuator { background: #f5f0e8; border: 1px solid #e8e0d0; border-radius: 8px; padding: 12px; }
  .act-row { display: flex; align-items: center; justify-content: space-between; }
  .act-name { font-size: 12px; font-weight: 600; color: #2d2d2d; }
  .act-sub { font-size: 11px; color: #8a7e6e; margin-top: 2px; }
  .toggle { display: flex; align-items: center; }
  .toggle-pill { width: 36px; height: 20px; border-radius: 10px; display: flex; align-items: center; padding: 2px; cursor: pointer; transition: background 0.2s; }
  .toggle-on { background: #1a6b3a; }
  .toggle-off { background: #d1cfc8; }
  .toggle-thumb { width: 16px; height: 16px; border-radius: 50%; background: white; transition: transform 0.2s; }
  .toggle-on .toggle-thumb { transform: translateX(16px); }
  .bot-row { display: grid; grid-template-columns: 1fr 1fr; gap: 14px; }
  .event-list { display: flex; flex-direction: column; gap: 8px; }
  .event-item { display: flex; align-items: flex-start; gap: 10px; padding: 10px; background: #f5f0e8; border-radius: 8px; }
  .event-icon { width: 28px; height: 28px; border-radius: 6px; display: flex; align-items: center; justify-content: center; font-size: 13px; flex-shrink: 0; }
  .ei-ok { background: #e8f5ee; color: #1a6b3a; }
  .ei-warn { background: #fff8e6; color: #d97706; }
  .ei-info { background: #eff6ff; color: #2563eb; }
  .ei-ac { background: #f5f3ff; color: #7c3aed; }
  .event-text { font-size: 12px; color: #4d4437; flex: 1; }
  .event-time { font-size: 10px; color: #b0a898; margin-top: 2px; }
  .sched-list { display: flex; flex-direction: column; gap: 8px; }
  .sched-item { display: flex; align-items: center; justify-content: space-between; padding: 10px 12px; background: #f5f0e8; border-radius: 8px; border: 1px solid #e8e0d0; }
  .sched-time { font-size: 13px; font-weight: 700; color: #1a2d1e; }
  .sched-desc { font-size: 11px; color: #8a7e6e; margin-top: 1px; }
  .sbadge { font-size: 10px; padding: 3px 9px; border-radius: 10px; font-weight: 600; }
  .sb-done { background: #e8f5ee; color: #1a6b3a; }
  .sb-act { background: #eff6ff; color: #2563eb; }
  .sb-next { background: #f5f0e8; color: #8a7e6e; border: 1px solid #e8e0d0; }
  .force-btn { width: 100%; padding: 10px; background: #1a6b3a; border: none; border-radius: 8px; color: white; font-size: 13px; font-weight: 700; cursor: pointer; margin-top: 12px; display: flex; align-items: center; justify-content: center; gap: 8px; }
</style>
</head>
<body>
<!-- SECTION: HEADER -->

<div class="topbar">
  <div class="logo">
    <div class="logo-icon">🌱</div>
    <div>
      <div class="logo-name">ClimaCast</div>
      <div class="logo-sub">Greenhouse Climate Control</div>
    </div>
  </div>
</div>


<!-- SECTION: MAIN CONTENT -->

<div class="main">
  <div class="content">
    <div class="page-header">
      <div>
        <div class="page-title">Greenhouse Dashboard</div>
        <div class="page-sub">Greenhouse Unit — Lettuce Crop</div>
      </div>
      <div class="week-wrap">
        <div class="week-label">Current Week</div>
        <select class="week-select" id="lettuce-week" onchange="console.log('Growth Stage changed to: ' + this.value)">
          <option value="1">Week 1</option>
          <option value="2">Week 2</option>
          <option value="3">Week 3</option>
          <option value="4" selected>Week 4</option>
          <option value="5">Week 5</option>
          <option value="6">Week 6</option>
        </select>
      </div>
    </div>


    <!-- SECTION: ALERT BANNER -->

    <div class="alert-banner">
      <i class="ti ti-alert-triangle"></i>
      <div class="alert-text"><strong>⚠ CO₂ Level Rising — Ventilation Auto-Triggered</strong>CO₂ reached 1,320 ppm (threshold: 1,200 ppm). Exhaust Fan 2 activated automatically by edge logic.</div>
    </div>


    <!-- SECTION: ENVIRONMENTAL SENSORS-->

    <div class="cards-row">


      <!-- SECTION: TEMPERATURE CARD -->

      <div class="card">
        <div class="card-label"><i class="ti ti-thermometer"></i> Temperature</div>
        <div class="card-value cv-temp"><span id="val-temp">--</span><span class="card-unit"> °C</span></div>
        <div class="card-thresh" id="thresh-temp">Threshold: --</div>
        <div class="card-status" id="status-temp">--</div>
      </div>


      <!-- SECTION: HUMIDITY CARD -->

      <div class="card">
        <div class="card-label"><i class="ti ti-droplet"></i> Humidity</div>
        <div class="card-value cv-hum"><span id="val-hum">--</span><span class="card-unit"> %</span></div>
        <div class="card-thresh" id="thresh-hum">Threshold: --</div>
        <div class="card-status" id="status-hum">--</div>
      </div>


      <!-- SECTION: CO2 CARD -->

      <div class="card">
        <div class="card-label"><i class="ti ti-cloud"></i> CO₂ Level</div>
        <div class="card-value cv-co2"><span id="val-co2">--</span><span class="card-unit"> ppm</span></div>
        <div class="card-thresh" id="thresh-co2">Threshold: --</div>
        <div class="card-status" id="status-co2">--</div>
      </div>
    </div>


    <div class="mid-row">


      <!-- SECTION: CLIMATE TRENDS GRAPH, MODIFY CHART IF NEEDED -->

      <div class="card">
        <div class="section-title"><i class="ti ti-chart-line"></i> Climate Trends — Last 8 Hours</div>
        <div class="chart-wrap">
          <canvas id="climateChart"></canvas>
        </div>
      </div>


      <!-- SECTION: ACTUATOR CONTROL -->

      <div class="card">
        <div class="section-title"><i class="ti ti-settings"></i> Actuator Control</div>
        <div class="actuator-list">


          <!-- SECTION: FANS -->

          <div class="actuator"><div class="act-row"><div><div class="act-name">Exhaust Fan</div><!--<div class="act-sub">Manual Control</div>--></div><div class="toggle"><div id="t-fan" class="toggle-pill toggle-off" onclick="toggleDevice('fan', 't-fan', 1, 0)"><div class="toggle-thumb"></div></div></div></div></div>
          

          <!-- SECTION: SERVO -->

          <div class="actuator">
            <div class="act-row">
              <div>
                <div class="act-name">Roof Vent (Servo)</div>
                <div class="act-sub" id="servo-label">Position: 0&deg;</div>
              </div>
              <div style="display: flex; align-items: center;">
                <input type="range" id="servo-slider" min="0" max="180" value="0" 
                       oninput="document.getElementById('servo-label').innerText = 'Position: ' + this.value + '&deg;'" 
                       onchange="updateServo(this.value)"
                       style="accent-color: #1a6b3a; cursor: pointer; width: 90px;">
              </div>
            </div>
          </div>

          <!-- SECTION: RED GROW LIGHTS -->

          <div class="actuator"><div class="act-row"><div><div class="act-name">Red Grow Lights</div><!--<div class="act-sub">Phase A</div>--></div><div class="toggle"><div id="t-red" class="toggle-pill toggle-off" onclick="toggleDevice('redlight', 't-red', 1, 0)"><div class="toggle-thumb"></div></div></div></div></div>

          <!-- SECTION: BLUE GROW LIGHTS -->

          <div class="actuator"><div class="act-row"><div><div class="act-name">Blue Grow Lights</div><!--<div class="act-sub">Phase B</div>--></div><div class="toggle"><div id="t-blue" class="toggle-pill toggle-off" onclick="toggleDevice('bluelight', 't-blue', 1, 0)"><div class="toggle-thumb"></div></div></div></div></div>
        </div>
      </div>
    </div>
  </div>
</div>
<script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/4.4.1/chart.umd.js"></script>
<script>
const climateChart = new Chart(document.getElementById('climateChart'), {
  type: 'line',
  data: {
    labels: [], 
    datasets: [
      { label: 'Temp (°C)', data: [], borderColor: '#e05a2b', tension: 0.4, borderWidth: 2, pointRadius: 3, yAxisID: 'y' },
      { label: 'Humidity (%)', data: [], borderColor: '#2563eb', tension: 0.4, borderWidth: 2, pointRadius: 3, borderDash: [4,3], yAxisID: 'y' },
      { label: 'CO₂ (×10 ppm)', data: [], borderColor: '#7c3aed', tension: 0.4, borderWidth: 2, pointRadius: 3, borderDash: [2,4], yAxisID: 'y' }
    ]
  },
  options: {
    responsive: true, maintainAspectRatio: false,
    plugins: { legend: { labels: { color: '#6e6258', font: { size: 10 }, boxWidth: 12 } } }
  }
});

// Define Thresholds For Each Week 
const growthStages = {
  "1": { minT: 20, maxT: 25, minH: 70, maxH: 80, maxC: 800 },
  "2": { minT: 18, maxT: 24, minH: 65, maxH: 75, maxC: 800 },
  "3": { minT: 17, maxT: 23, minH: 60, maxH: 70, maxC: 1000 },
  "4": { minT: 17, maxT: 22, minH: 50, maxH: 70, maxC: 1000 },
  "5": { minT: 15, maxT: 20, minH: 50, maxH: 60, maxC: 1000 },
  "6": { minT: 15, maxT: 18, minH: 50, maxH: 60, maxC: 1000 }
};

// Fetch new data every 2 seconds and update the HTML elements
setInterval(function() {
  fetch('/data')
    .then(response => response.json())
    .then(data => {
      document.getElementById("val-temp").innerText = data.temperature.toFixed(1);
      document.getElementById("val-hum").innerText = data.humidity.toFixed(1);
      document.getElementById("val-co2").innerText = data.co2;

      const currentWeek = document.getElementById("lettuce-week").value;
      const limits = growthStages[currentWeek]; 

      document.getElementById("thresh-temp").innerText = `Threshold: ${limits.minT} - ${limits.maxT} °C`;
      document.getElementById("thresh-hum").innerText = `Threshold: ${limits.minH} - ${limits.maxH} %`;
      document.getElementById("thresh-co2").innerText = `Threshold: < ${limits.maxC} ppm`;

      const statusTemp = document.getElementById("status-temp");
      if (data.temperature > limits.maxT) {
        statusTemp.className = "card-status s-warn";
        statusTemp.innerText = "Warning";
      } else if (data.temperature < limits.minT) {
        statusTemp.className = "card-status s-warn";
        statusTemp.innerText = "Warning";
      } else {
        statusTemp.className = "card-status s-ok";
        statusTemp.innerText = "Normal";
      }

      const statusHum = document.getElementById("status-hum");
      if (data.humidity > limits.maxH) {
        statusHum.className = "card-status s-warn";
        statusHum.innerText = "Warning";
      } else if (data.humidity < limits.minH) {
        statusHum.className = "card-status s-warn";
        statusHum.innerText = "Warning";
      } else {
        statusHum.className = "card-status s-ok";
        statusHum.innerText = "Normal";
      }

      const statusCo2 = document.getElementById("status-co2");
      if (data.co2 > limits.maxC) {
        statusCo2.className = "card-status s-warn";
        statusCo2.innerText = "Warning";
      } else {
        statusCo2.className = "card-status s-ok";
        statusCo2.innerText = "Normal";
      }

      const now = new Date();
      if (!window.lastChartTime || now.getTime() - window.lastChartTime >= 60000) {
        window.lastChartTime = now.getTime();
        
        const timeString = now.getHours().toString().padStart(2, '0') + ':' + 
                           now.getMinutes().toString().padStart(2, '0');

        climateChart.data.labels.push(timeString);
        climateChart.data.datasets[0].data.push(data.temperature);
        climateChart.data.datasets[1].data.push(data.humidity);
        climateChart.data.datasets[2].data.push(data.co2 / 10);

        if (climateChart.data.labels.length > 8) {
          climateChart.data.labels.shift();
          climateChart.data.datasets[0].data.shift();
          climateChart.data.datasets[1].data.shift();
          climateChart.data.datasets[2].data.shift();
        }

        climateChart.update();
      }
    })
    .catch(error => console.error("Error fetching data:", error));
}, 2000);

// Hardware Control Function
function toggleDevice(device, elemId, onValue, offValue) {
  const pill = document.getElementById(elemId);
  let targetValue = 0;

  // Swap UI colors visually
  if (pill.classList.contains('toggle-off')) {
    pill.classList.remove('toggle-off');
    pill.classList.add('toggle-on');
    targetValue = onValue;
  } else {
    pill.classList.remove('toggle-on');
    pill.classList.add('toggle-off');
    targetValue = offValue;
  }

  // Send the command directly to the ESP32 backend route
  fetch(`/control?device=${device}&state=${targetValue}`)
    .then(response => console.log(device + " set to " + targetValue))
    .catch(err => console.error("Failed to transmit command:", err));
}

// Dedicated Servo Control Function
function updateServo(angle) {
  fetch(`/control?device=servo&state=${angle}`)
    .then(response => console.log("Servo moved to " + angle + " degrees"))
    .catch(err => console.error("Failed to transmit command:", err));
}
</script>
</body>
</html>
)rawliteral";
// ---- END OF HTML AND CSS PART -----

void handleRoot() {
  server.send(200, "text/html", index_html);
}


void handleData() {
  // CHECK: Match the "t", "h" and "simulatedCoPPM" variable
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int rawGas = analogRead(GASPIN);

  if (isnan(t) || isnan(h)) {
    t = 0.0;
    h = 0.0;
  }

  int simulatedCoPPM = map(rawGas, 0, 4095, 10, 1000);

  // CHECK: Match the "t", "h" and "simulatedCoPPM" variable inside the String() function
  String json = "{";
  json += "\"temperature\":" + String(t) + ",";
  json += "\"humidity\":" + String(h) + ",";
  json += "\"co2\":" + String(simulatedCoPPM);
  json += "}";

  server.send(200, "application/json", json);
}

void handleControl() {
  // Check if the URL has the required arguments (e.g., /control?device=fan&state=1)
  if (server.hasArg("device") && server.hasArg("state")) {
    String device = server.arg("device");
    int state = server.arg("state").toInt();

    Serial.print("Web Command Received -> Device: ");
    Serial.print(device);
    Serial.print(" | State: ");
    Serial.println(state);

    if (device == "fan") {
      digitalWrite(FAN_LED_PIN, state ? HIGH : LOW);
    } 
    else if (device == "redlight") {
      digitalWrite(RED_LED_PIN, state ? HIGH : LOW);
    } 
    else if (device == "bluelight") {
      digitalWrite(BLUE_LED_PIN, state ? HIGH : LOW);
    } 
    else if (device == "servo") {
      roofServo.write(state);
    }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  pinMode(GASPIN, INPUT);
  roofServo.attach(SERVO_PIN);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(FAN_LED_PIN, OUTPUT);

  // Set initial default states (Turn everything off at boot)
  roofServo.write(0); 
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(FAN_LED_PIN, LOW);

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define Server Routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/data", HTTP_GET, handleData); 
  server.on("/control", HTTP_GET, handleControl);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(2);
}