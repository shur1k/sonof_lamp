#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define STRIPEOUT 12
#define LED       13
#define BUTTON    0 
#define SW_TIME   40   /* time for contacnts valuation */
#define STEP_TIME 30   /* time for change one step*/
#define STEP_BRIGHT 3  /* step for brightness change*/
#define PWMRANGE 100   /* esp8266 core range for analogWrite function*/

char auth[] = "auth";

char ssid[] = "SSID";
char pass[] = "PSWD";

boolean       sw_f = false, sw = false, gled = false;
unsigned long m_sw, m_step;
byte          cnt = 0, s_cnt=0, brightness = 0, goal_brightness = 0;

BLYNK_WRITE(V1)
{
  goal_brightness = param.asInt(); // assigning incoming value from pin V1 to a variable
}

void check_sw(void) 
{
  if (digitalRead(BUTTON)) { 
    sw    = false;
    cnt   = 0;
    s_cnt = 0;
  } 
  else {
    cnt++;
    s_cnt++;
  }
  if (cnt == 2) sw_f = true;
  if (s_cnt == 50) {
    gled = !gled;
    digitalWrite(LED, gled);
  }
  m_sw = millis(); 
}

void setup()
{
  pinMode(STRIPEOUT, OUTPUT); 
  pinMode(BUTTON,    INPUT);
  pinMode(LED,       OUTPUT);
  digitalWrite(LED,  HIGH);
  analogWriteRange(PWMRANGE);
  
  Blynk.begin(auth, ssid, pass);
  digitalWrite(LED, LOW);
  m_sw = millis();
  Blynk.virtualWrite(V1, brightness);
}

void loop()
{
  Blynk.run();
  if ((!digitalRead(BUTTON))&&(!sw)) {
    sw = true; 
    m_sw = millis();
  }
  
  if (sw && ((millis() - m_sw) > SW_TIME)) check_sw();
  
  if (brightness != goal_brightness && (millis() - m_step > STEP_TIME)) {
    if (brightness < goal_brightness) {
      brightness += STEP_BRIGHT;
      if ( (brightness >= goal_brightness)||( brightness < STEP_BRIGHT )) {
        brightness = goal_brightness;
        Blynk.virtualWrite(V1, brightness);
      }
    } else {
      brightness -= STEP_BRIGHT;
      if ( (brightness <= goal_brightness) || (brightness > (PWMRANGE - STEP_BRIGHT)) ) {
        brightness = goal_brightness;
        Blynk.virtualWrite(V1, brightness);
      }
    }
    analogWrite(STRIPEOUT, brightness);
    m_step = millis();    
  }
  if (sw_f) {
    sw_f = false;
    if (brightness <= STEP_BRIGHT) {
      goal_brightness = PWMRANGE;
    } else {
      goal_brightness = 0;
    }
  }
}
