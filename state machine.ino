// =====================================================
// MODULE 2 - NON-BLOCKING STATE MACHINE
// Arduino Uno
// =====================================================
//
// LCD is declared in uart.ino
// setup() is in uart.ino
// loop() is in uart.ino
//
// This file contains ONLY the state-machine logic.
// =====================================================


// =====================================================
// DEVICE STATES
// =====================================================

enum DeviceState
{
  IDLE,
  RUNNING,
  DONE
};


// =====================================================
// CURRENT STATE
// =====================================================

DeviceState state = IDLE;


// =====================================================
// TIME WHEN RUNNING STARTED
// =====================================================

unsigned long startTime = 0;


// =====================================================
// INITIALIZE STATE MACHINE
// =====================================================

void stateMachineInit()
{
  state = IDLE;
  startTime = 0;
}


// =====================================================
// UPDATE STATE MACHINE
// NON-BLOCKING
// =====================================================

void stateMachineUpdate()
{
  // Current time
  unsigned long currentTime = millis();

  // ===================================================
  // IDLE STATE
  // ===================================================

  if (state == IDLE)
  {
    // SELECT button connected to A0
    if (analogRead(A0) < 100)
    {
      // Change state
      state = RUNNING;

      // Save starting time
      startTime = currentTime;

      // Display RUNNING
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("STATE:");

      lcd.setCursor(0, 1);
      lcd.print("RUNNING");

      // UART message
      uartSendString("State: RUNNING\r\n");
    }
  }

  // ===================================================
  // RUNNING STATE
  // ===================================================

  else if (state == RUNNING)
  {
    // Stay RUNNING for 5 seconds
    if (currentTime - startTime >= 5000)
    {
      // Change state
      state = DONE;

      // Display DONE
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("STATE:");

      lcd.setCursor(0, 1);
      lcd.print("DONE");

      // UART message
      uartSendString("State: DONE\r\n");
    }
  }

  // ===================================================
  // DONE STATE
  // ===================================================

  else if (state == DONE)
  {
    // Total 7 seconds:
    // 5 seconds RUNNING
    // 2 seconds DONE

    if (currentTime - startTime >= 7000)
    {
      // Return to IDLE
      state = IDLE;

      // Display IDLE
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("STATE:");

      lcd.setCursor(0, 1);
      lcd.print("IDLE");

      // UART message
      uartSendString("State: IDLE\r\n");
    }
  }
}
