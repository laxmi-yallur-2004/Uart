#include <LiquidCrystal.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// ==================================================
// LCD
// RS, EN, D4, D5, D6, D7
// ==================================================
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// ==================================================
// UART CONFIGURATION
// ==================================================

#define F_CPU 16000000UL
#define BAUD 9600UL

#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)

// ==================================================
// RING BUFFER
// ==================================================

#define BUFFER_SIZE 16

struct RingBuffer
{
  volatile char buffer[BUFFER_SIZE];
  volatile uint8_t head;
  volatile uint8_t tail;
  volatile uint16_t overflow;
};

RingBuffer rxBuffer;
RingBuffer txBuffer;

// ==================================================
// BUFFER INITIALIZATION
// ==================================================

void bufferInit(RingBuffer &rb)
{
  rb.head = 0;
  rb.tail = 0;
  rb.overflow = 0;
}

// ==================================================
// BUFFER EMPTY
// ==================================================

bool bufferEmpty(RingBuffer &rb)
{
  return rb.head == rb.tail;
}

// ==================================================
// BUFFER FULL
// ==================================================

bool bufferFull(RingBuffer &rb)
{
  uint8_t next = rb.head + 1;

  if (next >= BUFFER_SIZE)
  {
    next = 0;
  }

  return next == rb.tail;
}

// ==================================================
// BUFFER PUT FROM MAIN PROGRAM
// ==================================================

bool bufferPut(RingBuffer &rb, char data)
{
  noInterrupts();

  uint8_t next = rb.head + 1;

  if (next >= BUFFER_SIZE)
  {
    next = 0;
  }

  if (next == rb.tail)
  {
    rb.overflow++;

    interrupts();
    return false;
  }

  rb.buffer[rb.head] = data;
  rb.head = next;

  interrupts();

  return true;
}

// ==================================================
// BUFFER PUT FROM ISR
// ==================================================

bool bufferPutISR(RingBuffer &rb, char data)
{
  uint8_t next = rb.head + 1;

  if (next >= BUFFER_SIZE)
  {
    next = 0;
  }

  if (next == rb.tail)
  {
    rb.overflow++;
    return false;
  }

  rb.buffer[rb.head] = data;
  rb.head = next;

  return true;
}

// ==================================================
// BUFFER GET FROM MAIN PROGRAM
// ==================================================

bool bufferGet(RingBuffer &rb, char &data)
{
  noInterrupts();

  if (rb.head == rb.tail)
  {
    interrupts();
    return false;
  }

  data = rb.buffer[rb.tail];

  rb.tail++;

  if (rb.tail >= BUFFER_SIZE)
  {
    rb.tail = 0;
  }

  interrupts();

  return true;
}

// ==================================================
// BUFFER GET FROM ISR
// ==================================================

bool bufferGetISR(RingBuffer &rb, char &data)
{
  if (rb.head == rb.tail)
  {
    return false;
  }

  data = rb.buffer[rb.tail];

  rb.tail++;

  if (rb.tail >= BUFFER_SIZE)
  {
    rb.tail = 0;
  }

  return true;
}

// ==================================================
// UART INITIALIZATION
// ==================================================

void uartInit()
{
  UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
  UBRR0L = (uint8_t)UBRR_VALUE;

  UCSR0A = 0;

  UCSR0B =
      (1 << RXEN0) |
      (1 << TXEN0) |
      (1 << RXCIE0);

  UCSR0C =
      (1 << UCSZ01) |
      (1 << UCSZ00);
}

// ==================================================
// UART SEND CHARACTER
// ==================================================

void uartSendChar(char data)
{
  bufferPut(txBuffer, data);

  UCSR0B |= (1 << UDRIE0);
}

// ==================================================
// UART SEND STRING
// ==================================================

void uartSendString(const char *text)
{
  while (*text)
  {
    uartSendChar(*text);
    text++;
  }
}

// ==================================================
// UART RECEIVE ISR
// ==================================================

ISR(USART_RX_vect)
{
  char received = UDR0;

  bufferPutISR(rxBuffer, received);
}

// ==================================================
// UART TRANSMIT ISR
// ==================================================

ISR(USART_UDRE_vect)
{
  char data;

  if (bufferGetISR(txBuffer, data))
  {
    UDR0 = data;
  }
  else
  {
    UCSR0B &= ~(1 << UDRIE0);
  }
}

// ==================================================
// PASS / FAIL
// ==================================================

int passCount = 0;
int failCount = 0;

void testResult(const char *name, bool result)
{
  if (result)
  {
    uartSendString("PASS: ");
    uartSendString(name);
    uartSendString("\r\n");

    passCount++;
  }
  else
  {
    uartSendString("FAIL: ");
    uartSendString(name);
    uartSendString("\r\n");

    failCount++;
  }
}

// ==================================================
// MODULE 1 TESTS
// ==================================================

void runTests()
{
  RingBuffer rb;
  char data;

  uartSendString("\r\n");
  uartSendString("===== MODULE 1 TESTS =====\r\n");

  // Test 1
  bufferInit(rb);

  testResult(
    "Buffer empty after init",
    bufferEmpty(rb)
  );

  // Test 2
  testResult(
    "Get from empty buffer fails",
    !bufferGet(rb, data)
  );

  // Test 3
  testResult(
    "Put A",
    bufferPut(rb, 'A')
  );

  // Test 4
  bool getA = bufferGet(rb, data);

  testResult(
    "Get returns A",
    getA && data == 'A'
  );

  // Test 5 - FIFO
  bufferPut(rb, 'A');
  bufferPut(rb, 'B');
  bufferPut(rb, 'C');

  bufferGet(rb, data);
  bool fifoA = (data == 'A');

  bufferGet(rb, data);
  bool fifoB = (data == 'B');

  bufferGet(rb, data);
  bool fifoC = (data == 'C');

  testResult(
    "FIFO order A B C",
    fifoA && fifoB && fifoC
  );

  // Test 6 - Full buffer
  bufferInit(rb);

  for (int i = 0; i < BUFFER_SIZE - 1; i++)
  {
    bufferPut(rb, 'X');
  }

  testResult(
    "Buffer becomes full",
    bufferFull(rb)
  );

  // Test 7 - Overflow
  bool overflowResult = bufferPut(rb, 'Y');

  testResult(
    "Overflow rejects new data",
    !overflowResult
  );

  // Test 8 - Overflow counter
  testResult(
    "Overflow counter increments",
    rb.overflow == 1
  );

  // Summary
  uartSendString("==========================\r\n");

  uartSendString("TOTAL PASS: ");
  uartSendChar('0' + passCount);
  uartSendString("\r\n");

  uartSendString("TOTAL FAIL: ");
  uartSendChar('0' + failCount);
  uartSendString("\r\n");

  uartSendString("==========================\r\n");

  // LCD result
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PASS:");
  lcd.print(passCount);

  lcd.setCursor(9, 0);
  lcd.print("FAIL:");
  lcd.print(failCount);

  lcd.setCursor(0, 1);

  if (failCount == 0)
  {
    lcd.print("MODULE 1 PASS");
  }
  else
  {
    lcd.print("CHECK FAILURES");
  }
}

// ==================================================
// NON-BLOCKING TEST START
// ==================================================

bool testsStarted = false;
unsigned long startupTime = 0;

void startTestsNonBlocking()
{
  if (!testsStarted)
  {
    if (startupTime == 0)
    {
      startupTime = millis();
    }

    if (millis() - startupTime >= 1000)
    {
      testsStarted = true;

      runTests();
    }
  }
}

// ==================================================
// SETUP
// ONLY ONE setup() IN PROJECT
// ==================================================

void setup()
{
  // Initialize buffers
  bufferInit(rxBuffer);
  bufferInit(txBuffer);

  // Initialize LCD
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Module 1 UART");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Initialize UART
  uartInit();

  // Initialize Module 2 state machine
  stateMachineInit();

  // Enable interrupts
  sei();
}

// ==================================================
// LOOP
// ONLY ONE loop() IN PROJECT
// ==================================================

void loop()
{
  // Start tests without delay()
  startTestsNonBlocking();

  // Run Module 2 state machine
  stateMachineUpdate();

  // Process received UART data
  char receivedData;

  if (bufferGet(rxBuffer, receivedData))
  {
    // Echo received character
    uartSendChar(receivedData);
  }
}
