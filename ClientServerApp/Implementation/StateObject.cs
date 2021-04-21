using System.Net.Sockets;
using System.Text;

namespace Server.Implementation
{
    public class StateObject
    {
        public Socket workSocket;
        public int BufferSize;
        public byte[] buffer;
        public StringBuilder sb = new StringBuilder();
    }
}
