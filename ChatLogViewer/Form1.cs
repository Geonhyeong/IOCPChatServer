using System;
using System.Data;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace ChatLogViewer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            DataSet dataSet = GetData();

            dataGridView1.DataSource = dataSet.Tables[0];
        }

        private DataSet GetData()
        {
            string connectionString = "Server=(localdb)\\MSSQLLocalDB;Database=ChatDB;";
            SqlConnection conn = new SqlConnection(connectionString);
            SqlDataAdapter adapter = new SqlDataAdapter("SELECT * FROM [dbo].[ChatLog]", conn);

            DataSet dataSet = new DataSet();
            adapter.Fill(dataSet);

            return dataSet;
        }
    }
}
