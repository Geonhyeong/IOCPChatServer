using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace WinformStudy
{
    class Connector
    {
        private Func<Session> _sessionFactory;

        public void Connect(string ip, int port, Func<Session> sessionFactory, int count = 1)
        {
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse(ip), port);

            for (int i = 0; i < count; i++)
            {
                Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
                _sessionFactory = sessionFactory;

                SocketAsyncEventArgs args = new SocketAsyncEventArgs();
                args.Completed += OnConnectCompleted;
                args.RemoteEndPoint = endPoint;
                args.UserToken = socket;

                RegisterConnect(args);

                // TEMP
                Thread.Sleep(10);
            }
        }

        private void RegisterConnect(SocketAsyncEventArgs args)
        {
            Socket socket = args.UserToken as Socket;
            if (socket == null)
                return;

            try
            {
                bool pending = socket.ConnectAsync(args);
                if (pending == false)
                    OnConnectCompleted(null, args);
            }
            catch (Exception e)
            {
                DevLog.Write($"Socket_{socket}: {e.Message}", LOG_LEVEL.ERROR);
            }
        }

        private void OnConnectCompleted(object sender, SocketAsyncEventArgs args)
        {
            try
            {
                if (args.SocketError == SocketError.Success)
                {
                    Session session = _sessionFactory.Invoke();
                    session.Start(args.ConnectSocket);
                    session.OnConnected(args.RemoteEndPoint);
                }
                else
                {
                    DevLog.Write(args.SocketError.ToString(), LOG_LEVEL.ERROR);
                }
            }
            catch (Exception e)
            {
                DevLog.Write(e.Message, LOG_LEVEL.ERROR);
            }
        }
    }
}