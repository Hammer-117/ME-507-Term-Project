#include <Wire.h>

#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

RF24 radio(7, 8); // CE, CSN

const byte address[5] = {0x30, 0x30, 0x30, 0x30, 0x31};

#define SLAVE_ADDRESS 0x69

int i = 0;

char transmissionbuf[13] = "0000m0000m000";

void sendData() {
  /*for (i=0; i < 13; i++){
    Wire.write(transmissionbuf[i]);  // Send the character buffer when requested
  }//*/

  Wire.write(transmissionbuf[10]);
  Wire.write(transmissionbuf[11]);
  Wire.write(transmissionbuf[0]);
  Wire.write(transmissionbuf[1]);//*/
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();

  Wire.begin(SLAVE_ADDRESS);  // Initialize I2C communication as slave
  Wire.onRequest(sendData);   // Register the sendData function to respond to I2C requests
}

void loop() {
  // put your main code here, to run repeatedly:
  if (radio.available()) {
    radio.read(&transmissionbuf, sizeof(transmissionbuf));
    
    Serial.println(transmissionbuf);
  }

}

