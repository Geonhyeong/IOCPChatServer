using System;
using System.Net;
using StackExchange.Redis;

public class RedisManager
{
    private readonly Lazy<ConnectionMultiplexer> lazyConnection;

    public RedisManager(string connectionString)
    {
        lazyConnection = new Lazy<ConnectionMultiplexer>(() =>
        {
            return ConnectionMultiplexer.Connect(connectionString);
        });
    }

    public IDatabase GetDatabase(int db = -1)
    {
        return lazyConnection.Value.GetDatabase(db);
    }

    public void FlushDatabase(int db = -1)
    {
        this.lazyConnection.Value.GetServer(lazyConnection.Value.GetEndPoints()[0]).FlushDatabase(db);
    }

    public void ClearCache()
    {
        foreach (EndPoint endPoint in lazyConnection.Value.GetEndPoints())
        {
            IServer server = lazyConnection.Value.GetServer(endPoint);
            server.FlushAllDatabases();
        }
    }
}

public class RedisToken
{
    public int AccountDbId { get; set; }
    public int Token { get; set; }
    public long Expired { get; set; }
}