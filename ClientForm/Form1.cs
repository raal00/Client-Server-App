using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ClientForm
{
    public partial class Form1 : Form
    {

        private Socket socket { get; set; }
        private DateTime? connectionDate { get; set; }
        private Logger logger { get; set; }

        public Form1()
        {
            InitializeComponent();
            logger = new Logger();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            timeBtn.Enabled = false;
            usersBtn.Enabled = false;
        }

        private void connect_Click(object sender, EventArgs e)
        {
            if (socket == null || !socket.Connected)
            {
                socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                string serverIp = serverIpBox.Text;
                string port = portBox.Text;

                if (string.IsNullOrEmpty(serverIp))
                {
                    MessageBox.Show("Введите адрес сервера", "Ошибка!", MessageBoxButtons.OK);
                    return;
                }
                if (string.IsNullOrEmpty(port))
                {
                    MessageBox.Show("Введите порт", "Ошибка!", MessageBoxButtons.OK);
                    return;
                }

                IPAddress address = null;
                if (IPAddress.TryParse(serverIp, out address))
                {
                    int portAsInt;
                    if (int.TryParse(port, out portAsInt))
                    {
                        try
                        {
                            socket.Connect(address, portAsInt);
                            int receiveBufferSize = 1028;
                            byte[] receivebuffer = new byte[receiveBufferSize];
                            int bytes = socket.Receive(receivebuffer);
                            logBox.Text += $"\n{Encoding.Default.GetString(receivebuffer, 0, bytes)}";
                            connectionDate = DateTime.Now; 
                            connect.Text = "Отключиться";
                            timeBtn.Enabled = true;
                            usersBtn.Enabled = true;
                        }
                        catch (Exception er)
                        {
                            MessageBox.Show(er.Message, "Ошибка!", MessageBoxButtons.OK);
                        }
                    }
                    else
                    {
                        MessageBox.Show("Порт имеет неправильную форму", "Ошибка!", MessageBoxButtons.OK);
                        return;
                    }

                }
                else
                {
                    MessageBox.Show("Адрес имеет неправильную форму", "Ошибка!", MessageBoxButtons.OK);
                    return;
                }
            }
            else
            {
                try
                {
                    socket.Disconnect(false);
                    connect.Text = "Подключиться";
                    timeBtn.Enabled = false;
                    usersBtn.Enabled = false;
                }
                catch (Exception er)
                {
                    MessageBox.Show(er.Message, "Ошибка!", MessageBoxButtons.OK);
                    return;
                }
            }
        }

        private void sendMsgBtn_Click(object sender, EventArgs e)
        {
            if (socket != null && socket.Connected)
            {
                string message = inputTextBox.Text;
                if (string.IsNullOrEmpty(message)) return;
                byte[] buffer = System.Text.Encoding.Default.GetBytes(message);
                try
                {
                    logBox.Text += "\nОтправка сообщения..";
                    socket.Send(buffer);

                    logBox.Text += "\nПолучение ответа..";
                    int receiveBufferSize = 1024;
                    byte[] receivebuffer = new byte[receiveBufferSize];
                    StringBuilder response = new StringBuilder();

                    do
                    {
                        int bytes = socket.Receive(receivebuffer);
                        response.Append(Encoding.Default.GetString(receivebuffer, 0, bytes));
                    }
                    while (socket.Available > 0);
                    logBox.Text += "\nОтвет получен";

                    IPEndPoint remoteIpEndPoint = socket.RemoteEndPoint as IPEndPoint;

                    TestMessageBox.Text += $"\nОтвет от {remoteIpEndPoint.Address}: \n{response.ToString()}";
                    inputTextBox.Text = string.Empty;

                    if (logger != null && connectionDate.HasValue)
                    {
                        logger.WriteConnectionInfoToFileAsync(socket, response.ToString(), connectionDate.Value, DateTime.Now);
                    }
                }
                catch (Exception er)
                {
                    MessageBox.Show(er.Message, "Ошибка!", MessageBoxButtons.OK);
                    return;
                }
            }
            else
            {
                MessageBox.Show("Необходимо подключиться к серверу", "Ошибка!", MessageBoxButtons.OK);
                return;
            }
        }
        private void sendCommand(string command)
        {
            if (socket != null && socket.Connected)
            {
                logBox.Text += $"\nВыполнение команды {command}..";
                byte[] buffer = System.Text.Encoding.Default.GetBytes(command);
                socket.Send(buffer);
                int receiveBufferSize = 1024;
                byte[] receivebuffer = new byte[receiveBufferSize];
                StringBuilder response = new StringBuilder();

                do
                {
                    int bytes = socket.Receive(receivebuffer);
                    response.Append(Encoding.Default.GetString(receivebuffer, 0, bytes));
                }
                while (socket.Available > 0);
                logBox.Text += $"\nКоманда {command} исполнена";
                TestMessageBox.Text += $"\nОтвет сервера:\n{response.ToString()}";
            }
        }
        private void usersBtn_Click(object sender, EventArgs e)
        {
            sendCommand("#users");
        }
        private void timeBtn_Click(object sender, EventArgs e)
        {
            sendCommand("#time");
        }
    }
}
