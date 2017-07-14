#define DA  A0
#define RDA A1
#define PB0 2
#define PB1 A7
#define PB2 A6
#define PB3 A5
#define PB4 A4
#define PB5 A3
#define PB6 A2

byte data = 0;
byte data_pins[] = { PB0, PB1, PB2, PB3, PB4, PB5, PB6 };

void setup() 
{
  pinMode(DA, INPUT);
  pinMode(RDA, OUTPUT);
  pinMode(PB0, INPUT);
  pinMode(PB1, INPUT);
  pinMode(PB2, INPUT);
  pinMode(PB3, INPUT);
  pinMode(PB4, INPUT);
  pinMode(PB5, INPUT);
  pinMode(PB6, INPUT);

  Serial.begin(9600);
}

void loop() 
{
  digitalWrite(RDA, HIGH);        // Tell PIA we are ready to receive data

  if (digitalRead(DA))            // Is there any data?
  {
    data = 0;

    for (byte i = 0; i < 7; i++)  // Yes, get byte data
    {
      if (digitalRead(data_pins[i]))
      {
        data |= (1 << i);
      }
    }

    // Output received data to serial terminal.
    Serial.print("Received: ");
    Serial.print(data);
    Serial.print("\t");


    for (int i = 7; i >= 0; i--)
    {
      Serial.print(digitalRead(data_pins[i]));
    }

    Serial.print("\t");
    Serial.println((char)data);

    digitalWrite(RDA, LOW);
    delay(12);
  }
}