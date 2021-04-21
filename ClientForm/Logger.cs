using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Net;

namespace ClientForm
{
    public class Logger
    {
        public async void WriteConnectionInfoToFileAsync(Socket serverSocket, string text, DateTime connectionDate, DateTime sendDate)
        {
            string fileName = DateTime.Now.ToShortDateString() + ".log";
            StringBuilder logBuilder = new StringBuilder();
            logBuilder.Append("Время отправки: ").Append(sendDate.ToShortTimeString()).Append("\n")
                      .Append("Адрес сервера: ").Append((serverSocket.RemoteEndPoint as IPEndPoint).Address).Append("\n")
                      .Append("Время подключения: ").Append(connectionDate.ToShortTimeString()).Append("\n")
                      .Append("Сообщение:\n").Append(text).Append("\n");

            using (StreamWriter writer = new StreamWriter(fileName, true, System.Text.Encoding.Default))
            {
                await writer.WriteAsync(logBuilder.ToString());
            }
        }
    }
}
