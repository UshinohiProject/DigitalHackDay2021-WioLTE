#include <WioLTEforArduino.h>
#include <stdio.h>
#include <ArduinoJson.h>

#define APN               "apn"
#define USERNAME          "username"
#define PASSWORD          "passward"

#define WEBHOOK_URL       "webhook_urrl"

#define PRESSURE_SENSOR_1_CLK  (WioLTE::D20)
#define PRESSURE_SENSOR_1_DAT  (WioLTE::D19)
#define PRESSURE_SENSOR_2_CLK  (WioLTE::D38)
#define PRESSURE_SENSOR_2_DAT  (WioLTE::D39)

#define INTERVAL          (10000)

#include <Wire.h>
#include "rgb_lcd.h"

const int sensorPin = A4; //pin A4 to read analog input

// Initialize variables of the total taken weights
int taken_weight_1 = 0;
int taken_weight_2 = 0;

long pre_initial_weight_1 = 0;
long pre_initial_weight_2 = 0;

char pin_num_1 = 20;
char pin_num_2 = 38;

unsigned long time_data = 0;

WioLTE Wio;

rgb_lcd lcd;
  
void setup() {

  SerialUSB.println("");
  SerialUSB.println("--- START ---------------------------------------------------");
  
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(500);
  
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyGrove(true);
  delay(500);
  
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("  Developed by");
  lcd.setCursor(0, 1);
  lcd.print("Ushinohi Project");
  
  delay(2000);
  
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("Hi, I'm Kenko!");
  lcd.setCursor(0, 1);
  lcd.print("Nice to meet you");

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

  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyGrove(true);
  delay(500);
 
  // set sensor pins as INPUT
  pinMode(PRESSURE_SENSOR_1_CLK, OUTPUT);
  pinMode(PRESSURE_SENSOR_1_DAT, INPUT);
  pinMode(PRESSURE_SENSOR_2_CLK, OUTPUT);
  pinMode(PRESSURE_SENSOR_2_DAT, INPUT);

  SerialUSB.println("### Setup completed.");
}

void loop() {
    delay(800);
    int initial_weight_1;
    int initial_weight_2;
  
  // Start measuring weight

  // Initialize weight sensors
    pre_initial_weight_1 = GetWeights(pin_num_1);
    pre_initial_weight_2 = GetWeights(pin_num_2);

  while (true) {
    long recorded_weight_1 = 0;
    long recorded_weight_2 = 0;
    // Read and save analog values from pressure sensors
    recorded_weight_1 = GetWeights(pin_num_1);
    recorded_weight_2 = GetWeights(pin_num_2);
    
    
    long weight_1;
    long weight_2;
    weight_1 = ((recorded_weight_1 - pre_initial_weight_1) / 1000) *2.2;
    weight_2 = ((recorded_weight_2 - pre_initial_weight_2) / 1000) *2.2;
  
    SerialUSB.print("Value1: ");
    SerialUSB.print(weight_1); 
    SerialUSB.print(" Value2: ");
    SerialUSB.println(weight_2);
    DisplayWeights(weight_1, weight_2);

    // Initial evaluation of the weighs
    if (weight_1 > 20 && weight_2 > 20) {
  
      delay(500);

      // save the first recorded weight in a day
      initial_weight_1 = weight_1;
      initial_weight_2 = weight_2;
  
      delay(500);

      // generate a json presenting data
      time_data = millis();  
      const int capacity_1 = JSON_OBJECT_SIZE(3);
      StaticJsonDocument<capacity_1> json_request;
      json_request["timestamp"] = time_data;
      json_request["salt"] = weight_1;
      json_request["suger"] = weight_2;
      char buffer[255];
      serializeJson(json_request, buffer, sizeof(buffer));

  
      int status;
  
      SerialUSB.println("### Post.");
      SerialUSB.print("Post:");
      SerialUSB.print(buffer);
      SerialUSB.println("");
      if (!Wio.HttpPost(WEBHOOK_URL, buffer, &status)) {
        SerialUSB.println("###Webhook ERROR! ###");
        goto err_1;
      }
      SerialUSB.print("Status:");
      SerialUSB.println(status);
      err_1:
        SerialUSB.println("### Wait.");
        delay(INTERVAL);
      
      break;
    }
    
    delay(2000);
  }
    

  while (true) {
    long recorded_weight_1 = 0;
    long recorded_weight_2 = 0;
    // Read and save analog values from pressure sensors 
    recorded_weight_1 = GetWeights(pin_num_1);
    recorded_weight_2 = GetWeights(pin_num_2);
    
    long weight_1;
    long weight_2;
    weight_1 = ((recorded_weight_1 - pre_initial_weight_1) / 1000) *2.2;
    weight_2 = ((recorded_weight_2 - pre_initial_weight_2) / 1000) *2.2;
  
    SerialUSB.print("Value1: ");
    SerialUSB.print(weight_1); 
    SerialUSB.print(" Value2: ");
    SerialUSB.println(weight_2); 
    DisplayWeights(weight_1, weight_2);

    // Initial evaluation of the weighs
    if (weight_1 > 20 && weight_2 > 20) {
      delay(2000);
      continue;
    } else {

      while (true) {
        delay(2000);
        long recorded_weight_1 = 0;
        long recorded_weight_2 = 0;
        // Read and save analog values from pressure sensors
        recorded_weight_1 = GetWeights(pin_num_1);
        recorded_weight_2 = GetWeights(pin_num_2);
    
        long weight_1;
        long weight_2;
        weight_1 = ((recorded_weight_1 - pre_initial_weight_1) / 1000) *2.2;
        weight_2 = ((recorded_weight_2 - pre_initial_weight_2) / 1000) *2.2;
  
        SerialUSB.print("Value1: ");
        SerialUSB.print(weight_1); 
        SerialUSB.print(" Value2: ");
        SerialUSB.println(weight_2); 
        DisplayWeights(weight_1, weight_2);
        
        if (weight_1 < 20 || weight_2 < 20) {
          continue;
        } else {

          delay(500);

          time_data = millis();
          const int capacity_2 = JSON_OBJECT_SIZE(3);
          StaticJsonDocument<capacity_2> json_request;
          json_request["timestamp"] = time_data;
          json_request["salt"] = weight_1;
          json_request["suger"] = weight_2;
          char buffer[255];
          serializeJson(json_request, buffer, sizeof(buffer));

      
          int status;
  
          SerialUSB.println("### Post.");
          SerialUSB.print("Post:");
          SerialUSB.print(buffer);
          SerialUSB.println("");
          if (!Wio.HttpPost(WEBHOOK_URL, buffer, &status)) {
            SerialUSB.println("###Webhook ERROR! ###");
            goto err_2;
          }
          SerialUSB.print("Status:");
          SerialUSB.println(status);

          taken_weight_1 += initial_weight_1 - weight_1;
          taken_weight_2 += initial_weight_2 - weight_2;
  
          err_2:
            SerialUSB.println("### Wait.");
            delay(INTERVAL);
          }
        }
      }
   }
}

long GetWeights(char pin_num){
  if (pin_num == 20) {
    long measured_weight_1 = 0;
    // Read and save analog values from pressure sensors   
    for (char i = 0; i < 24; i++) {
    digitalWrite(PRESSURE_SENSOR_1_CLK, 1);
    delayMicroseconds(1);
    digitalWrite(PRESSURE_SENSOR_1_CLK, 0);
    delayMicroseconds(1);
      measured_weight_1 = (measured_weight_1 << 1) | (digitalRead(PRESSURE_SENSOR_1_DAT));
    }
    measured_weight_1 = measured_weight_1 ^ 0x800000;
    return measured_weight_1;
  } else {
    long measured_weight_2 = 0;
    // Read and save analog values from pressure sensors   
    for (char i = 0; i < 24; i++) {
    digitalWrite(PRESSURE_SENSOR_2_CLK, 1);
    delayMicroseconds(1);
    digitalWrite(PRESSURE_SENSOR_2_CLK, 0);
    delayMicroseconds(1);
      measured_weight_2 = (measured_weight_2 << 1) | (digitalRead(PRESSURE_SENSOR_2_DAT));
    }
    measured_weight_2 = measured_weight_2 ^ 0x800000;
    return measured_weight_2;
  }
}

void DisplayWeights(long weight_1, long weight_2){
  char weight_1_digit = GetDigit(weight_1);
  char weight_2_digit = GetDigit(weight_2);
  
  // Print a message to the LCD.
  lcd.clear();
  lcd.print("  salt    suger ");
  lcd.setCursor(0, 1);
  for (char i = 0; i < 4 - weight_1_digit; i++){
    lcd.print(" ");
  }
  lcd.print(weight_1);
  lcd.print(" g  ");
  for (char i = 0; i < 4 - weight_2_digit; i++){
    lcd.print(" ");
  }
  lcd.print(weight_2);
  lcd.print(" g");
  
}

char GetDigit(long num){
   if (num == 0){
    return 1; 
   }else if (num < 0){
    return 4;
   } else {
    return log10(num)+1;    
   }
}
