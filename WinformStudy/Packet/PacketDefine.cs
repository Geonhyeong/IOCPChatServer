using System;

namespace WinformStudy
{
    class PacketDef
    {
        public const UInt16 PACKET_HEADER_SIZE = 5;
        public const int MAX_CHAT_MSG_SIZE = 256;
        public const int MAX_NICKNAME_BYTE_LENGTH = 32;

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
        LOGIN_REQUEST = 201,
        LOGIN_RESPONSE = 202,

        CHAT_REQ = 303,
        CHAT_RES = 304,
        CHAT_BROADCAST = 305,
    }

    public enum ERROR_CODE : UInt16
    {
        NONE = 0,
        INVALID_PACKET = 1,

        LOGIN_SUCCESS = 30,                 // 로그인 성공
        LOGIN_INVALID_PW = 31,              // 유효하지 않은 Password
        LOGIN_REDUNDANT_CONNECTION = 32,    // 중복 접속
        LOGIN_USED_ALL_OBJ = 33,			// 허용 가능한 유저 수 초과
    }
}
