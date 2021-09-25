#include <WioLTEforArduino.h>
#include <stdio.h>

#define APN               "apn"
#define USERNAME          "username"
#define PASSWORD          "passward"

#define WEBHOOK_URL       "webhook_urrl"

#define PRESSURE_SENSOR_1  (WioLTE::A4)
#define PRESSURE_SENSOR_2  (WioLTE::A6)

#define INTERVAL          (10000)

const int sensorPin = A4; //pin A4 to read analog input

//Variables:
int value; //save analog value

WioLTE Wio;
  
void setup() {
  delay(200);

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(500);

  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### Could NOT Turn ON, ERROR! ###");
    return;
  }

  SerialUSB.println("### Connecting to \""APN"\".");
  if (!Wio.Activate(APN, USERNAME, PASSWORD)) {
    SerialUSB.println("### Could NOT Activate, ERROR! ###");
    return;
  }

  // set sensor pins as INPUT_ANALOG
  pinMode(PRESSURE_SENSOR_1, INPUT_ANALOG);
  pinMode(PRESSURE_SENSOR_2, INPUT_ANALOG);

  SerialUSB.println("### Setup completed.");


  // Start measuring weight
  
  for (true) {
    // Read and save analog values from pressure sensors
    int weight_1 = analogRead(PRESSURE_SENSOR_1);
    int weight_2 = analogRead(PRESSURE_SENSOR_2);

    // Initialize variables of the total taken weights
    int taken_weight_1;
    int taken_weight_2;
  
    SerialUSB.print("Value1: ");
    SerialUSB.print(weight_1); 
    SerialUSB.print(" Value2: ");
    SerialUSB.println(weight_2); 

    // Initial evaluation of the weighs
    if (weight_1 < 20 || weight_2 < 20) {
      delay(2000);
      continue
    }
  
    delay(500);

    // save the first recorded weight in a day
    int initial_weight_1 = weight_1;
    int initial_weight_2 = weight_2;
  
    delay(500);

    // generate a sentence presenting data
    char data[] = "Value1: " + initial_weight_1 + "Value2: " + initial_weight_2;
    int status;
  
    SerialUSB.println("### Post.");
    SerialUSB.print("Post:");
    SerialUSB.print(data);
    SerialUSB.println("");
    if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
      SerialUSB.println("###Webhook ERROR! ###");
      goto err_1;
    }
    SerialUSB.print("Status:");
    SerialUSB.println(status);
err_1:
  SerialUSB.println("### Wait.");
  delay(INTERVAL);
    
  }
}

void loop() {
  weight_1 = analogRead(PRESSURE_SENSOR_1);
  weight_2 = analogRead(PRESSURE_SENSOR_2);
  
  SerialUSB.print("Value1: ");
  SerialUSB.print(weight_1); 
  SerialUSB.print(" Value2: ");
  SerialUSB.println(weight_2); 

  // Initial evaluation of the weighs
  if (weight_1 > 20 && weight_2 > 20) {
    delay(2000);
    continue
  }

  for (true) {
    delay(2000);
    if (weight_1 < 20 || weight_2 < 20) {
      continue
    }
  }
  
  delay(500);
  
  char data[] = "Value1: " + initial_weight_1 + "Value2: " + initial_weight_2;
  int status;
  
  SerialUSB.println("### Post.");
  SerialUSB.print("Post:");
  SerialUSB.print(data);
  SerialUSB.println("");
  if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
    SerialUSB.println("###Webhook ERROR! ###");
    goto err_2;
  }
  SerialUSB.print("Status:");
  SerialUSB.println(status);

  taken_weight_1 += initial_weight1 - weight_1;
  taken_weight_2 += initial_weight2 - weight_2;
  
  weight_1 = analogRead(PRESSURE_SENSOR_1);
  weight_2 = analogRead(PRESSURE_SENSOR_2);
  
  err_2:
    SerialUSB.println("### Wait.");
    delay(INTERVAL);
}
