#define DA  A0
#define RDA A1
#define PB0 2
#define PB1 3
#define PB2 4
#define PB3 A5
#define PB4 A4
#define PB5 A3
#define PB6 A2
#define PA0 5
#define PA1 6
#define PA2 7
#define PA3 8
#define PA4 9
#define PA5 10
#define PA6 11
#define STROBE 12

uint8_t video_data = 0;
uint8_t video_data_pins[] = { PB0, PB1, PB2, PB3, PB4, PB5, PB6 };
uint8_t serial_data;

void setup() 
{
  // Video data pins
  pinMode(RDA, OUTPUT);
  pinMode(DA, INPUT);
  pinMode(PB0, INPUT);
  pinMode(PB1, INPUT);
  pinMode(PB2, INPUT);
  pinMode(PB3, INPUT);
  pinMode(PB4, INPUT);
  pinMode(PB5, INPUT);
  pinMode(PB6, INPUT);

  // ASCII data pins
  pinMode(STROBE, OUTPUT);
  pinMode(PA0, OUTPUT);
  pinMode(PA1, OUTPUT);
  pinMode(PA2, OUTPUT);
  pinMode(PA3, OUTPUT);
  pinMode(PA4, OUTPUT);
  pinMode(PA5, OUTPUT);
  pinMode(PA6, OUTPUT);

  Serial.begin(9600);
  Serial.println("Arduino ready...");
}

void loop() 
{
  process_video_data();
  process_serial_data();  
}

void process_video_data()
{
  digitalWrite(RDA, HIGH);
  delayMicroseconds(20);

  if (digitalRead(DA))
  {
    video_data = 0;

    for (int i = 0; i < 7; i++)
    { 
      if (digitalRead(video_data_pins[i]))
      {
        video_data |= (1 << i);
      }
    }

    if (video_data == 13)
    {
      Serial.println();
    }
    else if (video_data > 31)
    {
      Serial.print((char)video_data);
    }

    digitalWrite(RDA, LOW);
    delayMicroseconds(20);
  }
}

void process_serial_data()
{
  if (Serial.available() > 0) 
  {
    serial_data = Serial.read();

    if (serial_data == 203) // Uppercase ESC
    {
      serial_data = 27;
    }

    if (serial_data >= 97 && serial_data <= 122)  // lower case a-z
    {
      serial_data -= 32;
    }

    if (serial_data < 96)   // Is the character Apple 1 compatible?
    {      
      digitalWrite(PA6, bitRead(serial_data, 6));
      digitalWrite(PA5, bitRead(serial_data, 5));
      digitalWrite(PA4, bitRead(serial_data, 4));
      digitalWrite(PA3, bitRead(serial_data, 3));
      digitalWrite(PA2, bitRead(serial_data, 2));
      digitalWrite(PA1, bitRead(serial_data, 1));
      digitalWrite(PA0, bitRead(serial_data, 0));

      digitalWrite(STROBE, HIGH);
      delayMicroseconds(20);
      digitalWrite(STROBE, LOW);
    }

    /*
    if (serial_data == 13)
    {
      Serial.println();
    }
    else
    {
      Serial.print((char)serial_data);
    }
    */
  }
}