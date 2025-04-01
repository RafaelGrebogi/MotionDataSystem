#include "json_handler.h"
#include "config.h"  // Include global configuration

char chipIDChar[16] = "";


// Define the number of samples per batch
// #define BATCH_SIZE 10

// Structure to store sensor data
struct SensorData {
    String targetLabel;
    float accelX, accelY, accelZ;
    float gyroX, gyroY, gyroZ;
    String timestamp;
};


// Buffer to store data before sending
SensorData sensorBuffer[BATCH_SIZE];
int dataCount = 0;

// Persistent counter (increment for each message)
int msgCounter = 0;  

String jsonDataReady;

//#################################################################
// Function to collect data
void collectData(String targetLabel, float ax, float ay, float az, float gx, float gy, float gz, String time) {
    if (dataCount < BATCH_SIZE) {
        sensorBuffer[dataCount++] = {targetLabel, ax, ay, az, gx, gy, gz, time };
    }


    if (dataCount >= BATCH_SIZE) {  // Check if buffer is full
        jsonDataReady = prepareJsonPayload();  // Generate JSON

        sendDataToFirebase(jsonDataReady);

        // Serial.println("JSON Ready:");
        // Serial.println(jsonDataReady);  // Print JSON for verification

        dataCount = 0;  // Reset data count for the next batch
    }

}

//#################################################################
// Function to prepare JSON payload
String prepareJsonPayload() {
    if (dataCount == 0) return "{}";  // Return empty JSON if no data
    

    JsonDocument doc;  // Adjust buffer size based on expected data
    doc["message_id"] = generateMessageID();
    doc["device_id"] = chipIDChar;
    doc["timestamp"] = getCurrentTimestamp();
    

    JsonArray samples = doc["samples"].to<JsonArray>();
    for (int i = 0; i < dataCount; i++) {
        JsonObject sample = samples.add<JsonObject>();
        sample["time"] = sensorBuffer[i].timestamp;
        sample["accel_x"] = sensorBuffer[i].accelX;
        sample["accel_y"] = sensorBuffer[i].accelY;
        sample["accel_z"] = sensorBuffer[i].accelZ;
        sample["gyro_x"] = sensorBuffer[i].gyroX;
        sample["gyro_y"] = sensorBuffer[i].gyroY;
        sample["gyro_z"] = sensorBuffer[i].gyroZ;
        // Include target in each sample
        if(TRAINING_MODE){
            sample["target"] = sensorBuffer[i].targetLabel;  // Use the selected target ("Normal Walk" or "Limping")
        }else{
            sample["target"] = "";  // Empty when not in training mode
        }

    }

    String jsonData;
    serializeJson(doc, jsonData);
    dataCount = 0;  //  Clear buffer after JSON creation

    return jsonData;
}

//#################################################################
// Extract chip ID
char* ESP32_ID_Extraction(){
    char chipIDChar1[10];   //print High2 bytes
    char chipIDChar2[10];   //print Low 4bytes.
    static char chipIDCharFull[16];    // Full ID char

    // Chip ID extraction to String
    uint64_t chipid = ESP.getEfuseMac(); //Get the CHIP MAC ADRESS
    Serial.printf("ESP32 ID = %04X", (uint16_t)(chipid >> 32));//print High2 bytes
    Serial.printf("%08X\n", (uint32_t)chipid); //print Low 4bytes.
  
    sprintf(chipIDChar1, "%04X", (uint16_t)(chipid >> 32));
    sprintf(chipIDChar2, "%08X", (uint32_t)chipid);
    strcpy(chipIDCharFull,chipIDChar1);
    strcat(chipIDCharFull,chipIDChar2);
    // Serial.println(chipIDChar); // holding chip ID characters
    //Serial.println(chipIDChar2); //
    return chipIDCharFull;
  }



//#################################################################
// Function to generate a unique message ID
String generateMessageID() {

    // char msgID[50];
    msgCounter++;  //  Increment counter for uniqueness
    // return String(chipIDChar) + "_" + getMsgTimestamp() + "_" + String(msgCounter);

    sprintf(msgID, "%s_%s_%d", chipIDChar, getMsgTimestamp().c_str(), msgCounter);

    return String(msgID);
}

//#################################################################
String getMsgTimestamp() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Unknown";  // ✅ Return default if time fetch fails
    }
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y%m%d%H%M%S", &timeinfo);  // ✅ Format: YYYYMMDDHHMMSS
    return String(timeStr);
}


//#################################################################
String getCurrentTimestamp(){
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Unknown";  // Return default if time fetch fails
    }
    char timeStr[30];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
    return String(timeStr);
}


//#################################################################
// SERVICE FUNCTION
//#################################################################
void sendJsonToPC() {
    String jsonData = prepareJsonPayload();
    Serial.println("Sending JSON to PC:");
    Serial.println(jsonData);
}