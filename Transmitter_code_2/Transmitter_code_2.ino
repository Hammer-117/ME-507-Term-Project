#include <RF24.h>
#include <RF24_config.h>
#include <nRF24L01.h>
#include <printf.h>

RF24 radio(7, 8); //CE, CSN

const byte address[5] = {0x30, 0x30, 0x30, 0x30, 0x31};

#define deadman 3
#define FuckingBitch A3
#define joyx A1
#define joyy A2

char angle_buf[8] = {0};
char dist_buf[8] = {0};

int temp = 0;
uint8_t temp8 = 0;

//int16_t angle = 0;
//uint16_t dist = 0;
//uint16_t 4dist = 0;

char transmissionbuf[13] = "0000,0000,110";

bool done = 0;
bool controller = false;

void setup(){
  Serial.begin(9600);
  Serial.setTimeout(1);

  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  //radio.setChannel(15);
  radio.stopListening();
  radio.openWritingPipe(address);
  //Serial.println(radio.getDataRate());
  //radio.setDataRate(RF24_2MBPS);

  pinMode(deadman, INPUT_PULLUP);
  pinMode(FuckingBitch, INPUT_PULLUP);
  pinMode(joyx, INPUT);
  pinMode(joyy, INPUT);
}

void loop(){

  getserialdata();
  if (controller == false){
    fullsend();
  }
  else {
    halfsend();
  }
  delay(25);

}

void getserialdata(){

  if(Serial.available() == 8){
    angle_buf[0] = Serial.read();
    angle_buf[1] = Serial.read();
    angle_buf[2] = Serial.read();
    angle_buf[3] = Serial.read();

    //angle = strtol(angle_buf, NULL, 16);

    dist_buf[0] = Serial.read();
    dist_buf[1] = Serial.read();
    dist_buf[2] = Serial.read();
    dist_buf[3] = Serial.read();

    //4dist = strtol(dist_buf, NULL, 16);

    //dist = 4dist/4;

    done = true;
  }
  /*if (done == 1){
    Serial.println(angle);
    Serial.println(dist);
    //delay(250);
  }
  else{
    //Serial.println(Serial.available());
    done = false;
  }//*/

  for (int i = 0; i < 4; i++) {
  transmissionbuf[i] = angle_buf[i];
  }

  transmissionbuf[4] = ",";

  for (int i = 0; i < 4; i++) {
  transmissionbuf[i+5] = dist_buf[i];
  }

  if (digitalRead(deadman) == false){
    transmissionbuf[10] = 0x01;
  }
  else {
    transmissionbuf[10] = 0x00;
  }

  if (digitalRead(FuckingBitch) == false) {
    transmissionbuf[11] = 0x01;
    controller = true;
  }
  else {
    transmissionbuf[11] = 0x00;
    controller = false;
  }

}

void fullsend() {
  //Serial.println("in full send");

  // send data as received

  radio.write(transmissionbuf, 12);

  done = false;

}

void halfsend() {
  //Serial.println("in half send");

  temp = analogRead(joyx);
  temp8 = temp/4;

  transmissionbuf[0] = temp8;

  temp = analogRead(joyy);
  temp8 = temp/4;
  Serial.println(temp8);

  transmissionbuf[1] = temp8;

  radio.write(transmissionbuf, 12);
  //Serial.print(sizeof(transmissionbuf));
  //Serial.println(transmissionbuf);

}