#ifndef CONFIG_H
#define CONFIG_H

#define TRAINING_MODE_PIN  12  

extern bool TRAINING_MODE;  // Operation mode: true -> Training mode | false -> Production mode

enum OperationMode { TRAINING, TESTING, PRODUCTION };
extern OperationMode currentMode;

#endif // CONFIG_H