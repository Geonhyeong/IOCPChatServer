using LoginServer.DB;
using Microsoft.AspNetCore.Mvc;
using Microsoft.EntityFrameworkCore;
using Newtonsoft.Json;
using StackExchange.Redis;
using System;
using System.Linq;
using System.Threading.Tasks;

namespace LoginServer.Controllers
{
    [Route("api/[controller]")]
    [ApiController]
    public class LoginController : ControllerBase
    {
        AppDbContext _context { get; }

        public LoginController(AppDbContext context)
        {
            _context = context;
        }

        [HttpPost]
        [Route("login")]
        public LoginPacketRes Login([FromBody] LoginPacketReq req)
        {
            LoginPacketRes res = new LoginPacketRes();

            AccountDb account = _context.Accounts
                                    .AsNoTracking()
                                    .Where(a => a.UserId == req.UserId)
                                    .FirstOrDefault();

            if (account == null)
            {
                // 등록
                _context.Accounts.Add(new AccountDb()
                {
                    UserId = req.UserId,
                    Password = req.Password
                });

                bool success = _context.SaveChangesEx();
                if (success == false)
                {
                    res.LoginOk = false;
                    return res;
                }
            }

            account = _context.Accounts
                        .AsNoTracking()
                        .Where(a => a.UserId == req.UserId && a.Password == req.Password)
                        .FirstOrDefault();

            if (account == null)
            {
                res.LoginOk = false;
            }
            else
            {
                res.LoginOk = true;

                // 토큰 생성 및 RedisDB에 저장
                DateTime expired = DateTime.UtcNow;
                expired.AddSeconds(600);

                RedisManager redisManager = new RedisManager("localhost:6379");
                IDatabase db = redisManager.GetDatabase();

                string json = db.StringGet(account.AccountDbId.ToString());
                RedisToken redisToken;
                if (json != null)
                {
                    redisToken = JsonConvert.DeserializeObject<RedisToken>(json);
                    redisToken.Token = new Random().Next(Int32.MinValue, Int32.MaxValue);
                    redisToken.Expired = expired.Ticks;
                    db.StringSet(account.AccountDbId.ToString(), JsonConvert.SerializeObject(redisToken));
                }
                else
                {
                    redisToken = new RedisToken()
                    {
                        AccountDbId = account.AccountDbId,
                        Token = new Random().Next(Int32.MinValue, Int32.MaxValue),
                        Expired = expired.Ticks
                    };
                    db.StringSet(account.AccountDbId.ToString(), JsonConvert.SerializeObject(redisToken));
                }

                res.AccountDbId = account.AccountDbId;
                res.Token = redisToken.Token;

                Console.WriteLine($"Id: {res.AccountDbId} Token: {res.Token}");
            }

            return res;
        }
    }

}
