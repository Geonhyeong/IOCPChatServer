using System;
using System.Collections.Generic;
using System.Text;

namespace DummyClient
{
	class SessionManager
	{
		static SessionManager _session = new SessionManager();
		public static SessionManager Instance { get { return _session; } }

		List<ServerSession> _sessions = new List<ServerSession>();
		static UInt32 _dummyCount = 0;
		object _lock = new object();
		
		public void LoginForEach()
        {
			lock (_lock)
			{
				foreach (ServerSession session in _sessions)
				{
					LoginReqPacket loginReqPacket = new LoginReqPacket();
					loginReqPacket.SetValue(session.UserId, 0, 0, true);

					byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.LOGIN_REQ, loginReqPacket.ToBytes());
					session.Send(new ArraySegment<byte>(sendBuffer));
				}
			}
		}

		public void EnterRoomForEach()
        {
			lock (_lock)
			{
				foreach (ServerSession session in _sessions)
				{
					RoomEnterReqPacket roomEnterReqPacket = new RoomEnterReqPacket();
					session.RoomNumber = (session.DummyId - 1) / 10 + 1000;
					roomEnterReqPacket.SetValue(session.RoomNumber);

					byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.ROOM_ENTER_REQ, roomEnterReqPacket.ToBytes());
					session.Send(new ArraySegment<byte>(sendBuffer));
				}
			}
		}

		public void SendForEach()
		{
			lock (_lock)
			{
				foreach (ServerSession session in _sessions)
				{
					ChatReqPacket chatPacket = new ChatReqPacket();
					chatPacket.SetValue(ChatMessageGenerator.GenerateChatMessage());

					byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.CHAT_REQ, chatPacket.ToBytes());
					session.Send(new ArraySegment<byte>(sendBuffer));
				}
			}
		}

		public ServerSession Generate()
		{
			lock (_lock)
			{
				ServerSession session = new ServerSession();
				session.UserId = RandomNicknameGenerator.GenerateNickname();
				session.DummyId = ++_dummyCount;

				_sessions.Add(session);
				return session;
			}
		}
	}
}
