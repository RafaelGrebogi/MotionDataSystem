#ifndef CONFIG_H
#define CONFIG_H

#define TRAINING_MODE_PIN  12 
#define TESTING_MODE_PIN  13 

// #define FAKE_ID


extern bool TRAINING_MODE;  // Operation mode: true -> Training mode | false -> Production mode



enum OperationMode { TRAINING, TESTING, PRODUCTION };
extern OperationMode currentMode;

#endif // CONFIG_H