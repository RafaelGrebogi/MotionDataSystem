#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"

// Define SPI pins for SD Card (change if needed)
#define SD_MISO 19
#define SD_MOSI 23
#define SD_SCK 18
#define SD_CS 5  // Change this if your SD module uses a different CS pin

SPIClass spi = SPIClass(VSPI);

// Function Prototypes
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void StartSDCard();
void logSDCard();

void setup() {
    Serial.begin(115200);
    delay(1000);
    StartSDCard();
}

void loop() {
    logSDCard();
    delay(2000);
}

// Initialize SD Card Module
void StartSDCard() {
    Serial.println("Initializing SD card...");

    spi.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

    if (!SD.begin(SD_CS, spi)) {
        Serial.println("❌ SD Card initialization failed!");
        return;
    }

    Serial.println("✅ SD Card initialized successfully!");

    if (!SD.exists("/data.txt")) {
        Serial.println("Creating data file...");
        writeFile(SD, "/data.txt", "CAN NETWORK DATA \r\n");
    } else {
        Serial.println("File already exists.");
    }
}

// Write Data to SD Card
void writeFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Writing to file: %s\n", path);
    fs::File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("❌ Failed to open file for writing");
        return;
    }
    file.print(message);
    file.close();
    Serial.println("✅ File written successfully!");
}

// Append Data to SD Card
void appendFile(fs::FS &fs, const char *path, const char *message) {
    Serial.printf("Appending to file: %s\n", path);
    fs::File file = fs.open(path, FILE_APPEND);
    if (!file) {
        Serial.println("❌ Failed to open file for appending");
        return;
    }
    file.print(message);
    file.close();
    Serial.println("✅ Message appended!");
}

// Log Data to SD Card
void logSDCard() {
    char msgString[50] = "This is a test";  // Increased buffer size
    strcat(msgString, "\r\n");
    Serial.println(msgString);
    appendFile(SD, "/data.txt", msgString);
}