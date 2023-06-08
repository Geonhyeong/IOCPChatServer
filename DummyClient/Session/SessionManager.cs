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
		object _lock = new object();

		public void SendForEach()
		{
			lock (_lock)
			{
				foreach (ServerSession session in _sessions)
				{
					ChatReqPacket chatPacket = new ChatReqPacket();
					chatPacket.SetValue(session.Nickname, ChatMessageGenerator.GenerateChatMessage());

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
				session.Nickname = RandomNicknameGenerator.GenerateNickname();

				_sessions.Add(session);
				return session;
			}
		}
	}
}
