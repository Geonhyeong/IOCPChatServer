module;
#include <memory>

export module iocp;
export import iocp.acceptor;

import lang;
import util;

//namespace Iocp
//{
//	export class Entry : public Singletone<Entry>
//	{
//		friend class Singletone;
//	public:
//		bool Init()
//		{
//			if (_acceptor->Init("127.0.0.1", 2581) == false)
//				return false;
//
//			return true;
//		}
//
//		void Join()
//		{
//			ThreadManager::GetInstance().Join();
//		}
//
//	private:
//		Entry() {}
//		virtual ~Entry() {}
//
//		std::unique_ptr<Acceptor> _acceptor;
//	};
//}