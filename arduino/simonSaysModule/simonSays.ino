#include "DSerial.h"
#include "KTANECommon.h"
#include <NeoICSerial.h>

NeoICSerial serial_port;
DSerialClient client(serial_port, MY_ADDRESS);
KTANEModule module(client);

#define MAX_NUM_STAGES 5
#define RED 0
#define YELLOW 1
#define GREEN 2
#define BLUE 3

int stage;
int num_stages = random(3, MAX_NUM_STAGES + 1);
int stage_colors[MAX_NUM_STAGES];
int mapping[2][3][4] = {
  { // No Vowel
    {BLUE, RED, GREEN, YELLOW}, // No Strikes
    {RED, GREEN, YELLOW, BLUE}, // One Strike
    {YELLOW, RED, BLUE, GREEN}, // Two Strikes
  },
  { // Vowel
    {BLUE, GREEN, YELLOW, RED}, // No Strikes
    {YELLOW, RED, BLUE, GREEN}, // One Strike
    {GREEN, BLUE, YELLOW, RED}, // Two Strikes
  },
}

void youWin() {
  module.sendSolve();
  digitalWrite(3, HIGH);
}

void youLose() {
  module.sendStrike();
  digitalWrite(4, HIGH);
  delayWithUpdates(module, 500);
  digitalWrite(4, LOW);
}

void setup() {
  serial_port.begin(19200);
  Serial.begin(19200);

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  while(!module.getConfig()){
    module.interpretData();
  }

  for(int i = 0; i < num_stages; i++) {
    stage_colors[i] = random(0, 4);
  }
  stage = 0;

  module.sendReady();
  digitalWrite(3, HIGH);
  delayWithUpdates(module, 1000);
  digitalWrite(3, LOW);
}

void loop() {
  module.interpretData();
  int button_pressed = 0;

  if(!module.is_solved){
    if(digitalRead(9)) {
      button_pressed = RED;
    } else if(digitalRead(10)) {
      button_pressed = YELLOW;
    } else if(digitalRead(11)) {
      button_pressed = GREEN;
    } else if(digitalRead(12)) {
      button_pressed = BLUE;
    }

    int vowel = module.serialContainsVowel();
    int strikes = module.getNumStrikes()
    int light_color = stage_colors[stage];
    if(button_pressed == mapping[vowel][strikes][light_color]) {
      stage++;
      if(stage == num_stages){
        youWin();
      }
    } else {
      youLose();
    }
  }
}