using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace Server.Abstraction
{
    interface IServer
    {
        void Start();
        Task StartListenRoom(int port, int roomNumber);
        string createRoom(string roomName);
    }
}
