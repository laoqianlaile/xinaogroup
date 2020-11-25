using System;

using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WeChatBot.UI
{
    /// <summary>
    /// 解密微信图片
    /// </summary>
    public static class DecryptImageHelper
    {
        public static int SecretKey = 0xE9;//开发机器 7C 客户的服务器 1C  客户2 e9
        public static string Decrypt(string inputPath)
        {
            Thread.Sleep(3000);
            if (!File.Exists(inputPath))
            {
                throw new Exception("没有找到对应的文件");
            }
            //////先移动到其他目录再做读取操作
            //string moveFile = Path.GetDirectoryName(inputPath) + "\\"+Guid.NewGuid() + ".jpg";
            //File.Copy(inputPath, moveFile);

            //using (FileStream fs = new FileStream(moveFile, FileMode.Open))
            //{
            //    byte[] buffer = new byte[fs.Length];
            //    fs.Read(buffer, 0, (int)fs.Length);
            //    string str = Convert.ToBase64String(buffer);
            //    return "data:image/jpeg;base64," + str;
            //}


            //目录输出基址
            string OutPutBaseDir = ConfigurationManager.AppSettings["OutPutBaseDir"];
            //网站根目录
            string WebSiteDomain = ConfigurationManager.AppSettings["webSiteDomain"];

            string relativePath = DateTime.Now.ToString("yyyy-MM-dd") + "/" + Guid.NewGuid() + ".jpg";
            string fileOutPath = OutPutBaseDir + relativePath;

            //没有目录创建目录
            if (!Directory.Exists(Path.GetDirectoryName(fileOutPath)))
            {
                Directory.CreateDirectory(Path.GetDirectoryName(fileOutPath));
            }

            using (FileStream fin = File.OpenRead(inputPath), fout = File.OpenWrite(fileOutPath))
            {
                byte[] bIn = new byte[fin.Length];
                fin.Read(bIn, 0, (int)fin.Length);

                byte[] bOut = new byte[bIn.Length];

                for (int i = 0; i < bIn.Length; i++)
                {
                    bOut[i] = (byte)(bIn[i] ^ SecretKey);
                }
                //string img64Str = Convert.ToBase64String(bOut);
                //return "data:image/jpeg;base64," + img64Str;
                fout.Write(bOut, 0, bOut.Length);//写到本地磁盘做测试

                return WebSiteDomain + relativePath;
            }

        }
    }
}
