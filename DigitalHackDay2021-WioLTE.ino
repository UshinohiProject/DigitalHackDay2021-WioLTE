#include <WioLTEforArduino.h>
#include <stdio.h>

#define APN               "apn"
#define USERNAME          "username"
#define PASSWORD          "passward"

#define WEBHOOK_URL       "webhook_urrl"

#define INTERVAL          (10000)

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

  SerialUSB.println("### Setup completed.");
}

void loop() {
  char data[] = "Let's go, Hack Day!";
  int status;
  
  SerialUSB.println("### Post.");
  SerialUSB.print("Post:");
  SerialUSB.print(data);
  SerialUSB.println("");
  if (!Wio.HttpPost(WEBHOOK_URL, data, &status)) {
    SerialUSB.println("###Webhook ERROR! ###");
    goto err;
  }
  SerialUSB.print("Status:");
  SerialUSB.println(status);
  
err:
  SerialUSB.println("### Wait.");
  delay(INTERVAL);
}
