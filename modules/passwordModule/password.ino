#include "DSerial.h"
#include "KTANECommon.h"
#include <NeoICSerial.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

NeoICSerial serial_port;
DSerialClient client(serial_port, MY_ADDRESS);
KTANEModule module(client, 3, 4);

// Success LEDS: 3, 4
// Serial pins: 8, 9
// Display: 13, 11, 10 (clock, data, cs)
U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ U8X8_PIN_NONE);

// Submit button: 2
// Upper switches: 5, 6, 7, A5, A6
// Lower switches: A0, A1, A2, A3, A4

char *correct_str;
char possible_letters[6][5];
char *possible_words[35] = {
  "ABOUT", "AFTER", "AGAIN", "BELOW", "COULD",
  "EVERY", "FIRST", "FOUND", "GREAT", "HOUSE",
  "LARGE", "LEARN", "NEVER", "OTHER", "PLACE",
  "PLANT", "POINT", "RIGHT", "SMALL", "SOUND",
  "SPELL", "STILL", "STUDY", "THEIR", "THERE",
  "THESE", "THING", "THINK", "THREE", "WATER",
  "WHERE", "WHICH", "WORLD", "WOULD", "WRITE"
};

void dispStr(char *str) {
  u8g2.firstPage();
  do {
    u8g2.drawGlyph(4, 45, str[0]);
    u8g2.drawGlyph(29, 45, str[1]);
    u8g2.drawGlyph(54, 45, str[2]);
    u8g2.drawGlyph(79, 45, str[3]);
    u8g2.drawGlyph(104, 45, str[4]);
    u8g2.drawLine(0, 0, 0, 63);
    u8g2.drawLine(1, 0, 1, 63);
    u8g2.drawLine(2, 0, 2, 63);
    u8g2.drawLine(26, 0, 26, 63);
    u8g2.drawLine(27, 0, 27, 63);
    u8g2.drawLine(51, 0, 51, 63);
    u8g2.drawLine(52, 0, 52, 63);
    u8g2.drawLine(76, 0, 76, 63);
    u8g2.drawLine(77, 0, 77, 63);
    u8g2.drawLine(101, 0, 101, 63);
    u8g2.drawLine(102, 0, 102, 63);
    u8g2.drawLine(126, 0, 126, 63);
    u8g2.drawLine(127, 0, 127, 63);
  } while(u8g2.nextPage());
}

void generateGrid(char* word) {
  int position, temp, letter_in_word;
  int num_possible = 35;

  while(num_possible != 1){
    Serial.println("Attempting word generation.");
    for(int i = 0; i < 5; i++) {
      char alphabet[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      position = random(6);
      alphabet[word[i] - 'A'] = 0;
      for(int j = 0; j < 6; j++) {
        if (position == j) {
          possible_letters[j][i] = word[i];
        } else {
          do {
            temp = random(26);
            possible_letters[j][i] = 'A' + temp;
          } while (alphabet[temp] == 0);
          alphabet[temp] = 0; 
        }
      }
    }

    // check for more than one possible word
    uint8_t word_checklist[35] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
    num_possible = 35;
    for(int i = 0; i < 5; i++) {
      for(int word_idx = 0; word_idx < 35; word_idx++) {
        if(word_checklist[word_idx] == 1){
          letter_in_word = 0;
          for(int j = 0; j < 6; j++) {
            if(possible_letters[j][i] == possible_words[word_idx][i]){
              letter_in_word = 1;
            }
          }
          word_checklist[word_idx] = letter_in_word;
          num_possible -= letter_in_word;
        }
      }
      if(num_possible == 1) {
        break;
      }
    }
  }
}

void setup() {
  serial_port.begin(19200);
  Serial.begin(19200);

  u8g2.begin();
  u8g2.setFont(u8g2_font_inb27_mf);

  // while(!module.getConfig()){
  //   module.interpretData();
  // }

  randomSeed(config_to_seed(module.getConfig()));
  int word_idx = random(35);
  correct_str = possible_words[word_idx];
  Serial.println(correct_str);
  generateGrid(correct_str);

  // module.sendReady();
}

void loop() {
  // module.interpretData();
  delay(10);
  dispStr(correct_str);
  if(!module.is_solved){
    /*
    checkInputs();
    if(they_solved_it) {
      module.win();
    }
    if(they_messed_up) {
      module.strike();
    }
    updateOutputs();
    */
  }
}