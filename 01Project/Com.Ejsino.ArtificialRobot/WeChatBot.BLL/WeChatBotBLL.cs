using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Com.Ejsino.Interface;
using DotNetty.Handlers.Timeout;
using WeChatBot.BLL.Handlers;

namespace WeChatBot.BLL
{
    public class WeChatBotBLL : IBLL
    {
        private readonly ILogWriter log;

        private string PluginBaseDic
        {
            get => Path.Combine(Directory.GetCurrentDirectory(), "plugin");
        }

        string[] sPathStr = {
            "ReceiveMessage.dll" ,
            "ChatRoomMember.dll"
            //,"SendMessage.dll"
             

};

        public WeChatBotBLL(ILogWriter log)
        {
            this.log = log;
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
                throw new Exception("未找到进程，请先启动微信");
            }

            //注入，Hook。Hook是要在Dll中自动Hook，还是要在这里主动Hook展示Hook的结果呢？
            foreach (var item in sPathStr)
            {
                Injector.Inject(Pid, Path.Combine(PluginBaseDic, item));
            }
        }

        public int Pid { get; private set; }

        public async Task<string> ExecuteAsync(string request)
        {
            SendMessage.Send(Pid, "cwb_100", "天青色等烟雨");
            return string.Empty;
        }

        public List<object> GetHandlers()
        {
            List<object> handlers =
                new List<object>()
                {
                    new Tuple<string,Tuple<Type,object[]>>(
                        "IdleStateHandler",
                        new Tuple<Type, object[]>(typeof(IdleStateHandler),new object[]{TimeSpan.Zero, TimeSpan.FromSeconds(5), TimeSpan.Zero})),
                    new Tuple<string,Tuple<Type,object[]>>(
                        "HeartbeatHandler",
                        new Tuple<Type, object[]>(typeof(HeartbeatHandler),new object[]{log})),
                    new Tuple<string,Tuple<Type,object[]>>(
                        "client",
                        new Tuple<Type, object[]>(typeof(RobotHandler),new object[]{this, log})),
                };
            return handlers;
        }

        public void OnUpdate()
        {
            throw new NotImplementedException();
        }
    }
}
