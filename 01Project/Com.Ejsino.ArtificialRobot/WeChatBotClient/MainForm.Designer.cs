namespace WeChatBot.UI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tb_WXID = new System.Windows.Forms.TextBox();
            this.tb_WXMsg = new System.Windows.Forms.TextBox();
            this.btn_SendMsg = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.tb_MsgBox = new System.Windows.Forms.TextBox();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // tb_WXID
            // 
            this.tb_WXID.Location = new System.Drawing.Point(62, 13);
            this.tb_WXID.Name = "tb_WXID";
            this.tb_WXID.Size = new System.Drawing.Size(100, 21);
            this.tb_WXID.TabIndex = 0;
            // 
            // tb_WXMsg
            // 
            this.tb_WXMsg.Location = new System.Drawing.Point(62, 49);
            this.tb_WXMsg.Name = "tb_WXMsg";
            this.tb_WXMsg.Size = new System.Drawing.Size(100, 21);
            this.tb_WXMsg.TabIndex = 1;
            // 
            // btn_SendMsg
            // 
            this.btn_SendMsg.Location = new System.Drawing.Point(62, 90);
            this.btn_SendMsg.Name = "btn_SendMsg";
            this.btn_SendMsg.Size = new System.Drawing.Size(75, 23);
            this.btn_SendMsg.TabIndex = 2;
            this.btn_SendMsg.Text = "发送";
            this.btn_SendMsg.UseVisualStyleBackColor = true;
            this.btn_SendMsg.Click += new System.EventHandler(this.Btn_SendMsg_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(474, 90);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 3;
            this.button1.Text = "开始";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.Button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(577, 90);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 4;
            this.button2.Text = "卸载";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.Button2_Click);
            // 
            // tb_MsgBox
            // 
            this.tb_MsgBox.Location = new System.Drawing.Point(73, 168);
            this.tb_MsgBox.Multiline = true;
            this.tb_MsgBox.Name = "tb_MsgBox";
            this.tb_MsgBox.Size = new System.Drawing.Size(595, 240);
            this.tb_MsgBox.TabIndex = 5;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(484, 13);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 6;
            this.button3.Text = "获取群成员列表";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(577, 13);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(75, 23);
            this.button4.TabIndex = 7;
            this.button4.Text = "获取用户id";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.tb_MsgBox);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btn_SendMsg);
            this.Controls.Add(this.tb_WXMsg);
            this.Controls.Add(this.tb_WXID);
            this.Name = "MainForm";
            this.Text = "WeChatBot";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.Shown += new System.EventHandler(this.MainForm_ShownAsync);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tb_WXID;
        private System.Windows.Forms.TextBox tb_WXMsg;
        private System.Windows.Forms.Button btn_SendMsg;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.TextBox tb_MsgBox;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
    }
}