using Com.Ejsino.Interface;
using Com.Ejsino.Utils;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using WeChatBot.BLL;

namespace WeChatBot.UI
{
    public class HandleReceiveMessage
    {
        public void HandleMessage(int Pid, string content, ILogWriter _log)
        {

            _log.Info("收到一条消息处理请求：" + content);
            //拆分字符串并拼json
            var arrContent = content.Split(new string[] { "|&|" }, StringSplitOptions.RemoveEmptyEntries);

            var center = ConfigurationManager.AppSettings["MessageCenter"];

            //如果不是文字、图片消息，就直接返回
            if (arrContent[3].IndexOf("no") > -1)
            {
                return;
            }

            Task.Run(() =>
            {
                string msgContent = "";
                if (arrContent[3].IndexOf("image") > -1)
                {
                    string u1 = GetImageBaseDir() + arrContent[4];
                    u1 = u1.Substring(0, u1.LastIndexOf(".dat") + 4);//防止后面有乱码
                    try
                    {
                        _log.Info(u1 + "\r\n");
                        //解密图片
                        msgContent = DecryptImageHelper.Decrypt(u1);
                    }
                    catch (Exception ex)
                    {
                        _log.Error("解密图片时候报错:" + ex.Message + u1);
                    }
                }
                else
                {
                    msgContent = arrContent[2];
                }

                var jsonObj = new
                {
                    GroupId = arrContent[0],
                    MemberId = arrContent[1],
                    MsgTime = DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss fff"),
                    MsgContent = msgContent,
                    MsgType = arrContent[3]
                };
                _log.Fatal(msgContent);
                var jsonContent = JsonSerializeHelper.JsonSerialize(jsonObj);
                //发送到数据接口
                //new WebHelper().PostMessage(center, jsonContent, _log);
                //调用微信群组接口
                //ChatRoomMember.GetChatRoomUser(Pid, jsonObj.GroupId);
                ChatRoomMember.SendRoomAtMsg(Pid, arrContent[0]);
            });
        }
        private string GetImageBaseDir()
        {
            return ConfigurationManager.AppSettings["ImageBaseDir"];
        }
    }
}
