using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinformStudy
{
    class SessionManager
    {
        public static SessionManager Instance { get; } = new SessionManager();

        private object _lock = new object();
        private List<DummySession> _dummies = new List<DummySession>();
        private HostSession _host = null;

        public HostSession GenerateHostSession()
        {
            if (_host != null)
            {
                DevLog.Write("Host is already generated.", LOG_LEVEL.WARN);
                return null;
            }

            _host = new HostSession();
            return _host;
        }

        public void RemoveHostSession()
        {
            if (_host == null)
            {
                DevLog.Write("Host is already Removed.", LOG_LEVEL.WARN);
                return;
            }

            _host.Disconnect();
            _host = null;
        }

        public DummySession GenerateDummySession()
        {
            lock (_lock)
            {
                DummySession dummySession = new DummySession();

                _dummies.Add(dummySession);
                return dummySession;
            }
        }

        public void RemoveDummySessions(int count = 0)
        {
            lock (_lock)
            {
                if (count == 0 || count > _dummies.Count())
                {
                    foreach (DummySession dummySession in _dummies)
                    {
                        dummySession.AutoSend = false;
                        dummySession.Disconnect();
                    }

                    _dummies.Clear();
                }
                else
                {
                    for (int i = 0; i < count; i++)
                    {
                        _dummies[i].AutoSend = false;
                        _dummies[i].Disconnect();
                    }

                    _dummies.RemoveRange(0, count);
                }
            }
        }

        public void DummyAutoChatStart()
        {
            lock (_lock)
            {
                foreach (DummySession dummySession in _dummies)
                {
                    dummySession.AutoSend = true;
                    dummySession.StartAutoChat();
                }
            }
        }

        public void DummyAutoChatStop()
        {
            lock (_lock)
            {
                foreach (DummySession dummySession in _dummies)
                {
                    dummySession.AutoSend = false;
                }
            }
        }

        public void SendFromHost(ArraySegment<byte> sendBuffer)
        {
            _host.Send(sendBuffer);
        }

        public int GetDummyCount() { return _dummies.Count(); }
    }
}
