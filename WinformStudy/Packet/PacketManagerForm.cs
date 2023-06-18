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
            PacketFuncDict.Add(PACKET_ID.LOGIN_RES, Process_LoginRes);
            PacketFuncDict.Add(PACKET_ID.LOGOUT_RES, Process_LogoutRes);
            PacketFuncDict.Add(PACKET_ID.ROOM_ENTER_RES, Process_RoomEnterRes);
            PacketFuncDict.Add(PACKET_ID.ROOM_LEAVE_RES, Process_RoomLeaveRes);
            PacketFuncDict.Add(PACKET_ID.ROOM_USER_LIST, Process_RoomUserList);
            PacketFuncDict.Add(PACKET_ID.CHAT_RES, Process_ChatRes);
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
        private void Process_LoginRes(byte[] bodyData)
        {
            LoginResPacket loginResPacket = new LoginResPacket();
            loginResPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)loginResPacket.Result;
            if (errCode == ERROR_CODE.LOGIN_SUCCESS)
            {
                DevLog.Write("클라이언트 로그인 성공!", LOG_LEVEL.INFO);
            }
            else
            {
                DevLog.Write($"로그인 요청 실패: {errCode}", LOG_LEVEL.ERROR);
            }
        }

        private void Process_LogoutRes(byte[] bodyData)
        {
            LogoutResPacket logoutResPacket = new LogoutResPacket();
            logoutResPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)logoutResPacket.Result;
            if (errCode == ERROR_CODE.NONE)
            {
                DevLog.Write("클라이언트 로그아웃.", LOG_LEVEL.INFO);
            }
            else
            {
                DevLog.Write($"로그아웃 요청 실패: {errCode}", LOG_LEVEL.ERROR);
            }
        }

        private void Process_RoomEnterRes(byte[] bodyData)
        {
            RoomEnterResPacket resPacket = new RoomEnterResPacket();
            resPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)resPacket.Result;
            if (errCode == ERROR_CODE.ROOM_ENTER_SUCCESS)
            {
                DevLog.Write("방 입장 성공!", LOG_LEVEL.INFO);
            }
            else
            {
                DevLog.Write($"방 입장 요청 실패: {errCode}", LOG_LEVEL.ERROR);
            }
        }

        private void Process_RoomLeaveRes(byte[] bodyData)
        {
            RoomLeaveResPacket resPacket = new RoomLeaveResPacket();
            resPacket.FromBytes(bodyData);

            var errCode = (ERROR_CODE)resPacket.Result;
            if (errCode == ERROR_CODE.ROOM_LEAVE_SUCCESS)
            {
                DevLog.Write("방 퇴장.", LOG_LEVEL.INFO);
            }
            else
            {
                DevLog.Write($"방 퇴장 요청 실패: {errCode}", LOG_LEVEL.ERROR);
            }
        }

        private void Process_RoomUserList(byte[] bodyData)
        {
            RoomUserListPacket packet = new RoomUserListPacket();
            packet.FromBytes(bodyData);

            UserList.Clear();
            foreach (string userId in packet.UserList)
                UserList.Add(userId);

            DevLog.Write($"방 인원 목록 갱신! {packet.UserCount} 명 업데이트.", LOG_LEVEL.INFO);
        }

        private void Process_ChatRes(byte[] bodyData)
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

            string msg = $"{chatBroadcastPacket.UserId} > {chatBroadcastPacket.Msg}";
            RoomChatMsgQueue.Enqueue(msg);
        }
#endregion
    }
}
