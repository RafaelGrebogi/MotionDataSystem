#ifndef CONFIG_H
#define CONFIG_H

#define TRAINING_MODE_PIN  12 
#define TESTING_MODE_PIN  13 

// #define FAKE_ID

// Chip ID number
extern char chipIDChar[16];


#define MAX_SERVICES 10

extern bool TRAINING_MODE;  // Operation mode: true -> Training mode | false -> Production mode

struct ServiceData {
  String id;
  String companyName;
};  
// extern ServiceData services;

struct UserStatus {
  bool success;
  String userId;
  bool hasActiveService;
  String selectedServiceId;
  ServiceData services[MAX_SERVICES];
  int serviceCount = 0;
};  // true if FastAPI call and parsing succeeded
extern UserStatus status;




enum OperationMode { TRAINING, TESTING, PRODUCTION };
extern OperationMode currentMode;
extern OperationMode lastMode;


#endif // CONFIG_H