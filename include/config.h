#ifndef CONFIG_H
#define CONFIG_H

#define TRAINING_MODE_PIN  12 
#define TESTING_MODE_PIN  13 

// #define FAKE_ID


extern bool TRAINING_MODE;  // Operation mode: true -> Training mode | false -> Production mode

struct UserStatus {
  String userId;
  bool hasActiveService;
  bool success;  // true if FastAPI call and parsing succeeded
};

enum OperationMode { TRAINING, TESTING, PRODUCTION };
extern OperationMode currentMode;
extern OperationMode lastMode;


#endif // CONFIG_H