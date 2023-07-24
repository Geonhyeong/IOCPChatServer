#pragma once

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#define size16(val)		static_cast<int16>(sizeof(val))
#define size32(val)		static_cast<int32>(sizeof(val))
#define len16(arr)		static_cast<int16>(sizeof(arr)/sizeof(arr[0]))
#define len32(arr)		static_cast<int32>(sizeof(arr)/sizeof(arr[0]))

using IocpObjectRef = shared_ptr<class IocpObject>;
using SessionRef = shared_ptr<class Session>;
using SendBufferRef = shared_ptr<class SendBuffer>;
using SendBufferChunkRef = shared_ptr<class SendBufferChunk>;

#define BUFFER_SIZE 65536
#define SEND_BUFFER_CHUNK_SIZE 8192		// TLS 영역의 최대 스택 사이즈는 16KB이다.