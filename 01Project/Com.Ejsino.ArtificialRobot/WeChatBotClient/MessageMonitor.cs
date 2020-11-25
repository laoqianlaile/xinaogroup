using Com.Ejsino.Interface;
using Com.Ejsino.Utils.Logger.FrameLogger;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WeChatBot.UI
{
    public class MessageMonitor
    {
        private string _dir { get; set; }
        private ILogWriter log;
        public MessageMonitor(int Pid, string dir, TextBox tb_MsgBox)
        {
            _dir = dir;
            log = new FrameLogger("监测MessageMonitor", null);
            Task.Run(() =>
            {
                while (true)
                {
                    Thread.Sleep(1000);
                    DirectoryInfo baseDirInfo = new DirectoryInfo(_dir);
                    foreach (FileInfo file in baseDirInfo.GetFiles("*.txt"))
                    {
                        try
                        {
                            string content = File.ReadAllText(file.FullName, Encoding.Unicode);
                            //tb_MsgBox.Text = content;
                            new HandleReceiveMessage().HandleMessage(Pid, content, new FrameLogger("消息监听", null));
                        }
                        catch (Exception ex)
                        {
                            log.Error(ex.Message);
                        }
                        finally
                        {
                            File.Delete(file.FullName);
                        }
                    }
                }
            });
        }
        public String ToDBC(String input)
        {
            char[] c = input.ToCharArray();
            for (int i = 0; i < c.Length; i++)
            {
                if (c[i] == 12288)
                {
                    c[i] = (char)32;
                    continue;
                }
                if (c[i] > 65280 && c[i] < 65375)
                    c[i] = (char)(c[i] - 65248);
            }
            return new String(c);
        }
        public static byte[] setByte2(string str, int len)
        {

            byte[] b = Encoding.Default.GetBytes(str);

            byte[] unicodeBytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, b);
            byte[] s = new byte[len];
            for (int i = 0; i < unicodeBytes.Length; i++)
            {
                s[i] = unicodeBytes[i];
            }
            for (int i = unicodeBytes.Length; i < len; i++)
            {
                s[i] = 0;
            }
            return s;
        }
    }
}
