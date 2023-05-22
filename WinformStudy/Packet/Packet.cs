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

    public class C_ChatPacket
    {
        byte[] Nickname = new byte[PacketDef.MAX_NICKNAME_BYTE_LENGTH];
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];

        public void SetValue(string nickname, string msg)
        {
            Encoding.UTF8.GetBytes(nickname).CopyTo(Nickname, 0);
            Encoding.UTF8.GetBytes(msg).CopyTo(Msg, 0);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(Nickname);
            dataSource.AddRange(Msg);

            return dataSource.ToArray();
        }
    }
    
    public class S_ChatPacket
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

    public class DelayCheckPacket
    {
        public long CurrentTimeSpan;

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(BitConverter.GetBytes(CurrentTimeSpan));
            return dataSource.ToArray();
        }

        public bool FromBytes(byte[] bodyData)
        {
            CurrentTimeSpan = BitConverter.ToInt64(bodyData, 0);
            return true;
        }
    }
}
