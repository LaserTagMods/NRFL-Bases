
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

uint8_t node = 1

; // number of node , must be from 1 to 6

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "portchange.h"
RF24 radio(9, 10); // CE, CSN
const uint64_t wAddress[] = {0xB3B4B5B6F0LL, 0xB3B4B5B6F1LL, 0xB3B4B5B6F2LL, 0xB3B4B5B6F3LL, 0xB3B4B5B6F4LL, 0xB3B4B5B6F5LL};
const uint64_t rAddress[] = {0xB3B4B5B6E0LL, 0xB3B4B5B6E1LL, 0xB3B4B5B6E2LL, 0xB3B4B5B6E3LL, 0xB3B4B5B6E4LL, 0xB3B4B5B6E5LL};
uint8_t nS[2];
byte gameStatus[1];
byte gameStatusLen = 1;
byte gotResponse[2];
uint16_t respStatus = 0;
byte pipeNum;

void team1() {
  nS[1] = 1;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(2, HIGH);
}
void team2() {
  nS[1]  = 2;
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(3, HIGH);
}
void team3() {
  nS[1]  = 3;
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
  nS[1] = 0;
  attItr();
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(8, LOW);
}

void setup() {
  nS[0] = node;
  nS[1] = 0;
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  attItr();
  interrupts();

  Serial.begin(115200);
  radio.begin();                  //Starting the Wireless communication
  radio.setDataRate (RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.enableAckPayload();
  radio.openReadingPipe(0, rAddress[node - 1]);
  radio.writeAckPayload(0, &nS, sizeof(nS));
  radio.startListening();
  Serial.print("start ");
}

void loop()
{
  if (radio.available() ) {
    radio.read( &gameStatus, sizeof(gameStatus));
    radio.writeAckPayload(0, &nS, sizeof(nS));
    Serial.print("gameStatus ");
    Serial.println(gameStatus[0]);
    Serial.print("nodeStatus ");
    Serial.print(nS[0]);
    Serial.println(nS[1]);

  }
  if (gameStatus[0] == 222) {
    gameOff();
  }
  if (gameStatus[0] == 111) {
    gameOn();
  }
  //delay(1000);
}
