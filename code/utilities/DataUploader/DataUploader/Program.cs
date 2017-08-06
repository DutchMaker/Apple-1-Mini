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
        private const string PORT = "COM5";
        private const int DELAY_CHAR = 30;
        private const int DELAY_LINE = 100;

        private static SerialPort serial = new SerialPort(PORT, 9600, Parity.None, 8, StopBits.One);
        private static bool receiving;
        private static Stopwatch duration;

        [STAThread]
        static void Main(string[] args)
        {
            var dialog = new OpenFileDialog();

            dialog.Title = "Select file to upload...";

            if (dialog.ShowDialog() == DialogResult.OK)
            {
                serial.DataReceived += Serial_DataReceived;
                serial.Open();

                Console.WriteLine("Connected...");
                Console.WriteLine($"Uploading {dialog.FileName}");

                // Wait for firmware to load (Arduino may reset on connect).
                Thread.Sleep(2000);

                // Clear the buffer
                SendNewLine();

                duration = Stopwatch.StartNew();

                using (var reader = new StreamReader(dialog.FileName, Encoding.ASCII))
                {
                    int progress = 0;
                    long total = reader.BaseStream.Length;

                    while (reader.Peek() >= 0)
                    {
                        int data = reader.Read();

                        // Don't send any data if we are busy receiving
                        while (receiving)
                        {
                            Thread.Sleep(10);
                        }

                        if (data < 96)
                        {
                            if (data != 10 && data != 13)
                            {
                                serial.Write(((char) data).ToString());
                                Thread.Sleep(DELAY_CHAR);
                            }
                            else if (data == 13)
                            {
                                SendNewLine();
                            }

                            Console.Write((char) data);
                        }
                        else
                        {
                            Debug.Write(string.Format("[{0}]", data));
                        }
                        
                        Progress(++progress, total);
                    }
                }

                Console.WriteLine("");
                Console.WriteLine("Finishing...");

                SendNewLine();
                
                serial.Close();

                Console.Title = "Upload succeeded!";
                Console.WriteLine("\r\nDone! Connection closed.");
            }

            Console.ReadKey();
        }

        private static void SendNewLine()
        {
            serial.Write("\r");
            Thread.Sleep(DELAY_CHAR);
            serial.Write("\n");
            Thread.Sleep(DELAY_LINE);
        }

        private static void Serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            receiving = true;
            serial.ReadExisting();
            receiving = false;
        }

        private static void Progress(int progress, long total)
        {
            decimal percentage = progress / ((decimal)total / 100);
            int bytesPerSec = (int)(progress / duration.Elapsed.TotalSeconds);

            Console.Title = $"Uploading... ({progress:N0} of {total:N0} bytes - {bytesPerSec} bytes/sec - {percentage:N2}%)";
        }
    }
}
