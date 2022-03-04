/*
 By:Farkad Adnan
 E-mail: farkad.hpfa95@gmail.com
 inst : farkadadnan
 #farkadadnan , #farkad_adnan , فرقد عدنان#
 FaceBook: كتاب عالم الاردوينو
 inst : arduinobook
 #كتاب_عالم_الاردوينو  #كتاب_عالم_الآردوينو 
 */
#include "MAX30100_PulseOximeter.h"
#include <Wire.h>
#define REPORTING_PERIOD_MS     500

PulseOximeter pox;

const int numReadings=10;
float filterweight=0.5;
uint32_t tsLastReport = 0;
uint32_t last_beat=0;
int readIndex=0;
int average_beat=0;
int average_SpO2=0;
bool calculation_complete=false;
bool calculating=false;
bool initialized=false;
byte beat=0;

void onBeatDetected(){
  last_beat=millis();
}

void display_calculating(int j){
  if (not calculating) {
      calculating=true;
    initialized=false;
  }
  Serial.print(". ");
}
void display_values(){
  Serial.println("");
  Serial.print(average_beat);
  Serial.println(" Bpm");
  Serial.print("SpO2 ");
  Serial.print(average_SpO2);
  Serial.println("%"); 
}

void initial_display() {
  if (not initialized) 
  {
    Serial.print("Place finger on the sensor");  
    initialized=true;
  }
}

void calculate_average(int beat, int SpO2) {
  if (readIndex==numReadings) {
    calculation_complete=true;
    calculating=false;
    initialized=false;
    readIndex=0;
    display_values();
  }
  
  if (not calculation_complete and beat>30 and beat<220 and SpO2>50) {
    average_beat = filterweight * (beat) + (1 - filterweight ) * average_beat;
    average_SpO2 = filterweight * (SpO2) + (1 - filterweight ) * average_SpO2;
    readIndex++;
    display_calculating(readIndex);
  }
}

void setup(){
    Serial.begin(115200);
    pox.begin();
    pox.setOnBeatDetectedCallback(onBeatDetected);   
    initial_display();
}

void loop(){
    // Make sure to call update as fast as possible
    pox.update();
    if ((millis() - tsLastReport > REPORTING_PERIOD_MS) and (not calculation_complete)) {
        calculate_average(pox.getHeartRate(),pox.getSpO2());
        tsLastReport = millis();
    }
    if ((millis()-last_beat>10000)) {
      calculation_complete=false;
      average_beat=0;
      average_SpO2=0;
      initial_display();
    }
}
