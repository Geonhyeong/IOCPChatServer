using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinformStudy
{
    public partial class Form1
    {
        static Dictionary<PACKET_ID, Action<byte[]>> PacketFuncDict = new Dictionary<PACKET_ID, Action<byte[]>>();

        void InitPacketHandler()
        {
            PacketFuncDict.Add(PACKET_ID.S_CHAT, Process_Chat);
            PacketFuncDict.Add(PACKET_ID.DELAY_CHECK, Process_DelayCheck);
        }

        public static void ProcessPacket(PacketData packet)
        {
            var packetType = (PACKET_ID)packet.packetId;

            if (PacketFuncDict.ContainsKey(packetType))
                PacketFuncDict[packetType](packet.bodyData);
            else
                DevLog.Write("Unknown Packet Id: " + packet.packetId.ToString(), LOG_LEVEL.WARN);
        }

#region PACKET HANDLER FUNCTION
        private void Process_Chat(byte[] bodyData)
        {
            S_ChatPacket chatPacket = new S_ChatPacket();
            chatPacket.FromBytes(bodyData);

            string msg = $"{chatPacket.Nickname} > {chatPacket.Msg}";
            RoomChatMsgQueue.Enqueue(msg);
        }

        private void Process_DelayCheck(byte[] bodyData)
        {
            DelayCheckPacket delayCheckPacket = new DelayCheckPacket();
            delayCheckPacket.FromBytes(bodyData);

            long delayTicks = DateTime.Now.Ticks - delayCheckPacket.CurrentTimeSpan;
            delayTime = delayTicks / TimeSpan.TicksPerMillisecond;

            DevLog.Write("DelayCheck 패킷 수신.", LOG_LEVEL.INFO);
        }
#endregion
    }
}
