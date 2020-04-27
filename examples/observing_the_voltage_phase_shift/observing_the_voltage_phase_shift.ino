/*
 * observing_the_voltage_phase_shift.ino
 * 
 * Created 26 April 2020 (amid CoViD-19 outbreaks)
 * by Zulns
 * @Gorontalo, Indonesia
 * 
 * This example code is in the public domain.
 * 
 * https://github.com/ZulNs/PowerMonitor
 */

#include <PowerMonitor.h>

const byte VOLTAGE_SENSOR_PIN = 0;
//const byte CURRENT_SENSOR_PIN = 1;
const float VOLTAGE_CALIBRATION = 1.0;
//const float CURRENT_CALIBRATION = 1.0;

PowerMonitor pmon;

void setup() {
  int i;
  int16_t * samplesV, * samplesI;
  
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println();
  Serial.println("*** Observing the Voltage Phase Shift ***");
  Serial.println();
  
  pmon.initVoltageSensor(VOLTAGE_SENSOR_PIN, VOLTAGE_CALIBRATION);
  //pmon.initCurrentSensor(CURRENT_SENSOR_PIN, CURRENT_CALIBRATION);
    // No need above line code.
    // After first time calling PowerMonitor::sample() method
    // will always init the current sensor by similar parameters
    // to PowerMonitor::initVoltageSensor().

  while (1) {
    pmon.sample();
    if (pmon.sampleCount == 0)
      Serial.println("No AC voltage...");
    else
      break;
    delay(1000);
  }
  
  // The following two lines of code must be performed after both sensor initialized.
  samplesV = pmon.getVoltageSampleArray();
  samplesI = pmon.getCurrentSampleArray();
  
  Serial.println(F("The sampling process is to take the voltage value and "));
  Serial.println(F("immediately followed by the current value repeatedly "));
  Serial.println(F("for one full voltage AC waveform. It takes around 90 times "));
  Serial.println(F("for 50Hz. If the current sensor is not initiated, then "));
  Serial.println(F("the voltage value on the same sensor will be sampled. "));
  Serial.println(F("Here the former sampling called V@V, and the later called V@I."));
  Serial.println();
  Serial.println("Number of sample (n): " + String(pmon.sampleCount));
  Serial.println(F("====================="));
  Serial.println(F(" n\tV@V\tV@I"));
  Serial.println(F("====================="));
  for (i = 0; i < pmon.sampleCount; ++i) {
    Serial.println(" " + String(i + 1) + "\t" + String(samplesV[i]) + "\t" + String(samplesI[i]));
  }
  Serial.println(F("====================="));
  
  pmon.calculate();
  
  Serial.println();
  Serial.println(F("After calculation, the phase of each V@V is shifted "));
  Serial.println(F("(here called shiftV) so that each result come near to "));
  Serial.println(F("each V@I value. Here you can observe the differences "));
  Serial.println(F("or similarities between the two."));
  Serial.println();
  Serial.println(F("====================="));
  Serial.println(F(" n\tshiftV\tV@I"));
  Serial.println(F("====================="));
  for (i = 0; i < pmon.sampleCount; ++i) {
    Serial.println(" " + String(i + 1) + "\t" + String(samplesV[i]) + "\t" + String(samplesI[i]));
  }
  Serial.println(F("====================="));

  Serial.println();
  Serial.println("Frequency\t\t= " + String(pmon.getFrequency()) + " Hz");
  Serial.println("Vcc (ADC ref voltage)\t= " + String(pmon.getVcc() / 1000.0, 3) + " V");
  Serial.println("V@V rms (sensor out)\t= " + String(pmon.Vrms, 3) + " V");
  Serial.println("V@I rms (sensor out)\t= " + String(pmon.Irms, 3) + " V");
}

void loop() {
}
