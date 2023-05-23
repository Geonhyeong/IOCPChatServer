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
            PacketFuncDict.Add(PACKET_ID.CHAT_RES, Process_ChatResponse);
            PacketFuncDict.Add(PACKET_ID.CHAT_BROADCAST, Process_ChatBroadcast);
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
        private void Process_ChatResponse(byte[] bodyData)
        {
            ChatResPacket chatResPacket = new ChatResPacket();
            chatResPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)chatResPacket.Result;
            if (errCode == ERROR_CODE.NONE)
            {
                long delayTicks = DateTime.Now.Ticks - chatResPacket.RequestTimeTick;
                long delayTime = delayTicks / TimeSpan.TicksPerMillisecond;

                DelayTimeQueue.Enqueue(delayTime);

                DevLog.Write($"클라이언트의 채팅응답 패킷 수신", LOG_LEVEL.INFO);
            }
            else
            {
                DevLog.Write($"채팅 요청 결과: {errCode}", LOG_LEVEL.ERROR);
            }
        }

        private void Process_ChatBroadcast(byte[] bodyData)
        {
            ChatBroadcastPacket chatBroadcastPacket = new ChatBroadcastPacket();
            chatBroadcastPacket.FromBytes(bodyData);

            string msg = $"{chatBroadcastPacket.Nickname} > {chatBroadcastPacket.Msg}";
            RoomChatMsgQueue.Enqueue(msg);
        }
#endregion
    }
}
