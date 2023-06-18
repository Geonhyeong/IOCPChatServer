using System;
using System.Collections.Generic;
using System.Text;
using System.Net;

namespace DummyClient
{
	class ServerSession : PacketSession
	{
		public string UserId { get; set; }
		public UInt32 DummyId { get; set; }
		public UInt32 RoomNumber { get; set; }

		public override void OnConnected(EndPoint endPoint)
		{
			Console.WriteLine($"OnConnected : {endPoint}");
		}

		public override void OnDisconnected(EndPoint endPoint)
		{
			Console.WriteLine($"OnDisconnected : {endPoint}");
		}

		public override void OnRecvPacket(ArraySegment<byte> buffer)
		{
			PacketManager.Instance.OnRecvPacket(this, buffer);
		}

		public override void OnSend(int numOfBytes)
		{
			//Console.WriteLine($"Transferred bytes: {numOfBytes}");
		}
	}
}
