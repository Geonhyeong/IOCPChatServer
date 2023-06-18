using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows.Forms;
using System.Windows.Threading;

namespace WinformStudy
{
    public partial class Form1 : Form
    {
        private Connector connector = new Connector();
        private HostSession session = null;
        private DispatcherTimer dispatcherUITimer;

        private bool IsBackgroundProcessRunning = false;
        private List<string> UserList = new List<string>();
        private Queue<string> RoomChatMsgQueue = new Queue<string>();
        private Queue<long> DelayTimeQueue = new Queue<long>(5);

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // 백그라운드 프로세스 실행
            IsBackgroundProcessRunning = true;
            dispatcherUITimer = new System.Windows.Threading.DispatcherTimer();
            dispatcherUITimer.Tick += new EventHandler(BackgroundWorker);
            dispatcherUITimer.Interval = new TimeSpan(0, 0, 0, 0, 100);
            dispatcherUITimer.Start();

            btnDisconnect.Enabled = false;
            btnLogin.Enabled = false;
            btnLogout.Enabled = false;
            btnRoomEnter.Enabled = false;
            btnRoomLeave.Enabled = false;
            btnChat.Enabled = false;

            InitPacketHandler();

            DevLog.Write("프로그램 시작!", LOG_LEVEL.INFO);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IsBackgroundProcessRunning = false;
        }

        private void checkBoxLocalHost_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxLocalHost.Checked == true)
                textBoxIP.Text = "127.0.0.1";
        }

        private void btnConnect_Click(object sender, EventArgs e)
        {
            if (session != null)
                return;

            string ip = textBoxIP.Text;
            int port = Convert.ToInt32(textBoxPort.Text);

            connector.Connect(ip, port, () => { session = new HostSession(); return session; });
            btnConnect.Enabled = false;
            btnDisconnect.Enabled = true;
            btnLogin.Enabled = true;
        }

        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            if (session == null)
                return;

            session.Disconnect();
            session = null;

            RoomChatMsgQueue.Clear();
            
            btnDisconnect.Enabled = false;
            btnConnect.Enabled = true;
            btnLogin.Enabled = false;
            btnLogout.Enabled = false;
            btnRoomEnter.Enabled = false;
            btnRoomLeave.Enabled = false;
            btnChat.Enabled = false;

            labelDelayTime.Text = "0 ms";
            tbRoomNumber.Text = "";
            tbUserId.Text = "";
            tbPassword.Text = "";

            UserList.Clear();
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            string userId = tbUserId.Text;
            string password = tbPassword.Text;

            if (userId == "" || password == "")
            {
                MessageBox.Show("아이디 및 비밀번호를 입력하세요");
                return;
            }

            // Web Server에 로그인 요청
            var httpWebRequest = (HttpWebRequest)WebRequest.Create("https://localhost:5001/api/login/login");
            httpWebRequest.ContentType = "application/json";
            httpWebRequest.Method = "POST";

            WebTokenReqPacket packet = new WebTokenReqPacket();
            packet.UserId = userId;
            packet.Password = password;

            string json = Newtonsoft.Json.JsonConvert.SerializeObject(packet);
            byte[] bytes = Encoding.UTF8.GetBytes(json);

            using (Stream reqStream = httpWebRequest.GetRequestStream())
            {
                reqStream.Write(bytes, 0, bytes.Length);
            }

            string responseText = string.Empty;
            using (var httpResponse = (WebResponse)httpWebRequest.GetResponse())
            {
                Stream resStream = httpResponse.GetResponseStream();
                using (StreamReader reader = new StreamReader(resStream))
                {
                    responseText = reader.ReadToEnd();
                    WebTokenResPacket resPacket = Newtonsoft.Json.JsonConvert.DeserializeObject<WebTokenResPacket>(responseText);

                    if (resPacket.LoginOk)
                    {
                        session.UserId = userId;

                        LoginReqPacket loginReqPacket = new LoginReqPacket();
                        loginReqPacket.SetValue(userId, resPacket.AccountDbId, resPacket.Token);

                        byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.LOGIN_REQ, loginReqPacket.ToBytes());
                        session.Send(new ArraySegment<byte>(sendBuffer));

                        btnLogin.Enabled = false;
                        btnLogout.Enabled = true;
                        btnRoomEnter.Enabled = true;
                    }
                    else
                        MessageBox.Show("로그인 실패");
                }
            }
        }

        private void btnLogout_Click(object sender, EventArgs e)
        {
            byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.LOGOUT_REQ, new byte[0]);
            session.Send(new ArraySegment<byte>(sendBuffer));
            
            btnLogout.Enabled = false;
            btnRoomEnter.Enabled = false;
            btnRoomLeave.Enabled = false;
            btnLogin.Enabled = true;

            labelDelayTime.Text = "0 ms";
            tbRoomNumber.Text = "";

            UserList.Clear();
        }

        private void btnRoomEnter_Click(object sender, EventArgs e)
        {
            if (tbRoomNumber.Text.Length == 0)
            {
                MessageBox.Show("방 번호를 입력하세요");
                return;
            }

            UInt32 roomNumber = UInt32.Parse(tbRoomNumber.Text);

            RoomEnterReqPacket roomEnterReqPacket = new RoomEnterReqPacket();
            roomEnterReqPacket.SetValue(roomNumber);

            byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.ROOM_ENTER_REQ, roomEnterReqPacket.ToBytes());
            session.Send(new ArraySegment<byte>(sendBuffer));

            btnRoomEnter.Enabled = false;
            btnRoomLeave.Enabled = true;
            btnChat.Enabled = true;
        }

        private void btnRoomLeave_Click(object sender, EventArgs e)
        {
            byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.ROOM_LEAVE_REQ, new byte[0]);
            session.Send(new ArraySegment<byte>(sendBuffer));

            btnChat.Enabled = false;
            btnRoomLeave.Enabled = false;
            btnRoomEnter.Enabled = true;

            UserList.Clear();
        }

        private void textBoxChatMessage_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                btnChat_Click(sender, e);
            }
        }

        private void btnChat_Click(object sender, EventArgs e)
        {
            if (textBoxChatMessage.Text.Length == 0)
            {
                MessageBox.Show("채팅 메시지를 입력하세요");
                return;
            }

            ChatReqPacket chatReqPacket = new ChatReqPacket();
            chatReqPacket.SetValue(textBoxChatMessage.Text, DateTime.Now.Ticks);

            byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.CHAT_REQ, chatReqPacket.ToBytes());
            session.Send(new ArraySegment<byte>(sendBuffer));

            textBoxChatMessage.Text = "";
        }

        private void BackgroundWorker(object sender, EventArgs e)
        {
            ProcessLog();
            ProcessUserList();
            ProcessRoomChat();
            DelayCheck();
        }

        private void ProcessLog()
        {
            // 너무 이 작업만 할 수 없으므로 일정 작업 이상을 하면 일단 패스한다.
            int logWorkCount = 0;

            while (IsBackgroundProcessRunning)
            {
                System.Threading.Thread.Sleep(1);

                string msg;

                if (DevLog.GetLog(out msg))
                {
                    ++logWorkCount;

                    if (listBoxLog.Items.Count > 512)
                    {
                        listBoxLog.Items.Clear();
                    }

                    listBoxLog.Items.Add(msg);
                    listBoxLog.SelectedIndex = listBoxLog.Items.Count - 1;
                }
                else
                {
                    break;
                }

                if (logWorkCount > 8)
                {
                    break;
                }
            }
        }

        private void ProcessUserList()
        {
            if (IsBackgroundProcessRunning)
            {
                listBoxUserList.Items.Clear();

                foreach (string user in UserList)
                {
                    listBoxUserList.Items.Add(user);
                }

                labelRoomUserCount.Text = UserList.Count.ToString();
            }
        }

        private void ProcessRoomChat()
        {
            int roomChatWorkCount = 0;

            while (IsBackgroundProcessRunning)
            {
                System.Threading.Thread.Sleep(1);

                if (RoomChatMsgQueue.Count > 0)
                {
                    ++roomChatWorkCount;

                    string msg = RoomChatMsgQueue.Dequeue();

                    if (listBoxRoomChatMsg.Items.Count > 512)
                        listBoxRoomChatMsg.Items.Clear();

                    listBoxRoomChatMsg.Items.Add("> " + msg);
                    listBoxRoomChatMsg.SelectedIndex = listBoxRoomChatMsg.Items.Count - 1;
                }
                else
                    break;

                if (roomChatWorkCount > 8)
                    break;
            }
        }

        private void DelayCheck()
        {
            if (IsBackgroundProcessRunning&& DelayTimeQueue.Count > 0)
            {
                while (DelayTimeQueue.Count() > 5)
                    DelayTimeQueue.Dequeue();

                long sum = 0;
                foreach (long delayTime in DelayTimeQueue)
                    sum += delayTime;
                
                labelDelayTime.Text = $"{sum / DelayTimeQueue.Count()} ms";
            }
        }

    }
}
