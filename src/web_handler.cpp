#include "web_handler.h"

WebServer server(80);  // Creates a web server that listens on port 80 (default for HTTP)

extern hw_timer_t *My_timer;

// 'Training mode' variables 
extern int msgCounter;
unsigned long acquisitionStartMillis = 0;
String targetLabel = "Normal Walk";  // Default target label

bool isAcquiring = false;
String loggedInUserId = "";



void InitialWebServerSetup() {
    server.on("/", handleRoot);  // Register dispatcher only once
    server.begin();              // Start server
}


void handleRoot() {
    if (status.userId.length() == 0 || !status.hasActiveService) {
        // Not logged in or no active service
        server.sendHeader("Location", "/login");
        server.send(302, "text/plain", "Redirecting to login...");
        return;
    }
    if (currentMode == TRAINING || currentMode == TESTING) {
        handleTrainingRoot();     // Sends training HTML
    } else {
        handleProductionRoot();   // Sends production HTML
    }
}

void handleLogin() {
  String html = "<html><body>";
  html += "<h2>User Login</h2>";
  html += "<form action=\"/submit_login\" method=\"POST\">";
  html += "Username: <input type=\"text\" name=\"username\">";
  html += "<input type=\"submit\" value=\"Login\">";
  html += "</form>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}



void handleLoginSubmit() {
  if (server.hasArg("username")) {
    String username = server.arg("username");
    Serial.println("Username entered: " + username);

    // Call API and retrieve status
    status = safeFetchUserId(username);
    loggedInUserId = status.userId;

    // Build response page
    String response = "<html><body>";
    if (status.success) {
      response += "<h3>Welcome, " + username + "</h3>";
      response += "<p>User ID: " + status.userId + "</p>";
      response += "<p>Active Service: " + String(status.hasActiveService ? "Yes" : "No") + "</p>";

      if (status.serviceCount > 0) {
        response += "<p>Select a service:</p><form method='POST' action='/select_service'>";
        response += "<select name='service_id'>";
        for (int i = 0; i < status.serviceCount; ++i) {
          response += "<option value='" + status.services[i].id + "'";
          if (status.services[i].id == status.selectedServiceId) {
            response += " selected";
          }
          response += ">";
          response += status.services[i].companyName + " (ID: " + status.services[i].id + ")";
          response += "</option>";
        }
        response += "</select><br><br>";
        response += "<input type='submit' value='Continue'>";
        response += "</form>";
      } else {
        response += "<p>No services found.</p>";
      }
    } else {
      response += "<p>No record found or user inactive.</p>";
    }

    response += "</body></html>";
    server.send(200, "text/html", response);
  } else {
    server.send(400, "text/plain", "Missing username");
  }
}

void handleSelectServiceSubmit() {
  if (server.hasArg("service_id")) {
    String selectedServiceIdStr = server.arg("service_id");
    status.selectedServiceId = selectedServiceIdStr.toInt();  // update global status
    Serial.println("Service selected: " + status.selectedServiceId);

    // Determine where to redirect based on mode
    String redirectTarget = (currentMode == TRAINING || currentMode == TESTING) ? "/training" : "/production";
    server.sendHeader("Location", redirectTarget);
    server.send(302, "text/plain", "Redirecting to service interface...");
  } else {
    server.send(400, "text/plain", "Missing service ID");
  }
}




void handleLogout() {
  status.userId = "";
  status.hasActiveService = false;
  status.success = false;
  loggedInUserId = "";
  server.sendHeader("Location", "/login");
  server.send(302, "text/plain", "Logging out...");
}





void handleProductionRoot() {
    String html = "<html><body>";
    html += "<h2>ESP32 Data Acquisition</h2>";
    html += "<p>Status: <span id='status'>STOPPED</span></p>";
    html += "<button onclick=\"calibrate()\">CALIBRATE</button>";
    html += "<button id='startBtn' onclick=\"startAcquisition()\" disabled>START</button>";
    html += "<button id='stopBtn' onclick=\"stopAcquisition()\" disabled>STOP</button>";
    html += "<br><br><button onclick=\"window.location.href='/logout'\">Log Out</button>";
    html += "<p id='message'></p>";

    html += "<script>";
    html += "let statusInterval = null;";
    html += "let isCalibrated = false;";

    html += "function startAcquisition() {";
    html += "  document.getElementById('startBtn').disabled = true;";
    html += "  fetch('/start').then(() => {";
    html += "    document.getElementById('stopBtn').disabled = false;";
    html += "  });";
    html += "}";

    html += "function stopAcquisition() {";
    html += "  document.getElementById('stopBtn').disabled = true;";
    html += "  fetch('/stop').then(() => {";
    html += "    if (isCalibrated) document.getElementById('startBtn').disabled = false;";
    html += "  });";
    html += "}";

    html += "function calibrate() {";
    html += "  if (statusInterval) clearInterval(statusInterval);";
    html += "  document.getElementById('message').innerText = 'Calibrating... Please stand still.';";
    html += "  document.getElementById('startBtn').disabled = true;";
    html += "  document.getElementById('stopBtn').disabled = true;";
    html += "  fetch('/calibrate').then(r => r.text()).then(t => {";
    html += "    document.getElementById('message').innerHTML = t;";
    html += "    isCalibrated = true;";
    html += "    updateStatus();";
    html += "    startPolling();";
    html += "  });";
    html += "}";

    html += "function updateStatus() {";
    html += "  fetch('/status').then(r => r.text()).then(t => {";
    html += "    document.getElementById('status').innerText = t;";
    html += "    updateButtons(t);";
    html += "  });";
    html += "}";

    html += "function updateButtons(statusText) {";
    html += "  let isRunning = (statusText || document.getElementById('status').innerText).includes('RUNNING');";
    html += "  document.getElementById('startBtn').disabled = !isCalibrated || isRunning;";
    html += "  document.getElementById('stopBtn').disabled = !isRunning;";
    html += "}";

    html += "function startPolling() {";
    html += "  statusInterval = setInterval(updateStatus, 500);";
    html += "}";

    html += "window.onload = () => {";
    html += "  updateButtons();";  // don't enable until calibrated
    html += "};";

    html += "</script>";
    
    html += "</body></html>";

    server.send(200, "text/html", html);
}

//------------------------------------
// Standard Data Acquisition Handlers
//------------------------------------

void handleStart() {
    if (!status.hasActiveService) {
        server.send(403, "text/plain", "Access denied: No active service.");
        return;
    }
    isAcquiring = true;
    timerAlarmEnable(My_timer);
    server.send(200, "text/plain", "Acquisition STARTED");
}

void handleStop() {
    isAcquiring = false;
    timerAlarmDisable(My_timer);
    if (!status.hasActiveService) {
        server.send(403, "text/plain", "Access denied: No active service.");
        return;
    }
    server.send(200, "text/plain", "Acquisition STOPPED");
    safeTriggerFastAPI();
    msgCounter = 0;
}

void handleUnavailableInProduction() {
    String html = "<html><body>";
    html += "<h2> This page is not available in Production Mode.</h2>";
    html += "<p>Please return to the <a href='/'>main interface</a>.</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleStatus() {
    server.send(200, "text/plain", isAcquiring ? "RUNNING" : "STOPPED");
}

void StartWebServer() {
    server.on("/", handleRoot);  // optional: this redirects to /login
    server.on("/production", handleProductionRoot);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);
    server.on("/status", handleStatus);
    server.on("/calibrate", handleCalibration); 


    server.on("/login", handleLogin);
    server.on("/submit_login", HTTP_POST, handleLoginSubmit);
    server.on("/select_service", HTTP_POST, handleSelectServiceSubmit);
    server.on("/logout", handleLogout);  // ✅ new route

    server.on("/training", handleUnavailableInProduction);  // block if wrong mode

    // Handle Training leftovers
    server.on("/train_status", handleUnavailableInProduction);
    server.on("/train_info", handleUnavailableInProduction);
    server.on("/train_start", handleUnavailableInProduction);
    server.on("/train_stop", handleUnavailableInProduction);
    server.on("/label_update", handleUnavailableInProduction);

    server.on("/favicon.ico", []() {
        server.send(204);
    }); 

    server.begin();
}


//------------------------------------
// Training Mode Handlers
//------------------------------------

void handleTrainingRoot() {
    Serial.println("handleTrainingRoot");
  Serial.print("UserId:");
  Serial.println(status.userId);
  Serial.print("ServiceId:");
  Serial.println(status.selectedServiceId);

    String html = "<html><body>";
    if (!status.hasActiveService) {
        String html = "<html><body>";
        html += "<h2> Access Denied</h2>";
        html += "<p>Your service is not active. Please contact admin or renew your access.</p>";
        html += "<a href='/login'>Return to Login</a>";
        html += "</body></html>";
        server.send(403, "text/html", html);
        return;
    }
    html += "<h2>ESP32 " + String(TESTING_MODE ? "Testing" : "Training") + " Data Acquisition</h2>";
    html += "<button onclick=\"calibrate()\">CALIBRATE</button><br><br>";
    html += "<p>Status: <span id='train_status'>STOPPED</span></p>";
    html += "<p id='calibration_result'></p>";
    html += "<p>Acquisition Info: <span id='info'>Time: 0s | Messages Sent: 0</span></p>";

    html += "<p>Select Label:</p>";
    html += "<input type='radio' class='label-radio' name='label' value='Normal Walk' checked> Normal Walk<br>";
    html += "<input type='radio' class='label-radio' name='label' value='Limping'> Limping<br>";    

    html += "<button id='startBtn' onclick=\"startTraining()\" disabled>START</button>";
    html += "<button id='stopBtn' onclick=\"fetch('/train_stop')\" disabled>STOP</button>";

    // if (!TESTING_MODE) {
    html += "<button id='sendBtn' onclick=\"sendDataToFastAPI()\">SEND</button>";

    html += "<br><br><button onclick=\"window.location.href='/logout'\">Log Out</button>";
    // }

    html += "<script>";

    html += "let updateInterval = setInterval(() => { updateUI();";
    if (TESTING_MODE) {
        html += "  let label = document.querySelector('input[name=label]:checked').value;";
        html += "  fetch('/label_update?label=' + encodeURIComponent(label));";
    }
    html += " }, " + String(TESTING_MODE ? 250 : 1000) + ");";

    html += "function startTraining() {";
    html += "  let label = document.querySelector('input[name=label]:checked').value;";
    html += "  fetch('/train_start?label=' + encodeURIComponent(label));";
    html += "}";

    html += "function sendDataToFastAPI() {";
    html += "  fetch('/send_now').then(() => { alert('FastAPI triggered!'); });";
    html += "}";

    html += "function calibrate() {";
    html += "  clearInterval(updateInterval);";    
    html += "  setTimeout(() => {";
    html += "    document.getElementById('train_status').innerText = 'Calibrating...';";
    html += "  }, 10);";    
    html += "  document.getElementById('startBtn').disabled = true;";
    html += "  document.getElementById('stopBtn').disabled = true;";    
    html += "  fetch('/calibrate').then(r => r.text()).then(t => {";
    html += "    document.getElementById('calibration_result').innerHTML = t;";
    html += "    document.getElementById('startBtn').disabled = false;";
    html += "    document.getElementById('stopBtn').disabled = false;";    
    html += "    setTimeout(() => {";
    html += "      updateInterval = setInterval(() => { updateUI();";
    if (TESTING_MODE) {
        html += "  let label = document.querySelector('input[name=label]:checked').value;";
        html += "  fetch('/label_update?label=' + encodeURIComponent(label));";
    }
    html += " }, " + String(TESTING_MODE ? 250 : 1000) + ");";
    html += "    }, 250);";  
    html += "  });";
    html += "}";

    html += "function updateUI() {";
    html += "  fetch('/train_status').then(r => r.text()).then(t => {";
    html += "    if (t.includes('Production Mode')) {";
    html += "      document.getElementById('train_status').innerText = 'Redirecting to main interface...';";
    html += "      setTimeout(() => { window.location.href = '/'; }, 1000);";
    html += "      return;";
    html += "    }";
    html += "    document.getElementById('train_status').innerText = t;";
    html += "    document.getElementById('startBtn').disabled = t.startsWith('RUNNING');";
    if (!TESTING_MODE) {
        html += "    document.querySelectorAll('.label-radio').forEach(rb => rb.disabled = isRunning);";
    }
    html += "  });";
    
    html += "  fetch('/train_info').then(r => r.text()).then(t => {";
    html += "    if (t.includes('Production Mode')) {";
    html += "      document.getElementById('info').innerText = 'Redirecting to main interface...';";
    html += "      setTimeout(() => { window.location.href = '/'; }, 1000);";
    html += "      return;";
    html += "    }";
    html += "    document.getElementById('info').innerText = t;";
    html += "  });";
    html += "}";

    html += "</script>";
    
    html += "</body></html>";

    server.send(200, "text/html", html);
}

//-----------------------------------
// Training Handlers
//-----------------------------------

void handleTrainStart() {
    if (!status.hasActiveService) {
        server.send(403, "text/plain", "Access denied: No active service.");
        return;
    }
    if (server.hasArg("label")) {
        targetLabel = server.arg("label");
    }
    isAcquiring = true;
    timerAlarmEnable(My_timer);
    acquisitionStartMillis = millis();
    server.send(200, "text/plain", "Training STARTED with label: " + targetLabel);
}

void handleTrainStop() {
    isAcquiring = false;
    timerAlarmDisable(My_timer);
    if (!status.hasActiveService) {
        server.send(403, "text/plain", "Access denied: No active service.");
        return;
    }
    server.send(200, "text/plain", "Training STOPPED");
}

void handleTrainStatus() {
    String response = isAcquiring ? "RUNNING - " + targetLabel : "STOPPED";
    server.send(200, "text/plain", response);
}

void handleTrainInfo() {
    String response = "";
    if (isAcquiring) {
        unsigned long currentMillis = millis();
        unsigned long elapsedSeconds = (currentMillis - acquisitionStartMillis) / 1000;
        response = "Time: " + String(elapsedSeconds) + "s | Messages Sent: " + String(msgCounter);
    } else {
        response = "Time: 0s | Messages Sent: " + String(msgCounter);
    }
    server.send(200, "text/plain", response);
}

void handleLabelUpdate() {
    if (server.hasArg("label")) {
        targetLabel = server.arg("label");
    }
    server.send(200, "text/plain", "Label updated: " + targetLabel);
}

void handleSendNow() {
    safeTriggerFastAPI();
    server.send(200, "text/plain", "Data sent to FastAPI");
    msgCounter = 0;
}

void StartWebServerTRAIN() {
    server.on("/", handleRoot);  // optional: also redirects based on login
    server.on("/training", handleTrainingRoot);  //  new endpoint
    server.on("/train_start", handleTrainStart);
    server.on("/train_stop", handleTrainStop);
    server.on("/train_status", handleTrainStatus);
    server.on("/train_info", handleTrainInfo);
    server.on("/calibrate", handleCalibration);  
    server.on("/label_update", handleLabelUpdate);
    server.on("/send_now", handleSendNow);

    server.on("/login", handleLogin);
    server.on("/submit_login", HTTP_POST, handleLoginSubmit);
    server.on("/select_service", HTTP_POST, handleSelectServiceSubmit);

    server.on("/logout", handleLogout);  //  new route

    server.on("/production", handleUnavailableInProduction);  // block wrong mode

    server.begin();
}


//--------------------------------
// GPS Handler
//--------------------------------




//--------------------------------
// Calibration Handler
//--------------------------------

void handleCalibration() {
    CalibrateAccelerometer(500);
    String result = "Calibration complete<br>";
    result += "TiltX: " + String(tiltX * 180.0 / PI, 2) + "°<br>";
    result += "TiltY: " + String(tiltY * 180.0 / PI, 2) + "°";
    server.send(200, "text/html", result);
}





