#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int Smoke = 0;
int r = 0;

#define t1 10
#define t2 9
#define t3 8

int distanceThreshold = 100;
bool alarmActive = false;
int buttonState = 0;
int oldButton = 0;

void setup() {
  pinMode(A0, INPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(11, INPUT);
}

void loop() {
  checkSmoke();
  checkDistance();
  handleButton();
}

void checkSmoke() {
  Smoke = analogRead(A0);
  Serial.println(analogRead(A0));

  if (Smoke >= 25 && oldButton != 1) {
    activateAlarm();
    displaySmokeInfo();
  } else {
    deactivateAlarm();
    displayGoodDay();
  }

  delay(10); // Delay a little bit to improve simulation performance
}

void activateAlarm() {
  digitalWrite(13, HIGH);
  digitalWrite(12, LOW);
  tone(13, 523); // play tone 60 (C5 = 523 Hz)
  alarmActive = true;
}

void deactivateAlarm() {
  digitalWrite(13, LOW);
  digitalWrite(12, HIGH);
  lcd.clear();
  noTone(13);
  alarmActive = false;
}

void handleButton() {
  buttonState = digitalRead(11);
  if (buttonState != oldButton && buttonState == HIGH) { 
    if (alarmActive) {
      deactivateAlarm(); // Turn off the alarm when the button is pressed
      oldButton = buttonState;
    } 
    delay(500); // Debouncing delay
  }else if(oldButton == buttonState && oldButton == HIGH){
    oldButton = 0;
    delay(500);
    }
}

void displaySmokeInfo() {
  lcd.setCursor(0, 0);
  lcd.print("Emergency exit is right to the elevator");
  delay(100);
  lcd.setCursor(0, 1);
  lcd.print("Fire and Rescue Dial 101 immediately");
  lcd.setCursor(1, 0);

  for (r = 0; r < 36; r++) {
    lcd.scrollDisplayLeft();
    delay(100);
  }
}

void displayGoodDay() {
  lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("GOOD");
  lcd.setCursor(6, 1);
  lcd.print("DAY:)");
  delay(1000);
}

// distance 

long readDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}
void displayFreeSlots(int slots) {
  lcd.setCursor(0, 0);
  Serial.print(slots);
  Serial.println(" Slots Free");
  lcd.print(slots);
  lcd.print(" Slots Free");
  lcd.setCursor(0, 1);

  if (slots == 3) {
    lcd.print("Slot 1 2 3 Free");
  } else if (slots == 2) {
    if (readDistance(t1, t1) > distanceThreshold && readDistance(t2, t2) > distanceThreshold) {
      lcd.print("Slot 1 & 2 Free");
    } else if (readDistance(t1, t1) > distanceThreshold && readDistance(t3, t3) > distanceThreshold) {
      lcd.print("Slot 1 & 3 Free");
    } else {
      lcd.print("Slot 2 & 3 Free");
    }
  } else if (slots == 0) {
    Serial.print("0");
    Serial.println(" Slots Free");
    lcd.print("No Slot Free");
    lcd.setCursor(0, 1);
    lcd.print("Parking Full");
  } else if (slots == 1) {
    if (readDistance(t1, t1) > distanceThreshold) {
      lcd.print("Slot 1 is Free");
    } else if (readDistance(t2, t2) > distanceThreshold) {
      lcd.print("Slot 2 is Free");
    } else {
      lcd.print("Slot 3 is Free");
    }
  }

  delay(500);
}

void checkDistance() {
  float d1 = 0.01723 * readDistance(t1, t1);
  float d2 = 0.01723 * readDistance(t2, t2);
  float d3 = 0.01723 * readDistance(t3, t3);

  Serial.println("d1 = " + String(d1) + "cm");
  Serial.println("d2 = " + String(d2) + "cm");
  Serial.println("d3 = " + String(d3) + "cm");

  if (d1 > distanceThreshold && d2 > distanceThreshold && d3 > distanceThreshold) {
    displayFreeSlots(3);
  } else if ((d1 > distanceThreshold && d2 > distanceThreshold) || (d2 > distanceThreshold && d3 > distanceThreshold) || (d3 > distanceThreshold && d1 > distanceThreshold)) {
    displayFreeSlots(2);
  } else if (d1 < distanceThreshold && d2 < distanceThreshold && d3 < distanceThreshold) {
    displayFreeSlots(0);
  } else if ((d1 < distanceThreshold && d2 < distanceThreshold) || (d2 < distanceThreshold && d3 < distanceThreshold) || (d3 < distanceThreshold && d1 < distanceThreshold)) {
    displayFreeSlots(1);
  }

  delay(100);
}
