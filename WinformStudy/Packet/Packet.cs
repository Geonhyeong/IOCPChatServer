using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinformStudy
{
    public struct PacketData
    {
        public UInt16 packetSize;
        public UInt16 packetId;
        public Byte type;
        public byte[] bodyData;
    }

    public class ChatReqPacket
    {
        byte[] Nickname = new byte[PacketDef.MAX_NICKNAME_BYTE_LENGTH];
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];
        byte[] RequestTimeTick = new byte[sizeof(Int64)];

        public void SetValue(string nickname, string msg, Int64 timeSpan = 0)
        {
            Encoding.UTF8.GetBytes(nickname).CopyTo(Nickname, 0);
            Encoding.UTF8.GetBytes(msg).CopyTo(Msg, 0);
            RequestTimeTick = BitConverter.GetBytes(timeSpan);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(Nickname);
            dataSource.AddRange(Msg);
            dataSource.AddRange(RequestTimeTick);

            return dataSource.ToArray();
        }
    }

    public class ChatResPacket
    {
        public UInt16 Result;
        public Int64 RequestTimeTick;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            RequestTimeTick = BitConverter.ToInt64(bodyData, 2);
            return true;
        }
    }
    
    public class ChatBroadcastPacket
    {
        public string Nickname;
        public string Msg;

        public bool FromBytes(byte[] bodyData)
        {
            Nickname = Encoding.UTF8.GetString(bodyData, 0, PacketDef.MAX_NICKNAME_BYTE_LENGTH);
            Nickname = Nickname.Trim().TrimEnd('\0');
            Msg = Encoding.UTF8.GetString(bodyData, PacketDef.MAX_NICKNAME_BYTE_LENGTH, PacketDef.MAX_CHAT_MSG_SIZE);
            Msg = Msg.Trim().TrimEnd('\0');

            return true;
        }
    }
}
