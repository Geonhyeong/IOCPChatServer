#pragma once

enum class EventType : uint8
{
	Accept,
	Recv,
	Send,
	Disconnect
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
	AcceptEvent() : IocpEvent(EventType::Accept) {}
};

class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::Recv) {}
};

class SendEvent : public IocpEvent
{
public:
	SendEvent() : IocpEvent(EventType::Send) {}
};

class DisconnectEvent : public IocpEvent
{
public:
	DisconnectEvent() : IocpEvent(EventType::Disconnect) {}
};