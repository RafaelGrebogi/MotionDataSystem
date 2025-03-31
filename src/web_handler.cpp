#include "web_handler.h"



WebServer server(80);  // Creates a web server that listens on port 80 (default for HTTP)

extern hw_timer_t *My_timer;

// 'Training mode' variables 
extern int msgCounter;
unsigned long acquisitionStartMillis = 0;
String targetLabel = "Normal Walk";  // Default target label

bool isAcquiring = false;


void handleRoot() {
    String html = "<html><body>";
    html += "<h2>ESP32 Data Acquisition</h2>";
    html += "<p>Status: <span id='status'>STOPPED</span></p>";
    html += "<button onclick=\"fetch('/start')\">START</button>";
    html += "<button onclick=\"fetch('/stop')\">STOP</button>";
    html += "<script>setInterval(()=>fetch('/status').then(r=>r.text()).then(t=>document.getElementById('status').innerText=t),2000);</script>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleStart() {
    isAcquiring = true;
    timerAlarmEnable(My_timer);
    server.send(200, "text/plain", "Acquisition STARTED");
}

void handleStop() {
    isAcquiring = false;
    stopFastAPIConnection();
    timerAlarmDisable(My_timer);
    server.send(200, "text/plain", "Acquisition STOPPED");
}

void handleStatus() {
    server.send(200, "text/plain", isAcquiring ? "RUNNING" : "STOPPED");
}

void StartWebServer() {
    server.on("/", handleRoot);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);
    server.on("/status", handleStatus);

    server.begin();
}

//######################################
// Web Server for Training Data Acquisition
//######################################

void handleTrainingRoot() {
    String html = "<html><body>";
    html += "<h2>ESP32 Training Data Acquisition</h2>";
    html += "<p>Status: <span id='train_status'>STOPPED</span></p>";
    html += "<p>Acquisition Info: <span id='info'>Time: 0s | Messages Sent: 0</span></p>";
    
    // Radio buttons for target selection
    html += "<p>Select Label:</p>";
    html += "<input type='radio' name='label' value='Normal Walk' checked> Normal Walk<br>";
    html += "<input type='radio' name='label' value='Limping'> Limping<br>";
    
    html += "<button onclick=\"startTraining()\">START</button>";
    html += "<button onclick=\"fetch('/train_stop')\">STOP</button>";

    // JavaScript for handling Start button with selected label
    html += "<script>";
    html += "function startTraining() {";
    html += "  let label = document.querySelector('input[name=label]:checked').value;";
    html += "  fetch('/train_start?label=' + encodeURIComponent(label));";
    html += "}";

    // Single interval that updates both status and info every second
    html += "setInterval(()=>{";
    html += "fetch('/train_status').then(r=>r.text()).then(t=>document.getElementById('train_status').innerText=t);";
    html += "fetch('/train_info').then(r=>r.text()).then(t=>document.getElementById('info').innerText=t);";
    html += "}, 1000);";

    html += "</script>";
    html += "</body></html>";

    server.send(200, "text/html", html);
}

//#####################
void handleTrainStart() {
    if (server.hasArg("label")) {
        targetLabel = server.arg("label");
    }
    isAcquiring = true;
    timerAlarmEnable(My_timer);
    acquisitionStartMillis = millis();
    server.send(200, "text/plain", "Training STARTED with label: " + targetLabel);
}
//#####################
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
//#####################
void handleTrainStop() {
    isAcquiring = false;
    timerAlarmDisable(My_timer);
    stopFastAPIConnection();
    server.send(200, "text/plain", "Training STOPPED");
}
//#####################
void handleTrainStatus() {
    String response = isAcquiring ? "RUNNING - " + targetLabel : "STOPPED";
    server.send(200, "text/plain", response);
}
//#####################
void StartWebServerTRAIN() {
    server.on("/", handleTrainingRoot);
    server.on("/train_start", handleTrainStart);
    server.on("/train_stop", handleTrainStop);
    server.on("/train_status", handleTrainStatus);
    server.on("/train_info", handleTrainInfo);
    server.begin();
}
//#####################



// #include "web_handler.h"

// // hw_timer_t *My_timer = NULL;
// extern hw_timer_t *My_timer;

// WebServer server(80);  // Creates a web server that listens on port 80 (default for HTTP)
// bool isAcquiring = false; 



// void handleRoot() {
//     String html = "<html><body>";
//     html += "<h2>ESP32 Data Acquisition</h2>";
//     html += "<p>Status: <span id='status'>STOPPED</span></p>";
//     html += "<button onclick=\"fetch('/start')\">START</button>";
//     html += "<button onclick=\"fetch('/stop')\">STOP</button>";
//     html += "<script>setInterval(()=>fetch('/status').then(r=>r.text()).then(t=>document.getElementById('status').innerText=t),2000);</script>";
//     html += "</body></html>";
//     server.send(200, "text/html", html);
// }

// void handleStart() {
//     isAcquiring = true;
//     timerAlarmEnable(My_timer); //Just Enabl
//     server.send(200, "text/plain", "Acquisition STARTED");
// }

// void handleStop() {
//     isAcquiring = false;
//     timerAlarmDisable(My_timer);
//     server.send(200, "text/plain", "Acquisition STOPPED");
// }

// void handleStatus() {
//     server.send(200, "text/plain", isAcquiring ? "RUNNING" : "STOPPED");  // Placeholder, will update later
// }

// void StartWebServer() {
//     server.on("/", handleRoot);
//     server.on("/start", handleStart);
//     server.on("/stop", handleStop);
//     server.on("/status", handleStatus);

//     server.begin();
// }

// //######################################
// // Web Server to acquire training subset
// //######################################

// void StartWebServerTRAIN() {
//     // server.on("/", handleRoot);
//     // server.on("/start", handleStart);
//     // server.on("/stop", handleStop);
//     // server.on("/status", handleStatus);

//     server.begin();
// }






//###########################
// Web Server test function

void test_handleStart() {
    isAcquiring = true;
    // timerAlarmEnable(My_timer); //Just Enabl
    server.send(200, "text/plain", "Acquisition STARTED");
}

void test_handleStop() {
    isAcquiring = false;
    // timerAlarmDisable(My_timer);
    server.send(200, "text/plain", "Acquisition STOPPED");
}