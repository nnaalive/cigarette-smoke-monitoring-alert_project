#define BLYNK_TEMPLATE_ID "..."
#define BLYNK_TEMPLATE_NAME "..."
#define BLYNK_AUTH_TOKEN "..."

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define BUZZpin 26
#define MQpin 35
#define redLED 14    // indikator asap rokok berlebih
#define yellowLED 27 // indikator gas CO berlebih
#define RL 1
#define Ro 2.5

// Get from datasheet MQ-2
#define aSMOKE -0.380125260960334
#define logbSMOKE 3.23938630995162
#define aCO -0.3104037934977602
#define logbCO 3.296899122408560

BlynkTimer timer;

// Set password to "" for open networks
char ssid[] = "..."; // Nama jaringan WiFi
char pass[] = "..."; // Password (kata sandi) jaringan

float VRL, Rs, ratio, analog_val, smoke, co;
int ctr, redIntensity, yellowIntensity;

// main program
void setup(){
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(BUZZpin, OUTPUT);
  digitalWrite(BUZZpin, LOW);
  analogSetAttenuation(ADC_11db);

  ledcSetup(0, 5000, 8);    // PWM red LED
  ledcAttachPin(redLED, 0);
  ledcSetup(1, 5000, 8);    // PWM yellow LED
  ledcAttachPin(yellowLED, 1);

  timer.setInterval(200L, sendSensor);
}

void loop(){
  Blynk.run();
  timer.run();
}

void sendSensor() {
  analog_val = analogRead(MQpin);
  Serial.print("analog_val : ");
  Serial.print(analog_val);

  VRL = analog_val * (3.3 / 4095.0); // konversi ADC VRL
  Rs = ((3.3 * RL) / VRL) - RL;
  ratio = Rs / Ro;
  Serial.print(" | Ratio : ");
  Serial.println(ratio);

  //SMOKE
  smoke = pow(10, (log10(ratio) - logbSMOKE) / aSMOKE);
  Serial.print("SMOKE : ");
  Serial.print(smoke);
  Serial.println(" ppm");
  delay(1000);

  //CO
  co = pow(10, (log10(ratio) - logbCO) / aCO);
  Serial.print("CO : ");
  Serial.print(co);
  Serial.println(" ppm");
  delay(200);

  Blynk.virtualWrite(V0, co); 
  Blynk.virtualWrite(V1, smoke);
  
  //Logika LED – buzzer
  redIntensity    = map(smoke, 0, 3000, 0, 255);
  redIntensity    = constrain(redIntensity, 0, 255);
  yellowIntensity = map(co, 0, 3000, 0, 255);
  yellowIntensity = constrain(yellowIntensity, 0, 255);

  ledcWrite(0, redIntensity);    // atur kecerahan LED merah
  ledcWrite(1, yellowIntensity); // atur kecerahan LED kuning

  if(smoke > 500) { // asap berlebih
    for (ctr = 0; ctr < 3; ctr++) {
    digitalWrite(BUZZpin, HIGH);
    delay(200);
    digitalWrite(BUZZpin, LOW);
    delay(200);
    }
  } else {
    digitalWrite(BUZZpin, LOW);
  }
}