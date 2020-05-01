/*
 * plotting_the_graphs.ino
 * 
 * Description:
 * Plotting multiple graphs of voltage, current, and real power at once.
 * 
 * Usage:
 * - Open Serial Plotter window by clicking [Tools] => [Serial Plotter]
 *   or press [Ctrl]+[Shift]+[L] buttons on your keyboard.
 * - The blue color for voltage graphs, red for current, and green for real power.
 * - Select 9600 Baud, also select [Newline] instead of [No line ending].
 * - To change the speed of drawing the graphs, adjust the delay time
 *   (default 100ms) by send "d50" or "d200". The former changes the delay
 *   to 50ms and the latter to 200ms.
 * - To reduce the shape of voltage graph by two times, simply send "v0.5".
 * - To increase the shape of current graph by 10 times, simply send "i10".
 * - To reduce the shape of real power graph by 20 times, simply send "p0.05".
 * 
 * Created 27 April 2020 (amid CoViD-19 outbreaks)
 * by Zulns
 * @Gorontalo, Indonesia
 * 
 * This example code is in the public domain.
 * 
 * https://github.com/ZulNs/PowerMonitor
 */

#include <PowerMonitor.h>

const byte VOLTAGE_SENSOR_PIN = 0;
const byte CURRENT_SENSOR_PIN = 1;
//const float VOLTAGE_CALIBRATION = 155.0;
//const float CURRENT_CALIBRATION = 3.06;

PowerMonitor pmon;

int16_t * samplesV, * samplesI;

void setup() {
  Serial.begin(9600);
  
  pmon.initVoltageSensor(VOLTAGE_SENSOR_PIN);
  pmon.initCurrentSensor(CURRENT_SENSOR_PIN);

  samplesV = pmon.getVoltageSampleArray();
  samplesI = pmon.getCurrentSampleArray();
}

void loop() {
  static float scaleV = 1.0, scaleI = 1.0, scaleP = 1.0;
  static unsigned int timeout = 100;
  static char str[12];
  static byte strCtr;
  static char cmdType = '-';
  char chr;
  unsigned long startTime;
  
  pmon.sampleAndCalculate();
  for (byte i = 0; i < pmon.sampleCount; ++i) {
    Serial.print(samplesV[i] * scaleV);
    Serial.print("\t");
    Serial.print(samplesI[i] * scaleI);
    Serial.print("\t");
    Serial.println((int32_t)samplesV[i] * samplesI[i] * scaleP);
    startTime = millis();
    while (millis() - startTime < timeout) {
      while (Serial.available()) {
        chr = Serial.read();
        if (cmdType == '-') {
          if (isAlpha(chr)) {
            chr &= 0xDF; // make uppercase
            if (chr == 'V' || chr == 'I' || chr == 'P' || chr == 'D') {
              strCtr = 0;
              cmdType = chr;
            }
          }
        }
        else {
          if (chr == '\n' && strCtr > 0) {
            str[strCtr] = 0; // zero terminate string
            switch (cmdType) {
              case 'V':
                scaleV = atof(str);
                break;
              case 'I':
                scaleI = atof(str);
                break;
              case 'P':
                scaleP = atof(str);
                break;
              case 'D':
                timeout = atoi(str);
            }
            if (cmdType != 'D')
              i = 0;
            strCtr = 0;
            cmdType = '-';
          }
          else if (strCtr < 11 && (isDigit(chr) || chr == '-' || chr == '.'))
            str[strCtr++] = chr;
          else {
            strCtr = 0;
            cmdType = '-';
          }
        }
      }
    }
  }
}
