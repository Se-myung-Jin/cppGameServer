#include "pch.h"
#include "Listener.h"

Listener::~Listener()
{
}

bool Listener::StartAccept(NetAddress netAddress)
{
	return false;
}

void Listener::CloseSocket()
{
}

HANDLE Listener::GetHandle()
{
	return HANDLE();
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
}

void Listener::RegisterAccept(AcceptEvent* acceptEvent)
{
}

void Listener::ProcessAccept(AcceptEvent* acceptEvent)
{
}
