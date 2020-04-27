/*
 * PowerMonitor.cpp
 *
 * Arduino Library for AC Power Monitoring.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 22 April 2020 (My WFH amid CoViD-19 outbreaks)
 * Released into the public domain.
 */

#include <Arduino.h>
#include "PowerMonitor.h"

void PowerMonitor::initVoltageSensor(uint8_t pin, float cal) {
  inPinV = pin;
  calV = cal;
  if (!isInitV)
    samplesV = (int16_t *)calloc(91, sizeof(int16_t));
  isInitV = true;
}

void PowerMonitor::initCurrentSensor(uint8_t pin, float cal) {
  inPinI = pin;
  calI = cal;
  if (!isInitI)
    samplesI = (int16_t *)calloc(91, sizeof(int16_t));
  isInitI = true;
}

void PowerMonitor::sample() {
  if (!isInitV)
    return;
  
  if (!isInitI)
    initCurrentSensor(inPinV, calV);
  
  sampleCount = 0;
  
  uint8_t i;
  int16_t startV, midValI;
  bool sampling = true, firstZC = false;
  uint32_t startTime = millis();
  
  startV = analogRead(inPinV);
  while (startV >= midValV - 20) {
    startV = analogRead(inPinV);
    if (millis() - startTime >= 30)
      return;
  }
  while (startV < midValV - 20) {
    startV = analogRead(inPinV);
    if (millis() - startTime >= 30)
      return;
  }
  
  while (sampling) {
    samplesV[sampleCount] = analogRead(inPinV);
    samplesI[sampleCount] = analogRead(inPinI);
    if (firstZC) {
      if (samplesV[sampleCount] > startV)
        sampling = false;
    }
    else {
      if (samplesV[sampleCount] < startV)
        firstZC = true;
    }
    if (++sampleCount == 90)
      sampling = false;
  }
  samplesV[sampleCount] = analogRead(inPinV);
  
  midValV = getMidVal(samplesV);
  midValI = getMidVal(samplesI);
  for (i = 0; i < sampleCount; ++i) {
    samplesV[i] -= midValV;
    samplesI[i] -= midValI;
  }
  samplesV[sampleCount] -= midValV;
}

void PowerMonitor::calculate() {
  if (!isInitV || !isInitI)
    return;
  
  float sumV = 0, sumI = 0, sumP = 0, adcStepVal = getVcc() / 1000.0 / 1024, ratioV = adcStepVal * calV, ratioI = adcStepVal * calI;
  int8_t i;
  
  for (int8_t i = 0; i < sampleCount; ++i) {
    sumV += (float)samplesV[i] * samplesV[i];
    sumI += (float)samplesI[i] * samplesI[i];
    // The phase of sampleV[] will be shifted so that it synchronizes with the sampling time of sampleI[].
    samplesV[i] = samplesV[i] + (samplesV[i + 1] - samplesV[i]) / 2;
    sumP += (float)samplesV[i] * samplesI[i];
  }
  
  Vrms = sqrt(sumV / sampleCount) * ratioV;
  Irms = sqrt(sumI / sampleCount) * ratioI;
  realPower = sumP / sampleCount * ratioV * ratioI;
  apparentPower = Vrms * Irms;
  powerFactor = realPower / apparentPower;
  if (isnan(powerFactor))
    powerFactor = 1.0;
}

float PowerMonitor::getFrequency(uint8_t ZCcount) {
  if (!isInitV)
    return 0.0;
  
  int16_t startV, sampleV;
  uint16_t timeout = ZCcount * 10 + 30;
  uint32_t startTime = millis(), timer;
  uint8_t ZCcounter = 0;
  bool phase;
  
  startV = analogRead(inPinV);
  while (startV > midValV + 20 || startV < midValV - 20) {
    if (millis() - startTime >= timeout)
      return 0.0;
    startV = analogRead(inPinV);
  }
  timer = micros();
  sampleV = analogRead(inPinV);
  phase = sampleV > startV;
  while (millis() - startTime < timeout && ZCcounter < ZCcount) {
    sampleV = analogRead(inPinV);
    if (phase && sampleV < startV || !phase && sampleV > startV) {
      ++ZCcounter;
      phase = !phase;
    }
  }
  timer = micros() - timer;
  return (millis() - startTime >= timeout) ? 0.0 : 500000.0 * ZCcount / timer;
}

// Thanks to Scott Daniels for http://provideyourown.com/2012/secret-arduino-voltmeter-measure-battery-voltage/
int16_t PowerMonitor::getVcc() {
  int16_t val;
  
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || defined(__AVR_AT90USB1286__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  ADCSRB &= ~_BV(MUX5);
#elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX = _BV(MUX5) | _BV(MUX0);
#elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX = _BV(MUX3) | _BV(MUX2);
#endif

  delay(2);  // Per datasheet "When the bandgap reference voltage is used as input to the ADC, it will take a certain time for the voltage to stabilize."
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC));
  val = ADCL;
  val |= ADCH << 8;
  return calVbg / val;
}

int16_t PowerMonitor::getMidVal(int16_t * arr) {
  int16_t minVal = 1023, maxVal = 0;
  for (uint8_t i = 0; i < sampleCount; ++i) {
    if (arr[i] < minVal)
      minVal = arr[i];
    if (arr[i] > maxVal)
      maxVal = arr[i];
  }
  return minVal + ((maxVal - minVal) >> 1);
}
