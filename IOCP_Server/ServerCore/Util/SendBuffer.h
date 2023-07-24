#pragma once

class SendBuffer
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, uint32 allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return _buffer; }
	uint32		WriteSize() { return _writeSize; }
	void		Close(uint32 writeSize);

private:
	BYTE*				_buffer;
	uint32				_allocSize;
	uint32				_writeSize;
	SendBufferChunkRef	_owner;
};

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
public:
	SendBufferChunk();
	~SendBufferChunk();

	void			Reset();
	SendBufferRef	Open(uint32 allocSize);
	void			Close(uint32 writeSize);

	bool			IsOpen() { return _open; }
	BYTE*			Buffer() { return &_buffer[_usedSize]; }
	uint32			FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	array<BYTE, SEND_BUFFER_CHUNK_SIZE>	_buffer;
	bool								_open;
	uint32								_usedSize;
};

class SendBufferManager
{
private:
	SendBufferManager();
	~SendBufferManager();

public:
	static SendBufferManager&	GetInstance();
	SendBufferRef				Open(uint32 size);

private:
	SendBufferChunkRef			Pop();
	void						Push(SendBufferChunkRef buffer);
	static void					PushGlobal(SendBufferChunk* buffer);

private:
	mutex _lock;
	list<SendBufferChunkRef> _sendBufferChunks;
};