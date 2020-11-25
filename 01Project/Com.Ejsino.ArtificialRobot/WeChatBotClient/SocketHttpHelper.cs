using Com.Ejsino.Interface;
using Com.Ejsino.Utils;
using Com.Ejsino.Utils.Logger.FrameLogger;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using WeChatBot.BLL;

namespace WeChatBot.UI
{
   
    public class SocketHttpHelper
    {
        //日志
        private ILogWriter _log = new FrameLogger("socket", null);


        private string ip = "";
        private int port = 8123;
        private int count = 0;
        private int pid = 0;//微信进程id
        private Socket server = null;

        public string DefaultReturn = string.Empty;



        public SocketHttpHelper(string ip, int port)
        {
            this.ip = ip;
            this.port = port;
        }

        public void StartListen(int pid, int count = 10)
        {
            this.pid = pid;
            this.count = count;
            Thread t = new Thread(new ThreadStart(ProcessThread));
            t.IsBackground = true;
            t.Start();
            //var task = Task.Run(() =>
            //{
            //    ProcessThread();
            //});
            //return task;
        }

        public void CloseSocket()
        {
            try
            {
                server.Close();
            }
            catch { }
        }

        private void ProcessThread()
        {
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            server.Bind(new System.Net.IPEndPoint(System.Net.IPAddress.Parse(ip), port));
            server.Listen(count);
            while (true)
            {
                try
                {
                    Socket client = server.Accept();
                    ThreadPool.QueueUserWorkItem(new WaitCallback(ListenExecute), client);
                }
                catch { }
                finally
                {
                }
            }
        }

        private void ListenExecute(object obj)
        {

            Socket client = obj as Socket;
            try
            {
                string ip = (client.RemoteEndPoint as System.Net.IPEndPoint).Address.ToString();
                byte[] buffer = new byte[1024];
                int count = client.Receive(buffer);
                if (count > 0)
                {
                    string content = Encoding.UTF8.GetString(buffer, 0, count);
                    if (content.IndexOf("@chatroom")==-1)
                    {
                        return;
                    }
                    //_log.Debug("请求一次微信群组调用：" + content);
                    _log.Debug(DateTime.Now.ToString() + "收到了一次socket请求:http://localhost:83/Home/About?a=" + content);
                    //System.IO.File.AppendAllText("c:/socketLog.txt", "socket:" + ip + ":" + port + "：" + content);
                    ////发起请求，获取微信群组列表
                    ChatRoomMember.GetChatRoomUser(pid, content);
                    try
                    {
                        string data = JsonSerializeHelper.JsonSerialize(new { status = "success", errorMessage = "" });
                        client.Send(Encoding.UTF8.GetBytes(data));
                    }
                    catch (Exception ex)
                    {
                        string data = JsonSerializeHelper.JsonSerialize(new { status = "error", errorMessage = ex.Message });
                        client.Send(Encoding.UTF8.GetBytes(data));
                    }
                    finally
                    {
                    }
                }
            }
            catch { }
            finally
            {
                try
                {
                    client.Shutdown(SocketShutdown.Both);
                    client.Close();
                    client.Dispose();
                }
                catch { }
            }
        }
    }
}
