
#define STROBE 14

// Active HIGH
#define LED1 13

#define led_on()  PORTB |= 0x20
#define led_off() PORTB &= 0xdf

// D0-D7
// Bottom nibble is PORTB, bits0-3
// Top nibble is PORTD, bits 4-7
byte data[8] = { 8,9,10,11,4,5,6,7 };

// For keycodes 0 thru 87
byte bytes[88] = {0,3,2,11,6,7,5,4,9,8,10,9,4,7,6,11,2,3,1,3,11,6,7,5,9,8,8,10,9,4,5,6,11,2,3,1,0,0,5,9,10,8,8,10,9,4,5,7,11,2,2,3,1,0,0,4,10,8,8,10,9,4,5,7,11,0,3,3,1,10,3,2,11,11,6,7,5,4,9,8,10,4,5,7,6,2,1,0};
byte bits[88]  = {0,6,0, 5,0,5,0,0,6,6, 6,0,5,0,5, 0,5,0,6,5, 2,1,2,1,1,0,5, 0,5,1,5,2, 1,1,1,1,5,2,2,3, 1,1,2, 5,2,2,3,3, 3,2,3,2,2,6,3,7, 3,3,7, 2,7,3,7,7, 7,7,7,3,3, 7,4,6, 4, 6,4,6,4,6,4,4, 4,4,6,4,6,4,4,4};

// buffer that is read - 1 bit means key not pressed, 0 means key pressed
byte buffer[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

void setup() 
{
  // put your setup code here, to run once:
  pinMode(LED1,OUTPUT);
  pinMode(STROBE, INPUT);
  for(int i=0;i<8;i++) {
    pinMode(data[i], INPUT);
  }
  Serial.begin(115200);
  noInterrupts();
}

void processSerial()
{
  // read serial byte
  unsigned char k = UDR0;

  if (k == 0)
    return;
    
  //Serial.println(k);
  //Serial.println(bytes[k&0x7f]);
  //Serial.println(bits[k&0x7f]);
  //Serial.println();

  if (k & 0x80) // KeyUp
  {
    k &= 0x7f;
    byte row = bytes[k];
    byte mask = (1<<bits[k]);
    buffer[row] |= mask;
  } else { // KeyDown
    byte row = bytes[k];
    byte mask = (1<<bits[k]) ^ 0xff;
    buffer[row] &= mask;
  }
  
  //for(unsigned char i=0;i<12;i++)
  //  Serial.println(buffer[i],HEX);
  //Serial.println();
}

void loop()
{
  // wait for strobe high
  while ((PINC & 0x01)==0x00);

  led_on();

  // read address bits
  byte addr = PINB & 0x0f;
  if (addr>12) addr=0; // temp

  // lookup row data
  byte data = buffer[addr];

  led_off();

  __builtin_avr_delay_cycles(7*16);  // 7us

  led_on();

  // write data bits
  DDRB |= 0x0f;
  DDRD |= 0xf0;
  PORTB = (PORTB & 0xf0) | (data & 0x0f);
  PORTD = (PORTD & 0x0f) | (data & 0xf0);
  
  led_off();

  // wait for strobe low
  while ((PINC & 0x01)==0x01);
  
  led_on();

  // take data off the bus
  DDRB &= 0xf0;
  DDRD &= 0x0f;

  // LED off
  led_off();
  
  if (UCSR0A & (1<<RXC0)) // if data ready
  {
    led_on();
    processSerial();
  }
  
  led_on();
  led_off();
}
