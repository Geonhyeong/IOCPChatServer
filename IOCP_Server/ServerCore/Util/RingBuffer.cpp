#include "pch.h"
#include "RingBuffer.h"

RingBuffer::RingBuffer(int32 bufferSize) : _bufferSize(bufferSize)
{
	_capacity = bufferSize * BUFFER_COUNT;
	_buffer.resize(_capacity);
}

RingBuffer::~RingBuffer()
{
	_buffer.clear();
}

void RingBuffer::Clean()
{
	int32 dataSize = DataSize();
	if (dataSize == 0)
	{
		// �� ��ħ �б�+���� Ŀ���� ������ ��ġ���, �� �� ����.
		_readPos = _writePos = 0;
	}
	else
	{
		// ���� ������ ���� 1�� ũ�� �̸��̸�, �����͸� ������ �����.
		if (FreeSize() < _bufferSize)
		{
			::memcpy(&_buffer[0], &_buffer[_readPos], dataSize);
			_readPos = 0;
			_writePos = dataSize;
		}
	}
}

bool RingBuffer::OnRead(int32 numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	_readPos += numOfBytes;
	return true;
}

bool RingBuffer::OnWrite(int32 numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	_writePos += numOfBytes;
	return true;
}
