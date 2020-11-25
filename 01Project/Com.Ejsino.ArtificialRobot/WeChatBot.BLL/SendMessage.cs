using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WeChatBot.BLL
{
    public class SendMessage
    {
        public static void Send(int pid, string wXid, string message)
        {
            var des = DllManager.Get(nameof(SendMessage) + ".dll");
            var baseAddr = Win32Api.LoadLibrary(des.Path);
            var funcAddr = Win32Api.GetProcAddress(baseAddr, "SendTextMessage");
            var p = funcAddr - baseAddr + des.BaseAddr;
            int hProcess = Win32Api.OpenProcess(0xFFFF, 0, pid);
            string a = $"{wXid}_ejflag_{message}";
            var c = setByte2(a);
            var dllAddr =
                    Win32Api.VirtualAllocEx((IntPtr)hProcess, IntPtr.Zero, (uint)c.Length, AllocationType.Commit, MemoryProtection.ReadWrite);
            bool success = Win32Api.WriteProcessMemory((IntPtr)hProcess, dllAddr, c, (uint)c.Length, out var dummy);
            var exeH = Win32Api.CreateRemoteThread((IntPtr)hProcess, IntPtr.Zero, 0, (IntPtr)p, dllAddr, 0, out var tid);
            Win32Api.FreeLibrary(baseAddr);
        }

        /// <summary>
        /// 多字节码转为宽字符
        /// </summary>
        /// <param name="str"></param>
        /// <returns></returns>
        public static byte[] setByte2(string str)
        {
            byte[] b = Encoding.Default.GetBytes(str);
            byte[] unicodeBytes = Encoding.Convert(Encoding.Default, Encoding.Unicode, b);
            byte[] s = new byte[unicodeBytes.Length + 2];
            for (int i = 0; i < unicodeBytes.Length; i++)
            {
                s[i] = unicodeBytes[i];
            }
            s[s.Length - 1] = 0;
            s[s.Length - 2] = 0;
            return s;
        }
    }
}
