using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WeChatBot.BLL
{
    public static class Injector
    {
        public static IntPtr Inject(int pid, string pathStr)
        {
            if (File.Exists(pathStr) == false)
            {
                throw new Exception($"{pathStr}文件不存在");
            }
            FileInfo fileInfo = new FileInfo(pathStr);

            if (pid == 0)
            {
                throw new Exception("微信没有正确启动");
            }
            else
            {
                int hProcess = Win32Api.OpenProcess(0xFFFF, 0, pid);
                if (hProcess == 0)
                {
                    throw new Exception("进程打开失败，可能权限不足或关闭了应用");
                }
                //在微信中申请内存
                var dllAddr =
                    Win32Api.VirtualAllocEx((IntPtr)hProcess, IntPtr.Zero, (uint)pathStr.Length, AllocationType.Commit, MemoryProtection.ReadWrite);
                if (dllAddr == IntPtr.Zero)
                {
                    throw new Exception("内存分配失败");
                }

                var a = Encoding.Default.GetBytes(pathStr);
                bool success = Win32Api.WriteProcessMemory((IntPtr)hProcess, dllAddr, a, (uint)a.Length, out var dummy);
                if (success == false)
                {
                    throw new Exception("路径写入失败");
                }

                var k32 = Win32Api.GetModuleHandle("Kernel32.dll");
                var loadAddr = Win32Api.GetProcAddress(k32, "LoadLibraryA");
                var exeH = Win32Api.CreateRemoteThread((IntPtr)hProcess, IntPtr.Zero, 0, (IntPtr)loadAddr, dllAddr, 0, out var tid);
                if (null == exeH)
                {
                    throw new Exception("注入失败");
                }
                var aaa = Win32Api.WaitForSingleObject((int)exeH, int.MaxValue);
                if (Win32Api.GetExitCodeThread(exeH, out var exitCode) == false)
                {
                    throw new Exception("GetExitCodeThread失败");
                }
                var des = new DllDescription(fileInfo.Name, pathStr, exitCode);
                DllManager.Add(des);
                if (Win32Api.VirtualFreeEx((IntPtr)hProcess, dllAddr, IntPtr.Zero, (IntPtr)FreeType.MEM_RELEASE) == false)
                {
                    var error = Marshal.GetLastWin32Error();
                    throw new Exception("释放内存失败");
                }
                if (Win32Api.CloseHandle(exeH) == false)
                {
                    throw new Exception("关闭远程线程句柄失败");
                }
                if (Win32Api.CloseHandle((IntPtr)hProcess) == false)
                {
                    throw new Exception("关闭微信句柄失败");
                }
                return exeH;
            }
        }

        public static IntPtr Eject(int pid, DllDescription item)
        {
            var freeFunc = Win32Api.GetProcAddress(Win32Api.GetModuleHandle("kernel32.dll"), "FreeLibrary");
            var hProcess = Win32Api.OpenProcess(0xffff, 0, pid);
            var h = Win32Api.CreateRemoteThread((IntPtr)hProcess, IntPtr.Zero, 0, (IntPtr)freeFunc, (IntPtr)item.BaseAddr, 0, out var tid);
            return h;
        }
    }
}
