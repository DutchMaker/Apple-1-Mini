using System;
using System.Diagnostics;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace DataUploader
{
    class Program
    {
        private const int DELAY_CHAR = 30;
        private const int DELAY_LINE = 100;

        private static SerialPort serial;
        private static bool receiving;
        private static Stopwatch duration;

        /// <summary>
        /// Main program logic.
        /// </summary>
        /// <param name="args"></param>
        [STAThread]
        private static void Main(string[] args)
        {
            bool execute = true;

            while (execute)
            {
                Console.Clear();

                // Display boot message and ask user what COM port to use for the serial connection.
                string comPort = DisplayBootMessage();

                // Display file selection dialog.
                Console.WriteLine("Select the file that you would like to upload...");
                var dialog = new OpenFileDialog
                {
                    Title = "Select file to upload..."
                };

                if (dialog.ShowDialog() == DialogResult.OK)
                {
                    Console.WriteLine($"Selected {dialog.FileName}\r\n");
                    Console.WriteLine($"Connecting to {comPort}...");

                    // Create and open serial connection.
                    serial = new SerialPort(comPort, 9600, Parity.None, 8, StopBits.One);
                    serial.DataReceived += Serial_DataReceived;
                    serial.Open();

                    // Wait for firmware to load (Arduino may reset on connect).
                    // This turns out to be necessary for the upload to succeed.
                    Thread.Sleep(2000);
                    SendNewLine();

                    Console.WriteLine("Connected...\r\n");
                    Console.WriteLine($"Uploading data...");

                    duration = Stopwatch.StartNew();
                    int progress = 0;

                    // Read the data from the selected file.
                    using (var reader = new StreamReader(dialog.FileName, Encoding.ASCII))
                    {
                        long total = reader.BaseStream.Length;

                        while (reader.Peek() >= 0)
                        {
                            int data = reader.Read();

                            while (receiving)
                            {
                                // Don't send any data while the Serial_DataReceived eventhandler is busy.
                                Thread.Sleep(10);
                            }

                            if (data < 96) // Only process Apple 1 compatible characters.
                            {
                                if (data != 10 && data != 13
                                ) // Skip CR and LF characters, we'll handle these differently.
                                {
                                    serial.Write(((char) data).ToString());
                                    Thread.Sleep(DELAY_CHAR); // Wait for the Apple 1 to process the input.
                                }
                                else if (data == 13)
                                {
                                    SendNewLine(); // Send a CRLF sequence.
                                }

                                Console.Write((char) data); // Display the data we have sent in the console.
                            }

                            Progress(++progress, total);
                        }
                    }

                    Console.WriteLine("");
                    Console.WriteLine("Finishing...");

                    // Send an extra CRLF and close the connection.
                    SendNewLine();
                    serial.Close();

                    Console.Title = "Upload succeeded!";
                    Console.WriteLine("Upload succeeded!\r\n");
                }
                else
                {
                    Console.WriteLine("No file was selected.\r\n");
                }

                Console.Write("Would you like to upload another file? (Y/N) ");
                execute = Console.ReadLine().Equals("Y", StringComparison.OrdinalIgnoreCase);
            }
        }

        /// <summary>
        /// Display the boot message and prompt user for COM port.
        /// </summary>
        /// <returns></returns>
        private static string DisplayBootMessage()
        {
            Console.WriteLine("+------------------------+");
            Console.WriteLine("|      APPLE 1 MINI      |");
            Console.WriteLine("|------------------------|");
            Console.WriteLine("| DATA UPLOADER UTILITY  |");
            Console.WriteLine("+------------------------+");
            Console.WriteLine("");

            int portNumber = 0;

            while (portNumber == 0)
            {
                Console.Write("What COM port number would you like to use? ");
                
                if (int.TryParse(Console.ReadLine(), out portNumber))
                {
                    Console.WriteLine($"Using COM{portNumber}\r\n");
                    return $"COM{portNumber}";
                }
                else
                {
                    Console.WriteLine("That's not a valid number, try again.");
                }
            }

            return null;
        }

        /// <summary>
        /// Send serial data for CRLF.
        /// </summary>
        private static void SendNewLine()
        {
            serial.Write("\r");
            Thread.Sleep(DELAY_CHAR);
            serial.Write("\n");
            Thread.Sleep(DELAY_LINE);
        }

        /// <summary>
        /// Serial DataReceived event handler.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private static void Serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            receiving = true;
            serial.ReadExisting();
            receiving = false;
        }

        /// <summary>
        /// Update the progress indication (console window title).
        /// </summary>
        /// <param name="progress"></param>
        /// <param name="total"></param>
        private static void Progress(int progress, long total)
        {
            decimal percentage = progress / ((decimal)total / 100);
            int bytesPerSec = (int)(progress / duration.Elapsed.TotalSeconds);

            Console.Title = $"Uploading... ({progress:N0} of {total:N0} bytes - {bytesPerSec} bytes/sec - {percentage:N2}%)";
        }
    }
}
