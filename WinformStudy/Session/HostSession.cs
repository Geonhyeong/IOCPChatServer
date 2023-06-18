using System;
using System.ComponentModel;
using System.Net;
using System.Threading;

namespace WinformStudy
{
    class HostSession : PacketSession
    {
        public string UserId { get; set; } = "";

        public override void OnConnected(EndPoint endPoint)
        {
            DevLog.Write($"클라이언트 서버 접속 성공!", LOG_LEVEL.INFO);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            DevLog.Write($"클라이언트 서버 접속 끊김.", LOG_LEVEL.INFO);
            UserId = "";
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            DevLog.Write($"클라이언트 패킷 수신 {buffer.Count} bytes", LOG_LEVEL.INFO);

            var packet = new PacketData();
            packet.packetSize = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
            packet.packetId = BitConverter.ToUInt16(buffer.Array, buffer.Offset + 2);
            packet.type = (Byte)buffer.Array[(buffer.Offset + 4)];
            packet.bodyData = new byte[packet.packetSize - PacketDef.PACKET_HEADER_SIZE];
            Buffer.BlockCopy(buffer.Array, buffer.Offset + PacketDef.PACKET_HEADER_SIZE, packet.bodyData, 0, packet.packetSize - PacketDef.PACKET_HEADER_SIZE);

            Form1.ProcessPacket(packet);
        }

        public override void OnSend(int numOfBytes)
        {
            DevLog.Write($"클라이언트 {numOfBytes} bytes 전송.", LOG_LEVEL.INFO);
        }
    }
}
