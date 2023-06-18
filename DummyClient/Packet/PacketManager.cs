using System;
using System.Collections.Generic;
using System.Text;

namespace DummyClient
{
    class PacketManager
    {
        #region Singleton
        static PacketManager _instance = new PacketManager();
        public static PacketManager Instance { get { return _instance; } }
		#endregion

		PacketManager()
		{
			Register();
		}

		Dictionary<PACKET_ID, Action<PacketSession, byte[]>> PacketFuncDict = new Dictionary<PACKET_ID, Action<PacketSession, byte[]>>();

		public void Register()
		{
			PacketFuncDict.Add(PACKET_ID.LOGIN_RES, Process_LoginRes);
			PacketFuncDict.Add(PACKET_ID.ROOM_ENTER_RES, Process_RoomEnterRes);
		}

		public void OnRecvPacket(PacketSession session, ArraySegment<byte> buffer)
		{
			var packet = new PacketData();
			packet.packetSize = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
			packet.packetId = BitConverter.ToUInt16(buffer.Array, buffer.Offset + 2);
			packet.type = (Byte)buffer.Array[(buffer.Offset + 4)];
			packet.bodyData = new byte[packet.packetSize - PacketDef.PACKET_HEADER_SIZE];
			Buffer.BlockCopy(buffer.Array, buffer.Offset + PacketDef.PACKET_HEADER_SIZE, packet.bodyData, 0, packet.packetSize - PacketDef.PACKET_HEADER_SIZE);

			var packetType = (PACKET_ID)packet.packetId;

			if (PacketFuncDict.ContainsKey(packetType))
				PacketFuncDict[packetType](session, packet.bodyData);
		}

        private void Process_LoginRes(PacketSession session, byte[] bodyData)
        {
            ServerSession serverSession = session as ServerSession;

            LoginResPacket loginResPacket = new LoginResPacket();
            loginResPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)loginResPacket.Result;
            if (errCode != ERROR_CODE.LOGIN_SUCCESS)
                Console.WriteLine($"Dummy_{serverSession.DummyId} Login FAIL. {errCode}");
        }

        private void Process_RoomEnterRes(PacketSession session, byte[] bodyData)
        {
            ServerSession serverSession = session as ServerSession;

            RoomEnterResPacket resPacket = new RoomEnterResPacket();
            resPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)resPacket.Result;
            if (errCode != ERROR_CODE.ROOM_ENTER_SUCCESS)
            {
                Console.WriteLine($"Dummy_{serverSession.DummyId} : Room_{serverSession.RoomNumber} Enter FAIL. {errCode}");
            }
        }
    }
}
