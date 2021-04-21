using System;

namespace ClientServerApp
{
    class Program
    {
        static void Main(string[] args)
        {
            Server.Implementation.Server server = new Server.Implementation.Server("127.0.0.1");
            server.Start();
            Console.ReadKey();
        }
    }
}
