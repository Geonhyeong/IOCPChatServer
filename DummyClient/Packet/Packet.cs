using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DummyClient
{
    public struct PacketData
    {
        public UInt16 packetSize;
        public UInt16 packetId;
        public Byte type;
        public byte[] bodyData;
    }

    public class LoginReqPacket
    {
        byte[] UserId = new byte[PacketDef.MAX_ID_PWD_BYTE_LENGTH];
        byte[] AccountDbId = new byte[sizeof(Int32)];
        byte[] Token = new byte[sizeof(Int32)];
        byte[] IsDummy = new byte[sizeof(bool)];

        public void SetValue(string userId, Int32 accountDbId, Int32 token, bool isDummy = false)
        {
            Encoding.Unicode.GetBytes(userId).CopyTo(UserId, 0);
            AccountDbId = BitConverter.GetBytes(accountDbId);
            Token = BitConverter.GetBytes(token);
            IsDummy = BitConverter.GetBytes(isDummy);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(UserId);
            dataSource.AddRange(AccountDbId);
            dataSource.AddRange(Token);
            dataSource.AddRange(IsDummy);

            return dataSource.ToArray();
        }
    }

    public class LoginResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class LogoutResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomEnterReqPacket
    {
        byte[] RoomNumber = new byte[sizeof(UInt32)];

        public void SetValue(UInt32 roomNumber)
        {
            RoomNumber = BitConverter.GetBytes(roomNumber);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
            dataSource.AddRange(RoomNumber);

            return dataSource.ToArray();
        }
    }

    public class RoomEnterResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomLeaveResPacket
    {
        public UInt16 Result;

        public bool FromBytes(byte[] bodyData)
        {
            Result = BitConverter.ToUInt16(bodyData, 0);
            return true;
        }
    }

    public class RoomUserListPacket
    {
        public UInt32 UserCount;
        public List<string> UserList = new List<string>();

        public bool FromBytes(byte[] bodyData)
        {
            var readPos = 0;

            UserCount = BitConverter.ToUInt32(bodyData, 0);
            readPos += 4;

            for (int i = 0; i < UserCount; i++)
            {
                var userId = Encoding.Unicode.GetString(bodyData, readPos, PacketDef.MAX_ID_PWD_BYTE_LENGTH);
                readPos += PacketDef.MAX_ID_PWD_BYTE_LENGTH;

                userId.Trim().TrimEnd('\0');
                UserList.Add(userId);
            }

            return true;
        }
    }

    public class ChatReqPacket
    {
        byte[] Msg = new byte[PacketDef.MAX_CHAT_MSG_SIZE];
        byte[] RequestTimeTick = new byte[sizeof(Int64)];

        public void SetValue(string msg, Int64 timeSpan = 0)
        {
            Encoding.Unicode.GetBytes(msg).CopyTo(Msg, 0);
            RequestTimeTick = BitConverter.GetBytes(timeSpan);
        }

        public byte[] ToBytes()
        {
            List<byte> dataSource = new List<byte>();
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
        public string UserId;
        public string Msg;

        public bool FromBytes(byte[] bodyData)
        {
            UserId = Encoding.Unicode.GetString(bodyData, 0, PacketDef.MAX_ID_PWD_BYTE_LENGTH);
            UserId = UserId.Trim().TrimEnd('\0');
            Msg = Encoding.Unicode.GetString(bodyData, PacketDef.MAX_ID_PWD_BYTE_LENGTH, PacketDef.MAX_CHAT_MSG_SIZE);
            Msg = Msg.Trim().TrimEnd('\0');

            return true;
        }
    }
}
