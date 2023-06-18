using System;

namespace DummyClient
{
    class PacketDef
    {
        public const UInt16 PACKET_HEADER_SIZE = 5;
        public const int MAX_ID_PWD_BYTE_LENGTH = 32;
        public const int MAX_CHAT_MSG_SIZE = 256;

        public static byte[] MakeSendBuffer(PACKET_ID packetId, byte[] bodyData)
        {
            UInt16 packetSize = (UInt16)(bodyData.Length + PACKET_HEADER_SIZE);
            byte[] sendBuffer = new byte[packetSize];

            Array.Copy(BitConverter.GetBytes(packetSize), 0, sendBuffer, 0, sizeof(UInt16));
            Array.Copy(BitConverter.GetBytes((UInt16)packetId), 0, sendBuffer, 2, sizeof(UInt16));
            Array.Copy(BitConverter.GetBytes((Byte)0), 0, sendBuffer, 4, sizeof(Byte));
            Array.Copy(bodyData, 0, sendBuffer, 5, bodyData.Length);

            return sendBuffer;
        }
    }

    public enum PACKET_ID : UInt16
    {
        LOGIN_REQ = 101,
        LOGIN_RES = 102,
        LOGOUT_REQ = 103,
        LOGOUT_RES = 104,

        ROOM_ENTER_REQ = 201,
        ROOM_ENTER_RES = 202,
        ROOM_LEAVE_REQ = 203,
        ROOM_LEAVE_RES = 204,
        ROOM_USER_LIST = 205,

        CHAT_REQ = 303,
        CHAT_RES = 304,
        CHAT_BROADCAST = 305,
    }

    public enum ERROR_CODE : UInt16
    {
        NONE = 0,
        INVALID_PACKET = 1,
        INVALID_SESSION = 2,

        LOGIN_SUCCESS = 30,                 // 로그인 성공
        LOGIN_INVALID_TOKEN = 31,              // 유효하지 않은 Password
        LOGIN_REDUNDANT_CONNECTION = 32,    // 중복 접속
        LOGIN_USED_ALL_OBJ = 33,            // 허용 가능한 유저 수 초과

        ROOM_ENTER_SUCCESS = 50,            // 방 입장 성공
        ROOM_LEAVE_SUCCESS = 51,            // 방 퇴장 성공
        ROOM_UNKNOWN_NUMBER = 52,           // 존재하지 않는 방 숫자
        ROOM_USER_EXCEED = 53,              // 방 최대 인원 수 초과
        ROOM_NONEXIST_USER = 54,			// 방에 존재하지 않는 유저
    }
}
