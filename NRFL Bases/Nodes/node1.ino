
#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

uint8_t node = 1; // number of node , must be from 1 to 6

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

// Set up for IR Recieving and detection of brx tag protocol
const byte IR_Sensor_Pin = 9; // Pin used for IR receiver
int B[4]; // bullet type bits of ir
int P[6]; // player id bits of ir
int T[2]; // team id bits of ir
int D[8]; // damage amount of ir
int C[1]; // is a critical hit shot? 
int U[2]; // power type of shot
int Z[3]; // parity bit for confirmin brx protocol
int team = 0; // used for team identification for last shot received


//******************************************************************************************************************************************************************************************
void teamrecognition() {
      // check if the IR is from Red team
      if (T[0] < 750 && T[1] < 750) {
      // sets the current team as red
      team = 1;
      Serial.print("team = Red = ");
      Serial.println(team);
      team1();
      PC_FALLING;
      }
      // check if the IR is from blue team 
      if (T[0] < 750 && T[1] > 750) {
      // sets the current team as blue
      team = 2;
      Serial.print("team = Blue = ");
      Serial.println(team);
      team2();
      PC_FALLING;
      }
      // check if the IR is from green team 
      if (T[0] > 750 && T[1] > 750) {
      // sets the current team as green
      team = 3;
      Serial.print("team = Green = ");
      Serial.println(team);
      team3();
      PC_FALLING;
      }
      if (T[0] > 750 && T[1] < 750) {
      // sets the current team as red
      team = 4;
      Serial.print("team = Yellow = ");
      Serial.println(team);
      }
}
//******************************************************************************************************************************************************************************************

//******************************************************************************************************************************************************************************************
// This procedure uses the preset IR_Sensor_Pin to determine if an ir received is BRX, if so it records the protocol received
void receiveBRXir() {
  // makes the action below happen as it cycles through the 25 bits as was delcared above
  for (byte x = 0; x < 4; x++) B[x]=0;
  for (byte x = 0; x < 6; x++) P[x]=0;
  for (byte x = 0; x < 2; x++) T[x]=0;
  for (byte x = 0; x < 8; x++) D[x]=0;
  for (byte x = 0; x < 1; x++) C[x]=0;
  for (byte x = 0; x < 2; x++) U[x]=0;
  for (byte x = 0; x < 2; x++) Z[x]=0;
  // checks for a 2 millisecond sync pulse signal with a tollerance of 500 microsecons
  if (pulseIn(IR_Sensor_Pin, LOW, 150000) > 1500) { // this checks to make sure the right sync bit of ir is a brx type
      // stores each pulse or bit, individually
      B[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // B1
      B[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // B2
      B[2] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // B3
      B[3] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // B4
      P[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P1
      P[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P2
      P[2] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P3
      P[3] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P4
      P[4] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P5
      P[5] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // P6
      T[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // T1
      T[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // T2
      D[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D1
      D[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D2
      D[2] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D3
      D[3] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D4
      D[4] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D5
      D[5] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D6
      D[6] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D7
      D[7] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // D8
      C[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // C1
      U[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // ?1
      U[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // ?2
      Z[0] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // Z1
      Z[1] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // Z2
      Z[2] = (pulseIn(IR_Sensor_Pin, LOW, 5000)); // Z3 check for incorrect IR
      if (Z[1] != Z[0] && Z[2] < 250) { // this is to make sure we got a legit brx tag since we know there is no Z3 and the parity bits are never identical
      teamrecognition(); // creates grenade tag with player bits received
      }
      }
}
//******************************************************************************************************************************************************************************************


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

// replaced with IR reception code
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
  receiveBRXir();
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
  pinMode(IR_Sensor_Pin, INPUT); // IR Sensor Pin
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, OUTPUT);
  receiveBRXir();
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
