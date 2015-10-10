/*
  freeze_alarm.ino

  freezer door open detect and sound an alarm for arduino.

  Copyright (c) 2015- Yoshinori Isozaki.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 */

#include <FlexiTimer2.h>

/* pin definition */
const unsigned int SWITCH_PIN = 2;
const unsigned int BUZZER_PIN = 12;
const unsigned int LED_PIN = 13;

/* switch open duration */
const unsigned int SWITCH_OPEN_TIMEOUT_MSEC = 60 * 1000;
const unsigned int BUZZER_ENABLE_INTERVAL_MSEC = 500;
const unsigned int BUZZER_CLOSED_DURATION_MSEC = 100;

/* share timer state for interrupt and timer  */
static volatile bool enabled_buzzer_timer = false;

/* function definitions */
bool isOpenedSwitch();
void handleSwitchStateChanged();

void enableLed();
void disableLed();
bool isEnableBuzzer();
void enableBuzzer();
void disableBuzzer();
void handleBuzzerTimer();
void handleSwitchClosedBuzzerTimer();

void enableTimer(const unsigned long duration, void (*func)());
void disableTimer();

void setup() {
  Serial.begin(115200);

  pinMode(SWITCH_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);  
  pinMode(LED_PIN, OUTPUT);  

  /* enable interrupt for external switch */
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), handleSwitchStateChanged, CHANGE);
}

void loop() {
  /* do nothing in loop */
}

void handleSwitchStateChanged()
{
  if (isOpenedSwitch()) {
    disableBuzzer();
    disableTimer();
    enableTimer(SWITCH_OPEN_TIMEOUT_MSEC, handleBuzzerTimer);
    enableLed();
  } else {
    disableTimer();
    if (!enabled_buzzer_timer) {
      enableBuzzer();
      enableTimer(BUZZER_CLOSED_DURATION_MSEC, handleSwitchClosedBuzzerTimer);
    } else {
      disableBuzzer();
    }
    enabled_buzzer_timer = false;
    disableLed();
  }
}

void handleBuzzerTimer()
{
  enabled_buzzer_timer = true;
  disableTimer();
  enableTimer(BUZZER_ENABLE_INTERVAL_MSEC, handleBuzzerTimer);

  if (isEnableBuzzer()) {
    disableBuzzer();
  } else {
    enableBuzzer();
  }
}

void handleSwitchClosedBuzzerTimer()
{
  enabled_buzzer_timer = false;
  disableBuzzer();
  disableTimer();
}

bool isOpenedSwitch()
{
  return (digitalRead(SWITCH_PIN) == HIGH);
}


void enableLed()
{
  digitalWrite(LED_PIN, HIGH);
}

void disableLed()
{
  digitalWrite(LED_PIN, LOW);
}

bool isEnableBuzzer()
{
  return (digitalRead(BUZZER_PIN) == HIGH);
}

void enableBuzzer()
{
  digitalWrite(BUZZER_PIN, HIGH);
}

void disableBuzzer()
{
  digitalWrite(BUZZER_PIN, LOW);
}

void enableTimer(const unsigned long duration, void (*func)())
{
  FlexiTimer2::set(duration, func);
  FlexiTimer2::start();
}

void disableTimer()
{
  FlexiTimer2::stop();
}
