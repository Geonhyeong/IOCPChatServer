
namespace WinformStudy
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.textBoxPort = new System.Windows.Forms.TextBox();
            this.textBoxIP = new System.Windows.Forms.TextBox();
            this.checkBoxLocalHost = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tbUserId = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.btnConnect = new System.Windows.Forms.Button();
            this.btnDisconnect = new System.Windows.Forms.Button();
            this.listBoxLog = new System.Windows.Forms.ListBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnLogout = new System.Windows.Forms.Button();
            this.btnLogin = new System.Windows.Forms.Button();
            this.tbPassword = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.listBoxRoomChatMsg = new System.Windows.Forms.ListBox();
            this.textBoxChatMessage = new System.Windows.Forms.TextBox();
            this.btnChat = new System.Windows.Forms.Button();
            this.label5 = new System.Windows.Forms.Label();
            this.labelDelayTime = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.labelRoomUserCount = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.listBoxUserList = new System.Windows.Forms.ListBox();
            this.btnRoomLeave = new System.Windows.Forms.Button();
            this.btnRoomEnter = new System.Windows.Forms.Button();
            this.tbRoomNumber = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.textBoxPort);
            this.groupBox1.Controls.Add(this.textBoxIP);
            this.groupBox1.Controls.Add(this.checkBoxLocalHost);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(414, 50);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Socket 클라이언트 설정";
            // 
            // textBoxPort
            // 
            this.textBoxPort.Location = new System.Drawing.Point(247, 17);
            this.textBoxPort.MaxLength = 5;
            this.textBoxPort.Name = "textBoxPort";
            this.textBoxPort.Size = new System.Drawing.Size(51, 21);
            this.textBoxPort.TabIndex = 6;
            this.textBoxPort.Text = "2581";
            // 
            // textBoxIP
            // 
            this.textBoxIP.Location = new System.Drawing.Point(73, 18);
            this.textBoxIP.MaxLength = 15;
            this.textBoxIP.Name = "textBoxIP";
            this.textBoxIP.Size = new System.Drawing.Size(100, 21);
            this.textBoxIP.TabIndex = 5;
            this.textBoxIP.Text = "127.0.0.1";
            // 
            // checkBoxLocalHost
            // 
            this.checkBoxLocalHost.AutoSize = true;
            this.checkBoxLocalHost.Location = new System.Drawing.Point(304, 20);
            this.checkBoxLocalHost.Name = "checkBoxLocalHost";
            this.checkBoxLocalHost.Size = new System.Drawing.Size(103, 16);
            this.checkBoxLocalHost.TabIndex = 4;
            this.checkBoxLocalHost.Text = "localhost 사용";
            this.checkBoxLocalHost.UseVisualStyleBackColor = true;
            this.checkBoxLocalHost.CheckedChanged += new System.EventHandler(this.checkBoxLocalHost_CheckedChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(180, 20);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(61, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "포트 번호:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(61, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "서버 주소:";
            // 
            // tbUserId
            // 
            this.tbUserId.Location = new System.Drawing.Point(64, 20);
            this.tbUserId.MaxLength = 16;
            this.tbUserId.Name = "tbUserId";
            this.tbUserId.Size = new System.Drawing.Size(79, 21);
            this.tbUserId.TabIndex = 8;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(9, 23);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 12);
            this.label3.TabIndex = 7;
            this.label3.Text = "아이디: ";
            // 
            // btnConnect
            // 
            this.btnConnect.Location = new System.Drawing.Point(432, 11);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(88, 23);
            this.btnConnect.TabIndex = 1;
            this.btnConnect.Text = "접속하기";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // btnDisconnect
            // 
            this.btnDisconnect.Location = new System.Drawing.Point(432, 40);
            this.btnDisconnect.Name = "btnDisconnect";
            this.btnDisconnect.Size = new System.Drawing.Size(88, 23);
            this.btnDisconnect.TabIndex = 2;
            this.btnDisconnect.Text = "접속 끊기";
            this.btnDisconnect.UseVisualStyleBackColor = true;
            this.btnDisconnect.Click += new System.EventHandler(this.btnDisconnect_Click);
            // 
            // listBoxLog
            // 
            this.listBoxLog.FormattingEnabled = true;
            this.listBoxLog.ItemHeight = 12;
            this.listBoxLog.Location = new System.Drawing.Point(12, 462);
            this.listBoxLog.Name = "listBoxLog";
            this.listBoxLog.ScrollAlwaysVisible = true;
            this.listBoxLog.Size = new System.Drawing.Size(508, 232);
            this.listBoxLog.TabIndex = 3;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnLogout);
            this.groupBox2.Controls.Add(this.btnLogin);
            this.groupBox2.Controls.Add(this.tbPassword);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.tbUserId);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Location = new System.Drawing.Point(13, 69);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(507, 53);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "로그인";
            // 
            // btnLogout
            // 
            this.btnLogout.Location = new System.Drawing.Point(413, 18);
            this.btnLogout.Name = "btnLogout";
            this.btnLogout.Size = new System.Drawing.Size(75, 23);
            this.btnLogout.TabIndex = 17;
            this.btnLogout.Text = "로그아웃";
            this.btnLogout.UseVisualStyleBackColor = true;
            this.btnLogout.Click += new System.EventHandler(this.btnLogout_Click);
            // 
            // btnLogin
            // 
            this.btnLogin.Location = new System.Drawing.Point(332, 18);
            this.btnLogin.Name = "btnLogin";
            this.btnLogin.Size = new System.Drawing.Size(75, 23);
            this.btnLogin.TabIndex = 16;
            this.btnLogin.Text = "로그인";
            this.btnLogin.UseVisualStyleBackColor = true;
            this.btnLogin.Click += new System.EventHandler(this.btnLogin_Click);
            // 
            // tbPassword
            // 
            this.tbPassword.Location = new System.Drawing.Point(230, 20);
            this.tbPassword.MaxLength = 16;
            this.tbPassword.Name = "tbPassword";
            this.tbPassword.PasswordChar = '*';
            this.tbPassword.Size = new System.Drawing.Size(79, 21);
            this.tbPassword.TabIndex = 15;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(163, 23);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(61, 12);
            this.label4.TabIndex = 14;
            this.label4.Text = "비밀번호: ";
            // 
            // listBoxRoomChatMsg
            // 
            this.listBoxRoomChatMsg.FormattingEnabled = true;
            this.listBoxRoomChatMsg.HorizontalScrollbar = true;
            this.listBoxRoomChatMsg.ItemHeight = 12;
            this.listBoxRoomChatMsg.Location = new System.Drawing.Point(107, 72);
            this.listBoxRoomChatMsg.Name = "listBoxRoomChatMsg";
            this.listBoxRoomChatMsg.ScrollAlwaysVisible = true;
            this.listBoxRoomChatMsg.Size = new System.Drawing.Size(395, 208);
            this.listBoxRoomChatMsg.TabIndex = 9;
            // 
            // textBoxChatMessage
            // 
            this.textBoxChatMessage.Location = new System.Drawing.Point(107, 287);
            this.textBoxChatMessage.Name = "textBoxChatMessage";
            this.textBoxChatMessage.Size = new System.Drawing.Size(343, 21);
            this.textBoxChatMessage.TabIndex = 11;
            this.textBoxChatMessage.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBoxChatMessage_KeyDown);
            // 
            // btnChat
            // 
            this.btnChat.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnChat.Location = new System.Drawing.Point(456, 285);
            this.btnChat.Name = "btnChat";
            this.btnChat.Size = new System.Drawing.Size(46, 23);
            this.btnChat.TabIndex = 12;
            this.btnChat.Text = "chat";
            this.btnChat.UseVisualStyleBackColor = true;
            this.btnChat.Click += new System.EventHandler(this.btnChat_Click);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(422, 25);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(41, 12);
            this.label5.TabIndex = 13;
            this.label5.Text = "Delay:";
            // 
            // labelDelayTime
            // 
            this.labelDelayTime.AutoSize = true;
            this.labelDelayTime.Location = new System.Drawing.Point(469, 25);
            this.labelDelayTime.Name = "labelDelayTime";
            this.labelDelayTime.Size = new System.Drawing.Size(33, 12);
            this.labelDelayTime.TabIndex = 14;
            this.labelDelayTime.Text = "0 ms";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.textBoxChatMessage);
            this.groupBox3.Controls.Add(this.labelDelayTime);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.labelRoomUserCount);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.listBoxUserList);
            this.groupBox3.Controls.Add(this.btnRoomLeave);
            this.groupBox3.Controls.Add(this.btnRoomEnter);
            this.groupBox3.Controls.Add(this.btnChat);
            this.groupBox3.Controls.Add(this.tbRoomNumber);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.listBoxRoomChatMsg);
            this.groupBox3.Location = new System.Drawing.Point(12, 128);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(508, 317);
            this.groupBox3.TabIndex = 15;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "채팅방";
            // 
            // labelRoomUserCount
            // 
            this.labelRoomUserCount.AutoSize = true;
            this.labelRoomUserCount.Location = new System.Drawing.Point(401, 25);
            this.labelRoomUserCount.Name = "labelRoomUserCount";
            this.labelRoomUserCount.Size = new System.Drawing.Size(11, 12);
            this.labelRoomUserCount.TabIndex = 19;
            this.labelRoomUserCount.Text = "0";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(346, 25);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(49, 12);
            this.label11.TabIndex = 18;
            this.label11.Text = "방 인원:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label7.Location = new System.Drawing.Point(6, 57);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(77, 12);
            this.label7.TabIndex = 14;
            this.label7.Text = "유저 리스트: ";
            // 
            // listBoxUserList
            // 
            this.listBoxUserList.FormattingEnabled = true;
            this.listBoxUserList.ItemHeight = 12;
            this.listBoxUserList.Location = new System.Drawing.Point(6, 72);
            this.listBoxUserList.Name = "listBoxUserList";
            this.listBoxUserList.Size = new System.Drawing.Size(94, 232);
            this.listBoxUserList.TabIndex = 13;
            // 
            // btnRoomLeave
            // 
            this.btnRoomLeave.Location = new System.Drawing.Point(247, 20);
            this.btnRoomLeave.Name = "btnRoomLeave";
            this.btnRoomLeave.Size = new System.Drawing.Size(75, 23);
            this.btnRoomLeave.TabIndex = 3;
            this.btnRoomLeave.Text = "방 퇴장";
            this.btnRoomLeave.UseVisualStyleBackColor = true;
            this.btnRoomLeave.Click += new System.EventHandler(this.btnRoomLeave_Click);
            // 
            // btnRoomEnter
            // 
            this.btnRoomEnter.Location = new System.Drawing.Point(166, 20);
            this.btnRoomEnter.Name = "btnRoomEnter";
            this.btnRoomEnter.Size = new System.Drawing.Size(75, 23);
            this.btnRoomEnter.TabIndex = 2;
            this.btnRoomEnter.Text = "방 입장";
            this.btnRoomEnter.UseVisualStyleBackColor = true;
            this.btnRoomEnter.Click += new System.EventHandler(this.btnRoomEnter_Click);
            // 
            // tbRoomNumber
            // 
            this.tbRoomNumber.Location = new System.Drawing.Point(65, 20);
            this.tbRoomNumber.Name = "tbRoomNumber";
            this.tbRoomNumber.Size = new System.Drawing.Size(95, 21);
            this.tbRoomNumber.TabIndex = 1;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.label6.Location = new System.Drawing.Point(6, 25);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(53, 12);
            this.label6.TabIndex = 0;
            this.label6.Text = "방 번호: ";
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(532, 714);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.listBoxLog);
            this.Controls.Add(this.btnDisconnect);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.groupBox3);
            this.Name = "Form1";
            this.Text = "네트워크 테스트 클라이언트";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox checkBoxLocalHost;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.TextBox textBoxPort;
        private System.Windows.Forms.TextBox textBoxIP;
        private System.Windows.Forms.Button btnDisconnect;
        private System.Windows.Forms.ListBox listBoxLog;
        private System.Windows.Forms.TextBox tbUserId;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ListBox listBoxRoomChatMsg;
        private System.Windows.Forms.TextBox textBoxChatMessage;
        private System.Windows.Forms.Button btnChat;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label labelDelayTime;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnRoomEnter;
        private System.Windows.Forms.TextBox tbRoomNumber;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnRoomLeave;
        private System.Windows.Forms.Label labelRoomUserCount;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ListBox listBoxUserList;
        private System.Windows.Forms.TextBox tbPassword;
        private System.Windows.Forms.Button btnLogin;
        private System.Windows.Forms.Button btnLogout;
    }
}

