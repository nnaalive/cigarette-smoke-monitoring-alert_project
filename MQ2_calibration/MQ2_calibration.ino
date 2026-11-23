#include "esp_adc_cal.h"
#define MQpin 35

float RL = 1.0; //load resistance (in kilo Ohms)
float analogVal = 0.0, VRL, Rs, Ro;

void setup() {
  Serial.begin(9600);
  analogSetAttenuation(ADC_11db);
}

void loop() {
  analogVal = analogRead(MQpin);
  float analogV1 = analogReadMilliVolts(MQpin) / 1000.0;
  float analogV2 = ReadVoltage(MQpin);

  VRL = 3.3 * (analogVal/4095.0);
  Rs = RL * ((3.3/VRL) - 1);
  Ro = Rs / 9.531618832347878;

  Serial.print("Analog-Value = ");
  Serial.print(analogVal);
  Serial.print(" | ADC v1 = ");
  Serial.print(analogV1);
  Serial.print(" | ADC v2 = ");
  Serial.print(analogV2);
  Serial.print(" | VRL = ");
  Serial.print(VRL);
  Serial.print(" | Rs = ");
  Serial.print(Rs);
  Serial.print(" | Ro udara bersih = ");
  Serial.println(Ro);
  delay(1000);
  }

float ReadVoltage(byte ADC_PIN) {
  float calibration = 1.000;
  float vref = 1.01;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12,
  ADC_WIDTH_BIT_12, 1100, &adc_chars);
  
  vref = adc_chars.vref;
  return(analogRead(ADC_PIN)/4095.0)*3.3*(1100/vref)*calibration;
}
