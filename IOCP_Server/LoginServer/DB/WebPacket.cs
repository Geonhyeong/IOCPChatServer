public class LoginPacketReq
{
    public string UserId { get; set; }
    public string Password { get; set; }
}

public class LoginPacketRes
{
    public bool LoginOk { get; set; }
    public int AccountDbId { get; set; }
    public int Token { get; set; }
}