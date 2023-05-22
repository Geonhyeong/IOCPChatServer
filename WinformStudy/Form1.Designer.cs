
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
            this.textBoxDummyCount = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.btnConnect = new System.Windows.Forms.Button();
            this.btnDisconnect = new System.Windows.Forms.Button();
            this.listBoxLog = new System.Windows.Forms.ListBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnDummyChatStop = new System.Windows.Forms.Button();
            this.btnDummyChatStart = new System.Windows.Forms.Button();
            this.btnDummyConnect = new System.Windows.Forms.Button();
            this.btnDummyDisconnect = new System.Windows.Forms.Button();
            this.listBoxRoomChatMsg = new System.Windows.Forms.ListBox();
            this.textBoxUserName = new System.Windows.Forms.TextBox();
            this.textBoxChatMessage = new System.Windows.Forms.TextBox();
            this.btnChat = new System.Windows.Forms.Button();
            this.btnDummyPlus = new System.Windows.Forms.Button();
            this.btnDummyMinus = new System.Windows.Forms.Button();
            this.comboBoxAddCount = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.labelDelayTime = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
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
            // textBoxDummyCount
            // 
            this.textBoxDummyCount.Location = new System.Drawing.Point(70, 32);
            this.textBoxDummyCount.MaxLength = 5;
            this.textBoxDummyCount.Name = "textBoxDummyCount";
            this.textBoxDummyCount.Size = new System.Drawing.Size(51, 21);
            this.textBoxDummyCount.TabIndex = 8;
            this.textBoxDummyCount.Text = "0";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 35);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(49, 12);
            this.label3.TabIndex = 7;
            this.label3.Text = "더미 수:";
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
            this.listBoxLog.Location = new System.Drawing.Point(12, 414);
            this.listBoxLog.Name = "listBoxLog";
            this.listBoxLog.ScrollAlwaysVisible = true;
            this.listBoxLog.Size = new System.Drawing.Size(508, 268);
            this.listBoxLog.TabIndex = 3;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.comboBoxAddCount);
            this.groupBox2.Controls.Add(this.btnDummyMinus);
            this.groupBox2.Controls.Add(this.btnDummyPlus);
            this.groupBox2.Controls.Add(this.btnDummyChatStop);
            this.groupBox2.Controls.Add(this.btnDummyChatStart);
            this.groupBox2.Controls.Add(this.textBoxDummyCount);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Location = new System.Drawing.Point(13, 69);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(413, 78);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "더미 클라이언트 설정";
            // 
            // btnDummyChatStop
            // 
            this.btnDummyChatStop.Location = new System.Drawing.Point(332, 44);
            this.btnDummyChatStop.Name = "btnDummyChatStop";
            this.btnDummyChatStop.Size = new System.Drawing.Size(75, 23);
            this.btnDummyChatStop.TabIndex = 10;
            this.btnDummyChatStop.Text = "채팅중지";
            this.btnDummyChatStop.UseVisualStyleBackColor = true;
            this.btnDummyChatStop.Click += new System.EventHandler(this.btnDummyChatStop_Click);
            // 
            // btnDummyChatStart
            // 
            this.btnDummyChatStart.Location = new System.Drawing.Point(332, 15);
            this.btnDummyChatStart.Name = "btnDummyChatStart";
            this.btnDummyChatStart.Size = new System.Drawing.Size(75, 23);
            this.btnDummyChatStart.TabIndex = 9;
            this.btnDummyChatStart.Text = "채팅시작";
            this.btnDummyChatStart.UseVisualStyleBackColor = true;
            this.btnDummyChatStart.Click += new System.EventHandler(this.btnDummyChatStart_Click);
            // 
            // btnDummyConnect
            // 
            this.btnDummyConnect.Location = new System.Drawing.Point(432, 84);
            this.btnDummyConnect.Name = "btnDummyConnect";
            this.btnDummyConnect.Size = new System.Drawing.Size(88, 23);
            this.btnDummyConnect.TabIndex = 7;
            this.btnDummyConnect.Text = "더미 생성";
            this.btnDummyConnect.UseVisualStyleBackColor = true;
            this.btnDummyConnect.Click += new System.EventHandler(this.btnDummyConnect_Click);
            // 
            // btnDummyDisconnect
            // 
            this.btnDummyDisconnect.Location = new System.Drawing.Point(432, 113);
            this.btnDummyDisconnect.Name = "btnDummyDisconnect";
            this.btnDummyDisconnect.Size = new System.Drawing.Size(88, 23);
            this.btnDummyDisconnect.TabIndex = 8;
            this.btnDummyDisconnect.Text = "더미 삭제";
            this.btnDummyDisconnect.UseVisualStyleBackColor = true;
            this.btnDummyDisconnect.Click += new System.EventHandler(this.btnDummyDisconnect_Click);
            // 
            // listBoxRoomChatMsg
            // 
            this.listBoxRoomChatMsg.FormattingEnabled = true;
            this.listBoxRoomChatMsg.HorizontalScrollbar = true;
            this.listBoxRoomChatMsg.ItemHeight = 12;
            this.listBoxRoomChatMsg.Location = new System.Drawing.Point(12, 153);
            this.listBoxRoomChatMsg.Name = "listBoxRoomChatMsg";
            this.listBoxRoomChatMsg.ScrollAlwaysVisible = true;
            this.listBoxRoomChatMsg.Size = new System.Drawing.Size(508, 208);
            this.listBoxRoomChatMsg.TabIndex = 9;
            // 
            // textBoxUserName
            // 
            this.textBoxUserName.Location = new System.Drawing.Point(12, 368);
            this.textBoxUserName.Name = "textBoxUserName";
            this.textBoxUserName.Size = new System.Drawing.Size(100, 21);
            this.textBoxUserName.TabIndex = 10;
            this.textBoxUserName.Text = "user_name";
            // 
            // textBoxChatMessage
            // 
            this.textBoxChatMessage.Location = new System.Drawing.Point(119, 368);
            this.textBoxChatMessage.Name = "textBoxChatMessage";
            this.textBoxChatMessage.Size = new System.Drawing.Size(349, 21);
            this.textBoxChatMessage.TabIndex = 11;
            // 
            // btnChat
            // 
            this.btnChat.Font = new System.Drawing.Font("굴림", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.btnChat.Location = new System.Drawing.Point(475, 368);
            this.btnChat.Name = "btnChat";
            this.btnChat.Size = new System.Drawing.Size(45, 23);
            this.btnChat.TabIndex = 12;
            this.btnChat.Text = "chat";
            this.btnChat.UseVisualStyleBackColor = true;
            this.btnChat.Click += new System.EventHandler(this.btnChat_Click);
            // 
            // btnDummyPlus
            // 
            this.btnDummyPlus.Location = new System.Drawing.Point(249, 29);
            this.btnDummyPlus.Name = "btnDummyPlus";
            this.btnDummyPlus.Size = new System.Drawing.Size(25, 25);
            this.btnDummyPlus.TabIndex = 11;
            this.btnDummyPlus.Text = "+";
            this.btnDummyPlus.UseVisualStyleBackColor = true;
            this.btnDummyPlus.Click += new System.EventHandler(this.btnDummyPlus_Click);
            // 
            // btnDummyMinus
            // 
            this.btnDummyMinus.Location = new System.Drawing.Point(280, 29);
            this.btnDummyMinus.Name = "btnDummyMinus";
            this.btnDummyMinus.Size = new System.Drawing.Size(25, 25);
            this.btnDummyMinus.TabIndex = 12;
            this.btnDummyMinus.Text = "-";
            this.btnDummyMinus.UseVisualStyleBackColor = true;
            this.btnDummyMinus.Click += new System.EventHandler(this.btnDummyMinus_Click);
            // 
            // comboBoxAddCount
            // 
            this.comboBoxAddCount.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxAddCount.FormattingEnabled = true;
            this.comboBoxAddCount.Items.AddRange(new object[] {
            "1",
            "5",
            "10",
            "50",
            "100",
            "500"});
            this.comboBoxAddCount.Location = new System.Drawing.Point(194, 32);
            this.comboBoxAddCount.Name = "comboBoxAddCount";
            this.comboBoxAddCount.Size = new System.Drawing.Size(49, 20);
            this.comboBoxAddCount.TabIndex = 13;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(127, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(61, 12);
            this.label4.TabIndex = 14;
            this.label4.Text = "추가 개수:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 689);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(45, 12);
            this.label5.TabIndex = 13;
            this.label5.Text = "Delay :";
            // 
            // labelDelayTime
            // 
            this.labelDelayTime.AutoSize = true;
            this.labelDelayTime.Location = new System.Drawing.Point(63, 689);
            this.labelDelayTime.Name = "labelDelayTime";
            this.labelDelayTime.Size = new System.Drawing.Size(33, 12);
            this.labelDelayTime.TabIndex = 14;
            this.labelDelayTime.Text = "0 ms";
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(532, 714);
            this.Controls.Add(this.labelDelayTime);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.btnChat);
            this.Controls.Add(this.textBoxChatMessage);
            this.Controls.Add(this.textBoxUserName);
            this.Controls.Add(this.listBoxRoomChatMsg);
            this.Controls.Add(this.btnDummyDisconnect);
            this.Controls.Add(this.btnDummyConnect);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.listBoxLog);
            this.Controls.Add(this.btnDisconnect);
            this.Controls.Add(this.btnConnect);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "네트워크 테스트 클라이언트";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

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
        private System.Windows.Forms.TextBox textBoxDummyCount;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnDummyConnect;
        private System.Windows.Forms.Button btnDummyDisconnect;
        private System.Windows.Forms.Button btnDummyChatStop;
        private System.Windows.Forms.Button btnDummyChatStart;
        private System.Windows.Forms.ListBox listBoxRoomChatMsg;
        private System.Windows.Forms.TextBox textBoxUserName;
        private System.Windows.Forms.TextBox textBoxChatMessage;
        private System.Windows.Forms.Button btnChat;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxAddCount;
        private System.Windows.Forms.Button btnDummyMinus;
        private System.Windows.Forms.Button btnDummyPlus;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label labelDelayTime;
    }
}

