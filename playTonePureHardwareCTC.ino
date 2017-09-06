#include "pitches.h"

#define MAX_MUSIC_LEN 100
float m_notes[MAX_MUSIC_LEN];
float m_beats[MAX_MUSIC_LEN];
float m_tempo = 70;
int   m_i = -1;

void setup() {
  setupBuzzer();
  playImperialMarch();
}

void loop() {
  // nothing to do here ;)
}

void setupBuzzer(){
  pinMode(5, OUTPUT);// OC3A

  // TIMER3 - Note freq timer setup
  TCCR3A = 0;
  TCCR3B = 0;

  // CTC
  bitWrite(TCCR3B, WGM32, 1);
  bitWrite(TCCR3A, WGM30, 0);
  // toggle mode 
  bitWrite(TCCR3A, COM3A0, 1);
  bitWrite(TCCR3A, COM3A1, 0);
  // prescaler 64
  bitWrite(TCCR3B, CS30, 1);
  bitWrite(TCCR3B, CS31, 1);

  // TIMER4 - Note duration timer setup
  TCCR4A = 0;
  TCCR4B = 0;
  // CTC
  bitWrite(TCCR4B, WGM42, 1);
  // prescaler 1024
  bitWrite(TCCR4B, CS40, 1);
  bitWrite(TCCR4B, CS42, 1);
  
  bitWrite(TIMSK4, OCIE4A, 1);
  OCR4A=1500;
}

void disableBuzzer(){
  bitWrite(TCCR3B, CS30, 0);
  bitWrite(TCCR3B, CS31, 0);
}

void enableBuzzer(){
  bitWrite(TCCR3B, CS30, 1);
  bitWrite(TCCR3B, CS31, 1);
}

ISR(TIMER4_COMPA_vect){
  static bool playingnote = false;
  if(m_i==-1){
    disableBuzzer();
    OCR4A = 1500;
  }else{
    /// pequena pausa entre notas
    if(playingnote){
      disableBuzzer();
      OCR3A = 1500;
      TCNT3=0;
      OCR4A = (F_CPU/1024)/(1000/(0.1*m_beats[m_i]*m_tempo)) - 1;
      playingnote = false;
      m_i--;
    }else{
      if(m_notes[m_i]==NOTE_REST){
        disableBuzzer();
        OCR3A = 1500;
        TCNT3=0;
      }else{
        enableBuzzer();
        OCR3A = (F_CPU/(64*m_notes[m_i]))/2 - 1;
        TCNT3=0;
      }
      OCR4A = (F_CPU/1024)/(1000/(m_beats[m_i]*m_tempo)) - 1;
      playingnote = true;
    }
  }
}

// I've got some errors withou this
ISR(TIMER3_COMPA_vect){
// do nothing, but the music will not play withou this
}

void playImperialMarch(){
  m_tempo = 70;
  m_i =46;
  float notes[] = {NOTE_G4,NOTE_G4,NOTE_G4,NOTE_Eb4,NOTE_Bb4,NOTE_G4,NOTE_Eb4,NOTE_Bb4,NOTE_G4,
                   NOTE_D5,NOTE_D5,NOTE_D5,NOTE_Eb5,NOTE_Bb4,NOTE_Gb4,NOTE_Eb4,NOTE_Bb4,NOTE_G4,
                   NOTE_G5,NOTE_G4,NOTE_G4,NOTE_G5,NOTE_Gb5,NOTE_F5,
                   NOTE_E5,NOTE_Eb5,NOTE_E5,NOTE_REST,NOTE_G4,NOTE_REST,
                   NOTE_Db5,NOTE_C5,NOTE_B4,NOTE_Bb4,NOTE_A4,NOTE_Bb4,NOTE_REST,NOTE_Eb4,NOTE_REST,NOTE_Gb4,
                   NOTE_Eb4,NOTE_Bb4,NOTE_G4,NOTE_Eb4,NOTE_Bb4,NOTE_G4}; 
  float beats[] = { 8, 8, 8, 6, 2, 8, 6, 2, 16, 8, 
                    8, 8, 6, 2, 8, 6, 2, 16, 8, 6, 
                    2, 8, 6, 2, 2, 2, 2, 6, 2, 2, 
                    8, 6, 2, 2, 2, 2, 6, 2, 2, 9,
                    6, 2, 8, 6, 2, 16  };
  for(int i=0;i<m_i;i++){
    m_notes[m_i-i-1]=notes[i];
    m_beats[m_i-i-1]=beats[i];
  }
  m_i--;
}

