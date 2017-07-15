#define DA  A0
#define RDA A1
#define PB0 2
#define PB1 3
#define PB2 4
#define PB3 A5
#define PB4 A4
#define PB5 A3
#define PB6 A2

uint8_t video_data = 0;
uint8_t video_data_pins[] = { PB0, PB1, PB2, PB3, PB4, PB5, PB6 };
uint8_t serial_data;

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
  Serial.println("Arduino ready...");
}

void loop() 
{
  process_video_data();
  process_serial_data();  
}

void process_video_data()
{
  digitalWrite(RDA, HIGH);        // Tell PIA we are ready to receive data
  delayMicroseconds(20);

  if (digitalRead(DA))            // Is there any data?
  {
    video_data = 0;

    for (int i = 0; i < 7; i++)  // Yes, get byte data
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

    if (serial_data == 13)
    {
      Serial.println();
    }
    else
    {
      Serial.print((char)serial_data);
    }
  }
}