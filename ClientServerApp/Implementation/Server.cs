using Server.Abstraction;
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Server.Implementation
{
    public class Server : IServer, IServerHandle
    {
        private readonly string hostIpAddress;
        private Socket hostSocket = null;

        private List<Socket> clientSockets = new List<Socket>();

        public Server(string ipAddress)
        {
            this.hostIpAddress = ipAddress;
        }
        
        public string createRoom(string roomName)
        {
            List<Socket> sockets = new List<Socket>();
            int roomNumber = 0;
            string newPort = "8888";

            ClientSockets.Add(new Dictionary<int, List<Socket>>() { 
             
            });
            return newPort;
        }

        public async Task StartListenRoom(int port, int roomNumber)
        {
            Console.WriteLine("Создание комнаты");
            Task newListerTask = new Task(() => {
                Console.WriteLine("Начало прослушивания порта " + port);
                IPEndPoint endPoints = new IPEndPoint(IPAddress.Any, port);
                hostSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                try
                {
                    hostSocket.Bind(endPoints);
                    hostSocket.Listen(100);

                    while (true)
                    {
                        hostSocket.BeginAccept(new AsyncCallback(AcceptCallback), hostSocket);
                    }

                }
                catch (Exception e)
                {
                    Console.WriteLine(e.ToString());
                }
            });
            newListerTask.Start();
        }

        private void CloseAllSockets()
        {
            foreach (Socket socket in clientSockets)
            {
                socket.Shutdown(SocketShutdown.Both);
                socket.Close();
            }
            hostSocket.Close();
        }

        public void AcceptCallback(IAsyncResult ar)
        {
            Console.WriteLine("Подключение нового пользователя");
            Socket socket = null;
            try
            {
                socket = hostSocket.EndAccept(ar);
            }
            catch (ObjectDisposedException er)
            {
                throw er;
            }
            clientSockets.Add(socket);

            int _bufferSize = 256;
            byte[] _buffer = new byte[_bufferSize];

            StateObject stateObject = new StateObject();
            stateObject.buffer = _buffer;
            stateObject.BufferSize = _bufferSize;
            stateObject.workSocket = socket;

            socket.BeginReceive(_buffer, 0, _bufferSize, SocketFlags.None, ReceiveCallback, stateObject);
            hostSocket.BeginAccept(AcceptCallback, null);
        }

        public void ReceiveCallback(IAsyncResult ar)
        {
            StateObject stateObject = (StateObject)ar.AsyncState;
            Socket current = stateObject.workSocket;
            int received;
            try
            {
                received = current.EndReceive(ar);
            }
            catch (SocketException)
            {
                current.Close(); 
                clientSockets.Remove(current);
                return;
            }

            byte[] recBuf = new byte[received];
            Array.Copy(stateObject.buffer, recBuf, received);
            string text = Encoding.ASCII.GetString(recBuf);
            Console.WriteLine("Текст: " + text);

            if (text.ToLower().Contains("[send]"))
            {
                byte[] data = null;
                int startBodyIndex = text.IndexOf('|');
                string messageBody = text.Substring(startBodyIndex, text.Length - startBodyIndex);
                data = System.Text.Encoding.Default.GetBytes(messageBody);
                // TODO: action here
                foreach (var client in clientSockets)
                {
                    if (client.Connected)
                    {
                        client.Send(data);
                    }
                }
            }
            else if (text.ToLower() == "action 2") 
            {
                // TODO: action here
                current.Shutdown(SocketShutdown.Both);
                current.Close();
                clientSockets.Remove(current);
                return;
            }
            else
            {
                byte[] data = Encoding.ASCII.GetBytes("Invalid request");
                current.Send(data);
            }

            stateObject.workSocket = current;
            current.BeginReceive(stateObject.buffer, 0, stateObject.BufferSize, SocketFlags.None, ReceiveCallback, stateObject);
        
        }

        public void Start()
        {
            Console.WriteLine("Запуск сервера");
            StartListenRoom(8888, 1);
        }
    }
}
