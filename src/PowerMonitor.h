/*
 * PowerMonitor.h
 *
 * Arduino Library for AC Power Monitoring.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 22 April 2020 (My WFH amid CoViD-19 outbreaks)
 * Released into the public domain.
 */

#ifndef PowerMonitor_h
#define PowerMonitor_h

#include <Arduino.h>

#ifndef __AVR__
#error "This library only suites for AVR based board."
#endif

class PowerMonitor {
public:
  void initVoltageSensor(uint8_t pin = 0, float cal = 1.0);
  
  void initCurrentSensor(uint8_t pin = 1, float cal = 1.0);
  
  void sample();
  
  void calculate();
  
  inline void sampleAndCalculate() {
    sample();
    calculate();
  }
  
  float getFrequency(uint8_t ZCcount = 10);
  
  inline int16_t * getVoltageSampleArray() {
    return isInitV ? samplesV : nullptr;
  }
  
  inline int16_t * getCurrentSampleArray() {
    return isInitI ? samplesI : nullptr;
  }
  
  int16_t getVcc();
  
  inline void setVbgCalibration(int32_t cal = 1126400L) {
    if (1013760L <= cal && cal <= 1239040L) // Vbandgap calibration between 990 mV until 1210 mV, plus minus 10% of 1100 mV
      calVbg = cal;
  }
  
  uint8_t sampleCount;
  float Vrms, Irms, realPower, apparentPower, powerFactor;
  
private:
  int16_t getMidVal(int16_t * arr);
  
  int32_t calVbg = 1126400L;  // Vbandgap 1100 mV times 1024
  int16_t midValV = 512;
  uint8_t inPinV, inPinI;
  float calV, calI;
  bool isInitV = false, isInitI = false;
  int16_t * samplesV, * samplesI;
};

#endif /*PowerMonitor_h*/