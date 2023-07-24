#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize)
	: _owner(owner), _buffer(buffer), _allocSize(allocSize), _writeSize(0)
{
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::Close(uint32 writeSize)
{
	ASSERT_CRASH(_allocSize >= writeSize);
	_writeSize = writeSize;
	_owner->Close(writeSize);
}

/*--------------------
	SendBufferChunk
--------------------*/

SendBufferChunk::SendBufferChunk()
{
	_buffer = {};
	_open = false;
	_usedSize = 0;
}

SendBufferChunk::~SendBufferChunk()
{
}

void SendBufferChunk::Reset()
{
	_open = false;
	_usedSize = 0;
}

SendBufferRef SendBufferChunk::Open(uint32 allocSize)
{
	ASSERT_CRASH(allocSize <= SEND_BUFFER_CHUNK_SIZE);
	ASSERT_CRASH(_open == false);

	if (allocSize > FreeSize())
		return nullptr;

	_open = true;
	return make_shared<SendBuffer>(shared_from_this(), Buffer(), allocSize);
}

void SendBufferChunk::Close(uint32 writeSize)
{
	ASSERT_CRASH(_open == true);
	_open = false;
	_usedSize += writeSize;
}

/*---------------------
	SendBufferManager
----------------------*/

SendBufferManager::SendBufferManager()
{
}

SendBufferManager::~SendBufferManager()
{
	_sendBufferChunks.clear();
}

SendBufferManager& SendBufferManager::GetInstance()
{
	static SendBufferManager* instance = new SendBufferManager();
	return *instance;
}

SendBufferRef SendBufferManager::Open(uint32 size)
{
	if (LSendBufferChunk == nullptr)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	ASSERT_CRASH(LSendBufferChunk->IsOpen() == false);

	// 다 썼으면 버리고 새거로 교체
	if (LSendBufferChunk->FreeSize() < size)
	{
		LSendBufferChunk = Pop();
		LSendBufferChunk->Reset();
	}

	return LSendBufferChunk->Open(size);
}

SendBufferChunkRef SendBufferManager::Pop()
{
	{
		lock_guard<mutex> lock(_lock);

		if (_sendBufferChunks.empty() == false)
		{
			SendBufferChunkRef sendBufferChunk = _sendBufferChunks.back();
			_sendBufferChunks.pop_back();
			return sendBufferChunk;
		}
	}
	
	// new로 생성하고 소멸자(deleter)로 PushGlobal을 호출하도록 하는 생성자(?), 이것이 PushGlobal이 static인 이유
	// 메모리를 소멸하지 않고 Pool에다가 다시 반납하도록 하기 위함.
	return SendBufferChunkRef(new SendBufferChunk, PushGlobal);
}

void SendBufferManager::Push(SendBufferChunkRef buffer)
{
	lock_guard<mutex> lock(_lock);
	_sendBufferChunks.push_back(buffer);
}

void SendBufferManager::PushGlobal(SendBufferChunk* buffer)
{
	SendBufferManager::GetInstance().Push(SendBufferChunkRef(buffer, PushGlobal));
}