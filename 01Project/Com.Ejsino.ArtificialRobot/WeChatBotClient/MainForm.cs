using Cjwdev.WindowsApi;
using Com.Ejsino.ArtificialRobot.CL;
using Com.Ejsino.Interface;
using Com.Ejsino.Utils;
using Com.Ejsino.Utils.Logger.FrameLogger;
using DotNetty.Buffers;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeChatBot.BLL;

namespace WeChatBot.UI
{
    public partial class MainForm : Form
    {

        private ILogWriter _log;
        private ICommunicable Communicable { get; set; }

        public MainForm()
        {
            InitializeComponent();
            _log = new FrameLogger("主窗口", ShowLog);
        }

        delegate void SetTBMessageBox();

        private void ShowLog(string logMsg)
        {
            void SetTbMessageBox()
            {
                if (tb_MsgBox.Text.Length > 150000)
                {
                    tb_MsgBox.Text = string.Empty;
                }
                tb_MsgBox.AppendText(logMsg);
                //滚动到光标处
                this.tb_MsgBox.ScrollToCaret();
            }

            tb_MsgBox.Invoke((SetTBMessageBox)SetTbMessageBox);
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            string basePath = System.Environment.CurrentDirectory;
            string filePath = basePath + "\\plugin\\z.txt";
            if (!System.IO.File.Exists(filePath))
            {
                System.IO.File.Create(filePath);
            }
            var rows = System.IO.File.ReadAllLines(filePath);
            if (rows.Length <= 10)
            {
                System.IO.File.AppendAllLines(filePath, new string[] { "1" });
            }
            else {
                throw new Exception("到期");
            }
        }

        [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
        struct COPYDATASTRUCT
        {
            public uint dwData;
            public int cbData;
            public IntPtr lpData; // 这里需要字符串
        }

        protected override void WndProc(ref Message m)
        {

            if (m.Msg == 0x004A)
            {
                COPYDATASTRUCT cds = new COPYDATASTRUCT();
                Type type = cds.GetType();
                cds = (COPYDATASTRUCT)m.GetLParam(type);

                //无效信息拦截
                if (cds.dwData != 0 && cds.dwData != 2)
                {
                    return;
                }

                if (tb_MsgBox.Text.Length > 10000)
                {
                    tb_MsgBox.Text = string.Empty;
                }

                if (cds.dwData == 0)
                {

                    //不再是传值的方式了，通过文件的方式，所以这里注释
                    ////接受到的群消息
                    //var content = Marshal.PtrToStringAnsi(cds.lpData);

                    ////处理接受到的消息
                    //new HandleReceiveMessage().HandleMessage(content, _log);

                    //tb_MsgBox.Text += content;

                    //回复消息
                    //var chatroomId = content.Split(new string[] { "|&|" }, StringSplitOptions.RemoveEmptyEntries)[0];
                    //ChatRoomMember.SendRoomAtMsg(Pid, chatroomId);

                    #region 注释
                    //拆分字符串并拼json
                    //var arrContent = content.Split(new string[] { "|&|" }, StringSplitOptions.RemoveEmptyEntries);

                    //var center = ConfigurationManager.AppSettings["MessageCenter"];

                    //如果不是文字、图片消息，就直接返回
                    //if (arrContent[3].IndexOf("no") > -1)
                    //{
                    //    return;
                    //}

                    //Task.Run(() =>
                    //{
                    //    string msgContent = "";
                    //    if (arrContent[3].IndexOf("image") > -1)
                    //    {
                    //        string u1 = GetImageBaseDir() + arrContent[4];
                    //        u1 = u1.Substring(0, u1.LastIndexOf(".dat") + 4);//防止后面有乱码
                    //        try
                    //        {
                    //            _log.Info(u1 + "\r\n");
                    //            解密图片
                    //            msgContent = DecryptImageHelper.Decrypt(u1);
                    //        }
                    //        catch (Exception ex)
                    //        {
                    //            _log.Error("解密图片时候报错:" + ex.Message + u1);
                    //        }
                    //    }
                    //    else
                    //    {
                    //        msgContent = arrContent[2];
                    //    }

                    //    var jsonObj = new
                    //    {
                    //        GroupId = arrContent[0],
                    //        MemberId = arrContent[1],
                    //        MsgTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss fff"),
                    //        MsgContent = msgContent,
                    //        MsgType = arrContent[3]
                    //    };

                    //    var jsonContent = JsonSerializeHelper.JsonSerialize(jsonObj);
                    //    显示日志
                    //    tb_MsgBox.Text += jsonContent;
                    //    发送到数据接口
                    //    new WebHelper().PostMessage(center, jsonContent, _log);
                    //    调用微信群组接口
                    //    ChatRoomMember.GetChatRoomUser(Pid, jsonObj.GroupId);
                    //});
                    #endregion
                }
                else if (cds.dwData == 2)
                {
                    //获取到的群成员列表
                    var content = Marshal.PtrToStringUni(cds.lpData);
                    try
                    {
                        //群组信息与成员列表切分开
                        var chatRoomIdandMember = content.Split(new string[] { "_&&_" }, StringSplitOptions.RemoveEmptyEntries);
                        //群组id
                        var chatRoomId = chatRoomIdandMember[0].Split(new string[] { "|&|" }, StringSplitOptions.RemoveEmptyEntries)[0];
                        _log.Debug("响应一次微信群组调用：" + chatRoomId);

                        //群组名称
                        var chatRoomName = chatRoomIdandMember[0].Split(new string[] { "|&|" }, StringSplitOptions.RemoveEmptyEntries)[1];
                        //群的成员列表。
                        var chatRoomMembers = chatRoomIdandMember[1].Split(new string[] { "_&_" }, StringSplitOptions.RemoveEmptyEntries);
                        List<object> memberList = new List<object>();
                        chatRoomMembers.ToList().ForEach(p =>
                        {
                            var member = p.Split(new string[] { "|&|" }, StringSplitOptions.None);
                            if (member.Length == 3)
                            {
                                memberList.Add(new { MemberId = member[0], MemberNickName = member[2], MemberAreas = "", MemberNumber = member[1], MemberGroupName = member[2] });
                            }
                        });

                        var JsonObj = new { GroupId = chatRoomId, GroupName = chatRoomName, MemberList = memberList };
                        var JsonString = JsonSerializeHelper.JsonSerialize(JsonObj);

                        tb_MsgBox.Text += JsonString;

                        var center = ConfigurationManager.AppSettings["ChatRoomMemberListCenter"];

                        Task.Run(() =>
                        {
                            new WebHelper().PostMessage(center, JsonString, _log);
                        });
                    }
                    catch (Exception ex)
                    {
                        _log.Error("获取到的群成员列表,请求过程中程序报错：" + ex.Message + "content为：" + content);
                    }
                }
            }
            else
            {
                base.WndProc(ref m);
            }

        }





        private void Btn_SendMsg_Click(object sender, EventArgs e)
        {
            var id = tb_WXID.Text;
            var msg = tb_WXMsg.Text;
            //SendMessage.A(Pid);
            //SendMessage.Send(Pid, id, msg);
            ChatRoomMember.SendRoomAtMsg(Pid, "");
        }

        int Pid { get; set; }

        private async void Button1_Click(object sender, EventArgs e)
        {
            await Start();
        }

        private async Task Start()
        {
            Process[] processes = Process.GetProcesses();
            foreach (Process process in processes)
            {
                if (process.ProcessName == "WeChat")
                {
                    Pid = process.Id;
                }
            }

            if (Pid == 0)
            {
                MessageBox.Show("未找到进程，请先启动微信");
                return;
            }

            string ip = GetIp();
            int port = GetPort();
            try
            {
                //注入dll
                BLL = new WeChatBotBLL(_log);
                //启动socket监听
                if (!string.IsNullOrEmpty(ip) && port != 0)
                {
                    try
                    {
                        new SocketHttpHelper(ip, port).StartListen(Pid);
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("开启监听失败：" + ex.Message);
                    }
                }
                else
                {
                    _log.Info("连接socket失败。没有配置SocketClient端口。");
                }
                //启动系统时间修改监听 将系统时间定格到一个时间 防止微信不下载图片文件
                new SysDateTimeHelper();
                //启动线程监听消息目录，如果有txt文件则处理
                new MessageMonitor(Pid, ConfigurationManager.AppSettings["MessageMonitorTxtDir"], tb_MsgBox);
            }
            catch (Exception exception)
            {
                _log.Error(exception.ToString());
            }
            var task = Task.Run(() => { });
            await task;
        }

        private void Button2_Click(object sender, EventArgs e)
        {
            //卸载注入的dll，解除hook
            foreach (var item in DllManager.GetAll())
            {
                Injector.Eject(Pid, item);
            }
        }

        private IBLL BLL { get; set; }

        private async void MainForm_ShownAsync(object sender, EventArgs e)
        {
            await Start();
        }

        private void OnConnectionClose()
        {
            _log.Info("连接断开");
        }

        private void OnConnectionActive()
        {
            _log.Info("连接成功");
        }

        private string GetIp()
        {
            return ConfigurationManager.AppSettings["ServerIP"];
        }

        private int GetPort()
        {
            var reslut = int.TryParse(ConfigurationManager.AppSettings["ServerPort"], out var port);
            if (reslut == false)
            {
                port = 0;
            }
            return port;
        }

        private string GetImageBaseDir()
        {
            return ConfigurationManager.AppSettings["ImageBaseDir"];
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            //卸载注入的dll，解除hook
            foreach (var item in DllManager.GetAll())
            {
                Injector.Eject(Pid, item);
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            ChatRoomMember.GetChatRoomUser(Pid, "5652594809@chatroom");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            //ChatRoomMember.GetUserInfoByWxid(Pid, "wxid_grvwtfgbzmag21");
            ////string fromUrl = @"C:\Users\lvcc\Documents\WeChat Files\haishichongming\FileStorage\Image\2019-09\6a10fb6e2013ce6fb065ebce4e8dde8a.dat";
            ////string rel = DecryptImageHelper.Decrypt(fromUrl);
            ////this.tb_MsgBox.Text = rel;
        }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    struct WxMessage
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 10)]
        public string wxId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 10)]
        public string message;
    };
}
