using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace WeChatBot.BLL
{
    public class Win32Api
    {
        [DllImport("kernel32.dll", EntryPoint = "OpenProcess")]
        public static extern int OpenProcess(
           int dwDesiredAccess,
           int bInheritHandle,
           int dwProcessId
       );

        [DllImport("kernel32.dll", SetLastError = true, ExactSpelling = true)]
        public static extern IntPtr VirtualAllocEx(
            IntPtr hProcess,
            IntPtr lpAddress,
            uint dwSize,
            AllocationType flAllocationType,
            MemoryProtection flProtect
        );

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool VirtualFreeEx(IntPtr hProcess, IntPtr lpAddress, IntPtr dwSize, IntPtr dwFreeType);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool WriteProcessMemory(
            IntPtr hProcess,
            IntPtr lpBaseAddress,
            byte[] lpBuffer,
            uint nSize,
            out UIntPtr lpNumberOfBytesWritten
        );

        [DllImport("kernel32.dll", EntryPoint = "GetModuleHandle")]
        public static extern int GetModuleHandle(
            string lpModuleName
        );

        [DllImport("kernel32.dll", SetLastError = true, EntryPoint = "GetProcAddress")]
        public static extern int GetProcAddress(
            int hModule,
            string lpProcName
        );

        [DllImport("kernel32")]

        public static extern IntPtr CreateRemoteThread(

          IntPtr hProcess,

          IntPtr lpThreadAttributes,

          uint dwStackSize,

          IntPtr lpStartAddress, // raw Pointer into remote process

          IntPtr lpParameter,

          uint dwCreationFlags,

          out uint lpThreadId

        );

        [DllImport("kernel32.dll", EntryPoint = "LoadLibrary")]
        public static extern int LoadLibrary(
            string lpLibFileName
        );

        [DllImport("kernel32.dll", EntryPoint = "FreeLibrary")]
        public static extern int FreeLibrary(
            int hLibModule
        );

        [DllImport("kernel32")]

        [return: MarshalAs(UnmanagedType.Bool)]

        public static extern bool GetExitCodeThread(IntPtr hThread, out uint lpExitCode);

        [DllImport("kernel32.dll", EntryPoint = "WaitForSingleObject")]
        public static extern int WaitForSingleObject(
            int hHandle,
            int dwMilliseconds
        );

        [DllImport("kernel32")]
        public static extern bool CloseHandle(IntPtr hObject);
    }
    [Flags]
    public enum AllocationType
    {
        Commit = 0x1000,
        Reserve = 0x2000,
        Decommit = 0x4000,
        Release = 0x8000,
        Reset = 0x80000,
        Physical = 0x400000,
        TopDown = 0x100000,
        WriteWatch = 0x200000,
        LargePages = 0x20000000
    }

    [Flags]
    public enum MemoryProtection
    {
        Execute = 0x10,
        ExecuteRead = 0x20,
        ExecuteReadWrite = 0x40,
        ExecuteWriteCopy = 0x80,
        NoAccess = 0x01,
        ReadOnly = 0x02,
        ReadWrite = 0x04,
        WriteCopy = 0x08,
        GuardModifierflag = 0x100,
        NoCacheModifierflag = 0x200,
        WriteCombineModifierflag = 0x400
    }

    [Flags]
    public enum FreeType
    {
        MEM_COALESCE_PLACEHOLDERS = 0x00000001,
        MEM_PRESERVE_PLACEHOLDER = 0x00000002,
        MEM_DECOMMIT = 0x4000,
        MEM_RELEASE = 0x8000
    }
}
