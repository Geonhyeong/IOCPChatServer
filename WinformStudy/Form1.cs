using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Windows.Threading;

namespace WinformStudy
{
    public partial class Form1 : Form
    {
        private Connector connector = new Connector();
        private DispatcherTimer dispatcherUITimer;

        private bool IsHostConnected = false;
        private bool IsBackgroundProcessRunning = false;
        private Queue<string> RoomChatMsgQueue = new Queue<string>();
        private long delayTime = -1;

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
            btnDummyDisconnect.Enabled = false;
            btnDummyChatStart.Enabled = false;
            btnDummyChatStop.Enabled = false;
            btnDummyPlus.Enabled = false;
            btnDummyMinus.Enabled = false;
            comboBoxAddCount.SelectedIndex = 0;

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
            string ip = textBoxIP.Text;
            int port = Convert.ToInt32(textBoxPort.Text);

            connector.Connect(ip, port, () => { return SessionManager.Instance.GenerateHostSession(); });
            btnConnect.Enabled = false;
            btnDisconnect.Enabled = true;

            IsHostConnected = true;
        }

        private void btnDisconnect_Click(object sender, EventArgs e)
        {
            IsHostConnected = false;
            SessionManager.Instance.RemoveHostSession();

            btnDisconnect.Enabled = false;
            btnConnect.Enabled = true;

            RoomChatMsgQueue.Clear();
        }

        private void btnDummyPlus_Click(object sender, EventArgs e)
        {
            if (comboBoxAddCount.SelectedIndex < 0)
                return;

            string ip = textBoxIP.Text;
            int port = Convert.ToInt32(textBoxPort.Text);
            int addCount = Convert.ToInt32(comboBoxAddCount.SelectedItem);

            connector.Connect(ip, port, () => { return SessionManager.Instance.GenerateDummySession(); }, addCount);
            textBoxDummyCount.Text = SessionManager.Instance.GetDummyCount().ToString();
        }

        private void btnDummyMinus_Click(object sender, EventArgs e)
        {
            if (comboBoxAddCount.SelectedIndex < 0)
                return;

            int minusCount = Convert.ToInt32(comboBoxAddCount.SelectedItem);

            SessionManager.Instance.RemoveDummySessions(minusCount);
            textBoxDummyCount.Text = SessionManager.Instance.GetDummyCount().ToString();
        }

        private void btnDummyChatStart_Click(object sender, EventArgs e)
        {
            SessionManager.Instance.DummyAutoChatStart();

            btnDummyChatStart.Enabled = false;
            btnDummyChatStop.Enabled = true;
        }

        private void btnDummyChatStop_Click(object sender, EventArgs e)
        {
            SessionManager.Instance.DummyAutoChatStop();

            btnDummyChatStop.Enabled = false;
            btnDummyChatStart.Enabled = true;
        }

        private void btnDummyConnect_Click(object sender, EventArgs e)
        {
            string ip = textBoxIP.Text;
            int port = Convert.ToInt32(textBoxPort.Text);
            int dummyCount = Convert.ToInt32(textBoxDummyCount.Text);

            connector.Connect(ip, port, () => { return SessionManager.Instance.GenerateDummySession(); }, dummyCount);
            btnDummyConnect.Enabled = false;
            btnDummyDisconnect.Enabled = true;

            btnDummyChatStart.Enabled = true;
            btnDummyChatStop.Enabled = false;

            btnDummyPlus.Enabled = true;
            btnDummyMinus.Enabled = true;
        }
        
        private void btnDummyDisconnect_Click(object sender, EventArgs e)
        {
            SessionManager.Instance.RemoveDummySessions();

            btnDummyChatStart.Enabled = false;
            btnDummyChatStop.Enabled = false;

            btnDummyDisconnect.Enabled = false;
            btnDummyConnect.Enabled = true;

            btnDummyPlus.Enabled = false;
            btnDummyMinus.Enabled = false;
        }

        private void btnChat_Click(object sender, EventArgs e)
        {
            if (textBoxUserName.Text.Length == 0)
            {
                MessageBox.Show("닉네임을 입력하세요");
                return;
            }

            if (textBoxChatMessage.Text.Length == 0)
            {
                MessageBox.Show("채팅 메시지를 입력하세요");
                return;
            }

            C_ChatPacket chatPacket = new C_ChatPacket();
            chatPacket.SetValue(textBoxUserName.Text, textBoxChatMessage.Text);

            byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.C_CHAT, chatPacket.ToBytes());
            SessionManager.Instance.SendFromHost(new ArraySegment<byte>(sendBuffer));

            string msg = $"{textBoxUserName.Text} > {textBoxChatMessage.Text}";
            RoomChatMsgQueue.Enqueue(msg);

            textBoxChatMessage.Text = "";
        }

        private void BackgroundWorker(object sender, EventArgs e)
        {
            DelayCheck();
            ProcessLog();
            ProcessRoomChat();
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
            if (IsBackgroundProcessRunning && IsHostConnected)
            {
                DelayCheckPacket delayCheckPacket = new DelayCheckPacket();
                delayCheckPacket.CurrentTimeSpan = DateTime.Now.Ticks;

                byte[] sendBuffer = PacketDef.MakeSendBuffer(PACKET_ID.DELAY_CHECK, delayCheckPacket.ToBytes());
                SessionManager.Instance.SendFromHost(new ArraySegment<byte>(sendBuffer));

                labelDelayTime.Text = $"{delayTime} ms";
            }
        }
    }
}
