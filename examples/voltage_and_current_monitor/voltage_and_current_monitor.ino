/*
 * voltage_and_current_monitor.ino
 * 
 * Created 27 April 2020 (amid CoViD-19 outbreaks)
 * by ZulNs
 * @Gorontalo, Indonesia
 * 
 * This example code is in the public domain.
 * 
 * https://github.com/ZulNs/PowerMonitor
 */

#include <PowerMonitor.h>

const byte VOLTAGE_SENSOR_PIN = 0;
const byte CURRENT_SENSOR_PIN = 1;
const float VOLTAGE_CALIBRATION = 155.0;
const float CURRENT_CALIBRATION = 3.06;

PowerMonitor pmon;

void setup() {
  int i;
  int16_t * samplesV, * samplesI;
  
  Serial.begin(9600);
  while (!Serial);
  
  Serial.println();
  Serial.println("*** AC Voltage, Current and Power Monitor ***");
  Serial.println();
  
  pmon.initVoltageSensor(VOLTAGE_SENSOR_PIN, VOLTAGE_CALIBRATION);
  pmon.initCurrentSensor(CURRENT_SENSOR_PIN, CURRENT_CALIBRATION);
}

void loop() {
  float freq = pmon.getFrequency();
  float Vcc = pmon.getVcc() / 1000.0;
  
  pmon.sampleAndCalculate();
  
  if (freq > 0.0 && pmon.sampleCount > 0) {
    Serial.println("Vcc (ADC ref voltage)\t= " + String(Vcc, 3) + " V");
    Serial.println("Out VT (Vrms)\t\t= " + String(pmon.Vrms / VOLTAGE_CALIBRATION, 3) + " V");
    Serial.println("Out CT (Vrms)\t\t= " + String(pmon.Irms / CURRENT_CALIBRATION, 3) + " V");
    Serial.println();
    Serial.println("Frequency\t\t= " + String(freq, 3) + " Hz");
    Serial.println("Vrms\t\t\t= " + String(pmon.Vrms, 3) + " V");
    Serial.println("Irms\t\t\t= " + String(pmon.Irms, 3) + " A");
    Serial.println("Real power\t\t= " + String(pmon.realPower, 3) + " W");
    Serial.println("Apparent power\t\t= " + String(pmon.apparentPower, 3) + " VA");
    Serial.println("Power factor\t\t= " + String(pmon.powerFactor, 3));
  }
  else
    Serial.println("No AC voltage...");
  Serial.println();
  delay(500);
}
