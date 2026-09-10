
#include <LiquidCrystal.h>

// =====================================================
// MODULE 2 - NON-BLOCKING STATE MACHINE
// Arduino Uno
// =====================================================

// LCD pins
// RS -> D8
// EN -> D9
// D4 -> D4
// D5 -> D5
// D6 -> D6
// D7 -> D7

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Device states
enum DeviceState
{
  IDLE,
  RUNNING,
  DONE
};

DeviceState state = IDLE;

// Time when RUNNING started
unsigned long startTime = 0;

void setup()
{
  // Initialize LCD
  lcd.begin(16, 2);

  // Initialize Serial
  Serial.begin(9600);

  // Initial LCD display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("DEVICE WORKFLOW");

  lcd.setCursor(0, 1);
  lcd.print("IDLE");

  Serial.println("State: IDLE");
}

void loop()
{
  // Get current time
  unsigned long currentTime = millis();

  // Check current state
  switch (state)
  {
    // =================================================
    // IDLE STATE
    // =================================================
    case IDLE:

      // SELECT button connected to A0
      if (analogRead(A0) < 100)
      {
        // Change state
        state = RUNNING;

        // Save starting time
        startTime = currentTime;

        // Display RUNNING on LCD
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("STATE:");

        lcd.setCursor(0, 1);
        lcd.print("RUNNING");

        // Send message to Serial Monitor
        Serial.println("State: RUNNING");
      }

      break;


    // =================================================
    // RUNNING STATE
    // =================================================
    case RUNNING:

      // Stay in RUNNING for 5 seconds
      if (currentTime - startTime >= 5000)
      {
        // Change state
        state = DONE;

        // Display DONE on LCD
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("STATE:");

        lcd.setCursor(0, 1);
        lcd.print("DONE");

        // Send message to Serial Monitor
        Serial.println("State: DONE");
      }

      break;


    // =================================================
    // DONE STATE
    // =================================================
    case DONE:

      // 5 seconds RUNNING + 2 seconds DONE = 7 seconds
      if (currentTime - startTime >= 7000)
      {
        // Return to IDLE
        state = IDLE;

        // Display IDLE on LCD
        lcd.clear();

        lcd.setCursor(0, 0);
        lcd.print("STATE:");

        lcd.setCursor(0, 1);
        lcd.print("IDLE");

        // Send message to Serial Monitor
        Serial.println("State: IDLE");
      }

      break;
  }
}

