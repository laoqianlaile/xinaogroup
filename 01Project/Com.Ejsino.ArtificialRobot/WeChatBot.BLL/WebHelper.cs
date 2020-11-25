using Com.Ejsino.Interface;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace WeChatBot.BLL
{
    public class WebHelper
    {
        /// <summary>
        /// 将数据发送到业务系统中
        /// </summary>
        /// <param name="url"></param>
        /// <param name="data"></param>
        public void PostMessage(string url, string data, ILogWriter _log)
        {

            //发送消息到回调接口

            if (string.IsNullOrEmpty(url))
            {
                _log.Info("CenterInterface配置为空");
                return;
            }
            _log.Info($"{DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss fff")} 回调{url}接口。");
            //实例化
            NewWebClient client = new NewWebClient();
            //参数转流
            byte[] bytearray = Encoding.UTF8.GetBytes(data);
            //采取POST方式必须加的header，如果改为GET方式的话就去掉这句话即可
            client.Headers.Add("Content-Type", "application/json;charset=UTF-8");
            client.Headers.Add("ContentLength", bytearray.Length.ToString());//长度

            //上传，post方式，并接收返回数据（这是同步，需要等待接收返回值）

            try
            {
                byte[] responseData = client.UploadData(url, "POST", bytearray);
                //释放
                client.Dispose();
                //处理返回数据
                string rel = Encoding.UTF8.GetString(responseData);


                if (!string.IsNullOrEmpty(rel))
                {
                    _log.Info($"{DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss fff")} 回调{url}接口返回内容：{rel}");
                }
            }
            catch (Exception ex)
            {
                _log.Info($"发送Post数据时出错：{ex.Message}");
            }
        }

        public class NewWebClient : WebClient
        {
            private int _timeout;

            /// <summary>
            /// 超时时间(毫秒)
            /// </summary>
            public int Timeout
            {
                get
                {
                    return _timeout;
                }
                set
                {
                    _timeout = value;
                }
            }

            public NewWebClient()
            {
                this._timeout = 60000;
            }

            public NewWebClient(int timeout)
            {
                this._timeout = timeout;
            }

            protected override WebRequest GetWebRequest(Uri address)
            {
                var result = base.GetWebRequest(address);
                result.Timeout = this._timeout;
                return result;
            }
        }


    }
}
