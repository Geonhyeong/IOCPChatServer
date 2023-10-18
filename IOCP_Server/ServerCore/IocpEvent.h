#pragma once

enum class EventType : uint8
{
	ACCEPT,
	RECV,
	SEND,
	DISCONNECT
};

class IocpEvent : public OVERLAPPED
{
public:
	IocpEvent(EventType type) : eventType(type) { Init(); }
	void Init()
	{
		OVERLAPPED::hEvent = 0;
		OVERLAPPED::Internal = 0;
		OVERLAPPED::InternalHigh = 0;
		OVERLAPPED::Offset = 0;
		OVERLAPPED::OffsetHigh = 0;
	}

public:
	EventType eventType;
	SessionRef ownerSession;
};

class AcceptEvent : public IocpEvent
{
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::SEND) {}
	
	vector<SendBufferRef> sendBuffers;
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::DISCONNECT) {}
};