﻿using System;
using System.Net;
using System.Threading;

namespace DummyClient
{
    class Program
    {
        static void Main(string[] args)
        {
			// DNS (Domain Name System)
			IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 7777);

			Connector connector = new Connector();

			connector.Connect(endPoint,
				() => { return SessionManager.Instance.Generate(); },
				10);

			Console.ReadLine();
			SessionManager.Instance.LoginForEach();
			Console.ReadLine();
			SessionManager.Instance.EnterRoomForEach();
			Console.ReadLine();

			while (true)
			{
				try
				{
					SessionManager.Instance.SendForEach();
				}
				catch (Exception e)
				{
					Console.WriteLine(e.ToString());
				}

				Thread.Sleep(2000);
			}
		}
    }
}
