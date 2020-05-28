uint8_t node = 0; // no change
// you should set number of nodes including coordinator node, but not more than seven
uint8_t number_of_nodes = 2; 
uint16_t delay_node = 50; //delay beetwen asking each node, ms


#include <SPI.h>
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include "portchange.h"
RF24 radio(9, 10); // CE, CSN

const uint64_t rAddress[] = {0xB3B4B5B6F0LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6F2LL, 0xB3B4B5B6F3LL, 0xB3B4B5B6F4LL, 0xB3B4B5B6F5LL};
const uint64_t wAddress[] = {0xB3B4B5B6E0LL, 0xB3B4B5B6E1LL, 0xB3B4B5B6E2LL, 0xB3B4B5B6E3LL, 0xB3B4B5B6E4LL, 0xB3B4B5B6E5LL};
uint8_t nS[2];
uint8_t all[7] = {0, 0, 0, 0, 0, 0, 0};
uint8_t gameStatus[1] = {10};
uint8_t i = 1;
boolean flagOff = false;
boolean flagOn = false;
void team1() {
  all[0] = 1;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(2, HIGH);
}
void team2() {
  all[0] = 2;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
}
void team3() {
  all[0] = 3;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(4, HIGH);
}

void attItr() {
  attachInterruptEx(5, team1, PC_FALLING);
  attachInterruptEx(6, team2, PC_FALLING);
  attachInterruptEx(7, team3, PC_FALLING);
}

void detItr() {
  detachInterruptEx(5);
  detachInterruptEx(6);
  detachInterruptEx(7);
}

void gameOff() {
  detItr();
  digitalWrite(8, HIGH);
}
void gameOn() {
  all[0] = 0;
  attItr();
  digitalWrite(8, LOW);
}


void setup()
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(8, HIGH);
  attItr();
  interrupts();
  Serial.begin(115200);  //start serial to communication
  radio.begin();  //Start the nRF24 module
  radio.setDataRate (RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);  // "short range setting" - increase if you want more range AND have a good power supply
  radio.enableAckPayload();
  radio.setRetries(3, 15); // delay, count
  gameStatus[0] = 111;
}


void loop()
{
  radio.openWritingPipe(wAddress[i - 1]);
  radio.write(&gameStatus, sizeof(gameStatus));
  if ( radio.available() ) {
    radio.read(&nS, sizeof(nS));
  }
  if (nS[0] == i) all[nS[0]] = nS[1];
  Serial.print("write: ");
  Serial.print(i);
  Serial.print(" / ");
  Serial.println(gameStatus[0]);
  Serial.print("received: ");
  Serial.print(i);
  Serial.print(" / ");
  Serial.print(nS[0]);
  Serial.print(" / ");
  Serial.println(nS[1]);
  for (int j = 0; j < number_of_nodes; j++) {
    Serial.print(all[j]);
    Serial.print(" / ");
  }
  Serial.println("");

  nS[0] = 0;
  nS[1] = 0;
  delay(delay_node);
  i++;
  if (i >= number_of_nodes) i = 1;

  if (all[0] != 0 && gameStatus[0] == 0) {
    flagOff = true;
    for (int j = 0; j < number_of_nodes; j++) {
      if (all[0] != all[j]) flagOff = false;
    }
    if (flagOff) {
      gameOff();
      gameStatus[0] = 222;
    }
  }

  if (all[0] == 0 && gameStatus[0] == 111) {
    flagOn = true;
    for (int j = 0; j < number_of_nodes; j++) {
      if (all[0] != all[j]) flagOn = false;
    }
    if (flagOn) {
      gameOn();
      gameStatus[0] = 0;
    }
  }
}
