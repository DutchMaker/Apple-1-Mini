using System;
using System.IO;
using System.IO.Ports;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace Uploader
{
    class Program
    {
        private const string PORT = "COM7";
        private static SerialPort serial = new SerialPort(PORT, 9600, Parity.None, 8, StopBits.One);

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
                
                using (var reader = new StreamReader(dialog.FileName, Encoding.ASCII))
                {
                    int progress = 0;
                    long total = reader.BaseStream.Length;

                    while (reader.Peek() >= 0)
                    {
                        int data = reader.Read();
                        
                        if (data < 96)
                        {
                            serial.Write(new byte[] { (byte)data }, 0, 1);

                            Thread.Sleep(50);

                            if (data == 13)
                            {
                                Thread.Sleep(150);
                            }

                            Console.Write((char)data);
                        }
                        
                        Progress(++progress, total);
                    }
                }
                
                serial.WriteLine("");
                serial.Close();

                Console.Title = "Upload succeeded!";
                Console.WriteLine("\r\nDone! Connection closed.");
            }

            Console.ReadKey();
        }

        private static void Serial_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            serial.ReadExisting();
        }

        private static void Progress(int progress, long total)
        {
            decimal percentage = progress / ((decimal)total / 100);

            Console.Title = $"Uploading... ({progress:N0} of {total:N0} bytes - {percentage:N2}%)";
        }
    }
}
