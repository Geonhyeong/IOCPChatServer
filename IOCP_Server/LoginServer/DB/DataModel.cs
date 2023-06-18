using System.ComponentModel.DataAnnotations.Schema;
using System.Threading.Tasks;

namespace LoginServer.DB
{
    [Table("Account")]
    public class AccountDb
    {
        public int AccountDbId { get; set; }
        public string UserId { get; set; }
        public string Password { get; set; }
    }
}
