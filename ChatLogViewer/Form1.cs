using System;
using System.Data;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace ChatLogViewer
{
    public partial class Form1 : Form
    {
        private string connectionString = "Server=(localdb)\\MSSQLLocalDB;Database=ChatDB;";

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            GetData();
        }

        private void GetData()
        {
            SqlConnection conn = new SqlConnection(connectionString);
            SqlCommand sqlCommand = new SqlCommand("SELECT * FROM [dbo].[ChatLog]", conn);

            try
            {
                SqlDataAdapter adapter = new SqlDataAdapter();
                adapter.SelectCommand = sqlCommand;
                
                DataTable dataSet = new DataTable();
                adapter.Fill(dataSet);

                dataGridView.DataSource = dataSet;
                resultLabel.Text = $"검색 결과 : {dataGridView.RowCount} 행";
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void btnSearch_Click(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection(connectionString);
            string cmd = "SELECT * FROM [dbo].[ChatLog]";

            if (tbNickname.Text != "")
            {
                cmd += " WHERE Nickname like N\'%" + tbNickname.Text + "%\'";

                if (tbMsg.Text != "")
                    cmd += " and ChatMsg like N\'%" + tbMsg.Text + "%\'";
            }
            else
            {
                if (tbMsg.Text != "")
                    cmd += " WHERE ChatMsg like N\'%" + tbMsg.Text + "%\'";
            }

            try
            {
                SqlDataAdapter adapter = new SqlDataAdapter();
                adapter.SelectCommand = new SqlCommand(cmd, conn);

                DataTable dataSet = new DataTable();
                adapter.Fill(dataSet);

                dataGridView.DataSource = dataSet;
                resultLabel.Text = $"검색 결과 : {dataGridView.RowCount} 행";
            }
            catch (SqlException ex)
            {
                MessageBox.Show(ex.Message);
            }
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            GetData();
            tbNickname.Text = "";
            tbMsg.Text = "";
        }
    }
}
