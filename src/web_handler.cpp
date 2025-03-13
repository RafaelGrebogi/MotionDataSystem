#include "web_handler.h"

// hw_timer_t *My_timer = NULL;

WebServer server(80);  // Creates a web server that listens on port 80 (default for HTTP)
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
    timerAlarmEnable(My_timer); //Just Enabl
    server.send(200, "text/plain", "Acquisition STARTED");
}

void handleStop() {
    isAcquiring = false;
    timerAlarmDisable(My_timer);
    server.send(200, "text/plain", "Acquisition STOPPED");
}

void handleStatus() {
    server.send(200, "text/plain", isAcquiring ? "RUNNING" : "STOPPED");  // Placeholder, will update later
}

void StartWebServer() {
    server.on("/", handleRoot);
    server.on("/start", handleStart);
    server.on("/stop", handleStop);
    server.on("/status", handleStatus);

    server.begin();
}


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