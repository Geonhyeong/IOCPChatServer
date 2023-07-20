module;
#include <Windows.h>
#include <string>

export module iocp.acceptor;
import lang;
import net;

namespace Iocp
{
	export class Acceptor
	{
	public:
		Acceptor() {}
		virtual ~Acceptor() {}

		bool Init(std::wstring ip, u_short port)
		{
			_acceptSocket = SocketUtils::CreateSocket();
			if (_acceptSocket == INVALID_SOCKET)
				return false;

			if (SocketUtils::SetReuseAddress(_acceptSocket, true) == false)
				return false;

			if (SocketUtils::SetLinger(_acceptSocket, 0, 0) == false)
				return false;

			//_netAddress = NetAddress(ip, port);

			/*if (SocketUtils::Bind(_acceptSocket, &_netAddress.GetSockAddr()) == false)
				return false;*/

			// TODO : bind, accept, iocp register...

			return true;
		}

	private:
		bool CreateSocket();
		bool BindAndListen(int port);


		void AcceptThread();

	private:
		SOCKET	_acceptSocket;
		//NetAddress _netAddress;

		Thread* _acceptThread;
	};

	/*-----------------------------------------------------------------------------*/
}