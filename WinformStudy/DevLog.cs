using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WinformStudy
{
    public enum LOG_LEVEL
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        DISABLE
    }

    public class DevLog
    {
        static ConcurrentQueue<string> logMsgQueue = new ConcurrentQueue<string>();
        static Int64 curLogLevel = (Int64)LOG_LEVEL.TRACE;

        static public void Init(LOG_LEVEL logLevel)
        {
            ChangeLogLevel(logLevel);
        }

        static public void ChangeLogLevel(LOG_LEVEL logLevel)
        {
            Interlocked.Exchange(ref curLogLevel, (int)logLevel);
        }

        public static LOG_LEVEL CurrentLogLevel()
        {
            return (LOG_LEVEL)Interlocked.Read(ref curLogLevel);
        }

        static public void Write(string msg, LOG_LEVEL logLevel = LOG_LEVEL.TRACE,
                                [CallerFilePath] string fileName = "",
                                [CallerMemberName] string methodName = "",
                                [CallerLineNumber] int lineNumber = 0)
        {
            if (CurrentLogLevel() <= logLevel)
            {
                logMsgQueue.Enqueue(string.Format("{0}:{1}| {2}", DateTime.Now, methodName, msg));
            }
        }

        static public bool GetLog(out string msg)
        {
            if (logMsgQueue.TryDequeue(out msg))
            {
                return true;
            }

            return false;
        }
    }
}
