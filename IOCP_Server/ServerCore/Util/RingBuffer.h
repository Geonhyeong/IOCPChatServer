#pragma once

class RingBuffer
{
	enum { BUFFER_COUNT = 10 };
public:
	RingBuffer(int32 bufferSize);
	~RingBuffer();

	void	Clean();
	bool	OnRead(int32 numOfBytes);
	bool	OnWrite(int32 numOfBytes);

	BYTE*	ReadPos() { return &_buffer[_readPos]; }
	BYTE*	WritePos() { return &_buffer[_writePos]; }
	int32	DataSize() { return _writePos - _readPos; }
	int32	FreeSize() { return _bufferSize - _writePos; }

private:
	vector<BYTE> _buffer;

	int32 _capacity;
	int32 _bufferSize;
	int32 _writePos;
	int32 _readPos;
};

