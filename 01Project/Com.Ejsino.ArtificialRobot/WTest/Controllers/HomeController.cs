using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Web;
using System.Web.Mvc;

namespace WTest.Controllers
{
    public class HomeController : Controller
    {
        public ActionResult Index()
        {
            return View();
        }

        /// <summary>
        /// 发起请求，获取最新群成员列表
        /// </summary>
        /// <returns></returns>
        public ActionResult About(string a)
        {
            int port = 8123;
            string host = "127.0.0.1";//服务器端ip地址 127.0.0.1  39.108.126.47
            string recStr = RequestChatRoomMember(host, port, a);//测试群   21689047908@chatroom  //5652594809@chatroom 老熟人
            ViewBag.Message = recStr;
            return View();
        }

        /// <summary>
        /// 发送获取微信群组成员的请求
        /// </summary>
        /// <param name="host">ip</param>
        /// <param name="port">端口</param>
        /// <param name="roomId">群组id</param>
        /// <returns></returns>
        private string RequestChatRoomMember(string host, int port, string roomId) {
            IPAddress ip = IPAddress.Parse(host);
            IPEndPoint ipe = new IPEndPoint(ip, port);

            Socket clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect(ipe);

            //send message
            string sendStr = roomId;
            byte[] sendBytes = Encoding.ASCII.GetBytes(sendStr);
            clientSocket.Send(sendBytes);

            //receive message
            string recStr = "";
            byte[] recBytes = new byte[4096];
            int bytes = clientSocket.Receive(recBytes, recBytes.Length, 0);
            recStr += Encoding.ASCII.GetString(recBytes, 0, bytes);

            //返回值格式：如果成功：{"status":"success","errorMessage":""}；如果失败：{"status":"error","errorMessage":"错误信息"}
            return recStr;
        }



        /// <summary>
        /// 接收实时发送的微信聊天记录
        /// </summary>
        /// <param name="Model"></param>
        /// <returns></returns>
        public ActionResult ChatMessage(ChatMessageModel Model)
        {
            //解析json
            var modelStr = JsonConvert.SerializeObject(Model);
            //写日志
            System.IO.File.AppendAllText(AppDomain.CurrentDomain.BaseDirectory + "/ChatMessage_log.txt", modelStr+"\r\n");
            return Content("success");
        }

        /// <summary>
        /// 微信群组聊天记录的对象传输模型
        /// </summary>
        public class ChatMessageModel
        {
            public string GroupId { get; set; }
            public string MemberId { get; set; }
            public DateTime MsgTime { get; set; }
            public string MsgContent { get; set; }
            /// <summary>
            /// word或image
            /// </summary>
            public string MsgType { get; set; }
        }









        public ActionResult ChatRoomDetail(ChatRoomDetailModel Model)
        {
            //解析json
            var modelStr = JsonConvert.SerializeObject(Model);
            //写日志
            //System.IO.File.AppendAllText("C:\\07新奥集团\\WebSite\\ChatRoomDetailModelLog.txt", modelStr);
            return Content("success");
        }
        /// <summary>
        /// 微信群组信息传输模型
        /// </summary>
        public class ChatRoomDetailModel {
            /// <summary>
            /// 群组id
            /// </summary>
            public string GroupId { get; set; }
            /// <summary>
            /// 群组名称
            /// </summary>
            public string GroupName { get; set; }
            /// <summary>
            /// 群成员列表
            /// </summary>
            public List<ChatMemberModel> MemberList { get; set; }
        }
        /// <summary>
        /// 微信群组内成员模型
        /// </summary>
        public class ChatMemberModel {
            /// <summary>
            /// 微信id
            /// </summary>
            public string MemberId { get; set; }
            /// <summary>
            /// 微信号
            /// </summary>
            public string MemberNumber { get; set; }
            /// <summary>
            /// 微信昵称
            /// </summary>
            public string MemberNickName { get; set; }
            /// <summary>
            /// 所属地区
            /// </summary>
            public string MemberAreas { get; set; }
            
            /// <summary>
            /// 微信群名称
            /// </summary>
            public string MemberGroupName { get; set; }
        }
    }
}