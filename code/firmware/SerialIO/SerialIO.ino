// ---------------------------------------------------------------------------------- //
// Apple 1 Mini firmware for Arduino                                                  //
//                                                                                    //
// Version 1.0                                                                        //
// By Ruud van Falier                                                                 //
//                                                                                    //
// Enables serial communication between an Arduino and the Apple 1 PIA.               //
// Also implements a blinking cursor and green text using VT100 escape codes          //
// (VT100 codes only tested with PuTTY client!)                                       //
//                                                                                    //
// Heavily based on Propeller source code from Briel Computers' Apple 1 replica.      //
// ---------------------------------------------------------------------------------- //

// Port definitions (Arduino pins connected to the PIA)
#define PIN_DA  A0
#define PIN_RDA A1
#define PIN_PB0 2
#define PIN_PB1 3
#define PIN_PB2 4
#define PIN_PB3 A5
#define PIN_PB4 A4
#define PIN_PB5 A3
#define PIN_PB6 A2
#define PIN_PA0 5
#define PIN_PA1 6
#define PIN_PA2 7
#define PIN_PA3 8
#define PIN_PA4 9
#define PIN_PA5 10
#define PIN_PA6 11
#define PIN_STROBE 12

#define DELAY_VIDEO 1
#define DELAY_ASCII 1

// VT100 codes (used for cursor and text color implementation)
#define VT100_OFF           "\033[0m"     // Disable formatting
#define VT100_DEFAULT       "\033[32m"    // Color green
#define VT100_BLINK         "\033[5;32m"  // Blink color green
#define VT100_BOLD          "\033[1;32m"  // Bold color green
#define VT100_CLS           "\033[2J"     // Clear screen
#define VT100_RESET_CURSOR  "\033[H"      // Position cursor in top left corner
#define VT100_CURSOR_LEFT   "\033[1D"     // Move cursor one column to the left
#define VT100_ERASE_EOL     "\033[K"      // Erase from cursor until end of line
#define CURSOR_CHAR   "@"

// Global variables
uint8_t video_data = 0;
uint8_t video_data_pins[] = { PIN_PB0, PIN_PB1, PIN_PB2, PIN_PB3, PIN_PB4, PIN_PB5, PIN_PB6 };
uint8_t serial_data;

bool cursor_visible = false;

/*
 * Program initialization
 */
void setup() 
{
  // Setup video data pins (output from the PIA)
  pinMode(PIN_RDA, OUTPUT);
  pinMode(PIN_DA, INPUT);
  pinMode(PIN_PB0, INPUT);
  pinMode(PIN_PB1, INPUT);
  pinMode(PIN_PB2, INPUT);
  pinMode(PIN_PB3, INPUT);
  pinMode(PIN_PB4, INPUT);
  pinMode(PIN_PB5, INPUT);
  pinMode(PIN_PB6, INPUT);

  // Setup ASCII data pins (input for the PIA)
  pinMode(PIN_STROBE, OUTPUT);
  pinMode(PIN_PA0, OUTPUT);
  pinMode(PIN_PA1, OUTPUT);
  pinMode(PIN_PA2, OUTPUT);
  pinMode(PIN_PA3, OUTPUT);
  pinMode(PIN_PA4, OUTPUT);
  pinMode(PIN_PA5, OUTPUT);
  pinMode(PIN_PA6, OUTPUT);

  Serial.begin(9600);
 
  display_boot_message();
}

/*
 * Program loop
 */
void loop() 
{
  process_video_data();
  process_serial_data(); 
}

/*
 * Displays the Apple 1 Replica firmware boot message and enables the blinking cursor
 */
void display_boot_message()
{
  cursor_visible = false;

  Serial.print(VT100_CLS);
  Serial.print(VT100_RESET_CURSOR);
  Serial.print(VT100_OFF);
  Serial.print(VT100_DEFAULT);
  Serial.println("+-----------------------+");
  Serial.print("|     ");
  Serial.print(VT100_OFF);
  Serial.print(VT100_BOLD);
  Serial.print("APPLE 1 MINI");
  Serial.print(VT100_OFF);
  Serial.print(VT100_DEFAULT);
  Serial.println("      |");
  Serial.print(VT100_OFF);
  Serial.print(VT100_DEFAULT);
  Serial.println("|-----------------------|");
  Serial.println("| FIRMWARE VERSION 1.0  |");
  Serial.println("| RUUD VAN FALIER, 2017 |");
  Serial.println("+-----------------------+");
  Serial.println();
  Serial.println("READY...");
  Serial.println();
  Serial.print(VT100_OFF);

  show_cursor();
}

/*
 * Retrieves video (character) data from the PIA and sends it to the user over the serial connection.
 */
void process_video_data()
{
  // Tell the PIA we are ready to receive data.
  digitalWrite(PIN_RDA, HIGH);
  delay(DELAY_VIDEO);

  while (digitalRead(PIN_DA))
  {
    // Data is available.
    video_data = 0;

    // Decode the data bits to a byte.
    for (int i = 0; i < 7; i++)
    { 
      if (digitalRead(video_data_pins[i]))
      {
        video_data |= (1 << i);
      }
    }

    // Send video data over serial connection.
    if (video_data == 13)
    {
      // Carrage return.
      hide_cursor();
      Serial.println();
    }
    else if (video_data > 31)
    {
      // Display-compatible character.
      hide_cursor();
      Serial.print(VT100_DEFAULT);
      Serial.print((char)video_data);
      Serial.print(VT100_OFF);
    }

    // Done receiving this byte.
    digitalWrite(PIN_RDA, LOW);
    delay(DELAY_VIDEO);

    // Tell the PIA we are ready to receive more data.
    digitalWrite(PIN_RDA, HIGH);
    delay(DELAY_VIDEO);
  }

  show_cursor();
}

/*
 * Processes data received from the serial connection.
 * Puts the received data on to the PIA where the CPU will read it from.
 */
void process_serial_data()
{
  // Wait for serial data coming in.
  while (Serial.available() > 0)
  {
    serial_data = Serial.read();

    if (serial_data == 203) 
    {
      // Translate uppercase ESC to normal ESC.
      serial_data = 27;
    }

    if (serial_data >= 97 && serial_data <= 122)
    {
      // Translate lowercase characters to uppercase as the Apple 1 only supports uppercase.
      serial_data -= 32;
    }

    if (serial_data < 96)   
    {
      // Encode any Apple 1 compatible character to data bits.
      digitalWrite(PIN_PA6, bitRead(serial_data, 6));
      digitalWrite(PIN_PA5, bitRead(serial_data, 5));
      digitalWrite(PIN_PA4, bitRead(serial_data, 4));
      digitalWrite(PIN_PA3, bitRead(serial_data, 3));
      digitalWrite(PIN_PA2, bitRead(serial_data, 2));
      digitalWrite(PIN_PA1, bitRead(serial_data, 1));
      digitalWrite(PIN_PA0, bitRead(serial_data, 0));

      // Pulse the STROBE bit so the PIA will process the input.
      digitalWrite(PIN_STROBE, HIGH);
      delay(DELAY_ASCII);
      digitalWrite(PIN_STROBE, LOW);
    }

    process_video_data();
  }
}

/*
 * Remove the cursor character from the screen by sending a backspace.
 */
void hide_cursor()
{
  if (cursor_visible)
  {
    Serial.print(VT100_CURSOR_LEFT);
    Serial.print(VT100_ERASE_EOL);
    cursor_visible = false;
  }
}

/*
 * Display blinking cursor character.
 */
void show_cursor()
{
  if (!cursor_visible)
  {
    Serial.print(VT100_BLINK);
    Serial.print(CURSOR_CHAR);
    Serial.print(VT100_OFF);
    cursor_visible = true;
  }
}