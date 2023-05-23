using System;
using System.ComponentModel;
using System.Net;
using System.Threading;

namespace WinformStudy
{
    class DummySession : PacketSession
    {
        public bool AutoSend = false;
        private string _nickname = "";
        Random random = new Random();
        BackgroundWorker worker = new BackgroundWorker();

        public override void OnConnected(EndPoint endPoint)
        {
            _nickname = RandomNicknameGenerator.GenerateNickname();

            //DevLog.Write($"Dummy({_nickname}) 서버 연결 완료.", LOG_LEVEL.INFO);

            worker.DoWork += new DoWorkEventHandler(Work_Send);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            //DevLog.Write($"Dummy({_nickname}) 서버 접속 끊김.", LOG_LEVEL.INFO);
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
        }

        public override void OnSend(int numOfBytes)
        {
        }

        public void StartAutoChat()
        {
            if (AutoSend == false)
            {
                if (worker.IsBusy == false)
                    worker.RunWorkerAsync();
                AutoSend = true;
            }
        }

        private void Work_Send(object sender, DoWorkEventArgs e)
        {
            while (AutoSend)
            {
                ChatReqPacket chatPacket = new ChatReqPacket();
                chatPacket.SetValue(_nickname, ChatMessageGenerator.GenerateChatMessage());

                byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.CHAT_REQ, chatPacket.ToBytes());
                Send(new ArraySegment<byte>(sendBuffer));

                Thread.Sleep(random.Next(3, 6) * 1000);
            }
        }
    }

    class HostSession : PacketSession
    {
        public override void OnConnected(EndPoint endPoint)
        {
            DevLog.Write($"Host 클라이언트 서버 접속 성공!", LOG_LEVEL.INFO);
        }

        public override void OnDisconnected(EndPoint endPoint)
        {
            DevLog.Write($"Host 클라이언트 서버 접속 끊김.", LOG_LEVEL.INFO);
        }

        public override void OnRecvPacket(ArraySegment<byte> buffer)
        {
            //DevLog.Write($"Host 클라이언트 패킷 수신 {buffer.Count} bytes", LOG_LEVEL.INFO);

            var packet = new PacketData();
            packet.packetSize = BitConverter.ToUInt16(buffer.Array, buffer.Offset);
            packet.packetId= BitConverter.ToUInt16(buffer.Array, buffer.Offset + 2);
            packet.type = (Byte)buffer.Array[(buffer.Offset + 4)];
            packet.bodyData = new byte[packet.packetSize - PacketDef.PACKET_HEADER_SIZE];
            Buffer.BlockCopy(buffer.Array, buffer.Offset + PacketDef.PACKET_HEADER_SIZE, packet.bodyData, 0, packet.packetSize - PacketDef.PACKET_HEADER_SIZE);

            Form1.ProcessPacket(packet);
        }

        public override void OnSend(int numOfBytes)
        {
            throw new NotImplementedException();
        }
    }
}
