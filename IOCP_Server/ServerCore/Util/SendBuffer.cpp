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

	// �� ������ ������ ���ŷ� ��ü
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
	
	// new�� �����ϰ� �Ҹ���(deleter)�� PushGlobal�� ȣ���ϵ��� �ϴ� ������(?), �̰��� PushGlobal�� static�� ����
	// �޸𸮸� �Ҹ����� �ʰ� Pool���ٰ� �ٽ� �ݳ��ϵ��� �ϱ� ����.
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