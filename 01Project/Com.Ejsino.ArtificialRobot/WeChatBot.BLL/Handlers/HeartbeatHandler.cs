using DotNetty.Buffers;
using DotNetty.Handlers.Timeout;
using DotNetty.Transport.Channels;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using Com.Ejsino.Utils;
using System.Threading;
using System.Linq;
using System.Net;
using Com.Ejsino.Interface;
using DotNetty.Common.Utilities;

namespace WeChatBot.BLL.Handlers
{
    public class HeartbeatHandler : ChannelHandlerAdapter
    {
        private static IByteBuffer HEARTBEAT_SEQUENCE;
        private static MEMORY_INFO MemInfo = new MEMORY_INFO();
        private static readonly AttributeKey<string> attributeKey = AttributeKey<string>.ValueOf("IsBusy");
        private ILogWriter _logWriter;

        public HeartbeatHandler(ILogWriter logWriter)
        {
            _logWriter = logWriter;
        }
        public override async void UserEventTriggered(IChannelHandlerContext context, object evt)
        {
            try
            {
                if (evt is IdleStateEvent)
                {
                    PerformanceCounter cpu = new PerformanceCounter("Processor", "% Processor Time", "_Total");
                    var percentage = cpu.NextValue();
                    Volatile.Read(ref percentage);
                    percentage = cpu.NextValue();
                    GlobalMemoryStatus(ref MemInfo);
                    var memory = MemInfo.dwMemoryLoad;
                    var msg = $"{percentage},{memory},{{{string.Join(",", string.Empty)}}}";
                    _logWriter?.Info($"{DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss fff")}:{msg}");
                    HEARTBEAT_SEQUENCE = Unpooled.UnreleasableBuffer(Unpooled.CopiedBuffer(Encoding.UTF8.GetBytes("HEARTBEAT:" + msg)));
                    try
                    {
                        await context.WriteAndFlushAsync(HEARTBEAT_SEQUENCE.Duplicate());
                    }
                    catch (Exception e)
                    {
                        _logWriter?.Fatal($"{nameof(HeartbeatHandler)}.{nameof(UserEventTriggered)}:{e}");
                    }
                }
                else
                {
                    base.UserEventTriggered(context, evt);
                }
            }
            catch (Exception e)
            {
                _logWriter?.Fatal($"{nameof(HeartbeatHandler)}.{nameof(UserEventTriggered)}:{e}");
            }
        }

        [DllImport("kernel32")]
        public static extern void GetSystemDirectory(StringBuilder SysDir, int count);
        [DllImport("kernel32")]
        public static extern void GetSystemInfo(ref CPU_INFO cpuinfo);
        [DllImport("kernel32")]
        public static extern void GlobalMemoryStatus(ref MEMORY_INFO meminfo);
        [DllImport("kernel32")]
        public static extern void GetSystemTime(ref SYSTEMTIME_INFO stinfo);
    }


    //定义CPU的信息结构  
    [StructLayout(LayoutKind.Sequential)]
    public struct CPU_INFO
    {
        public uint dwOemId;
        public uint dwPageSize;
        public uint lpMinimumApplicationAddress;
        public uint lpMaximumApplicationAddress;
        public uint dwActiveProcessorMask;
        public uint dwNumberOfProcessors;
        public uint dwProcessorType;
        public uint dwAllocationGranularity;
        public uint dwProcessorLevel;
        public uint dwProcessorRevision;
    }
    //定义内存的信息结构  
    [StructLayout(LayoutKind.Sequential)]
    public struct MEMORY_INFO
    {
        public uint dwLength;
        public uint dwMemoryLoad;
        public uint dwTotalPhys;
        public uint dwAvailPhys;
        public uint dwTotalPageFile;
        public uint dwAvailPageFile;
        public uint dwTotalVirtual;
        public uint dwAvailVirtual;
    }
    //定义系统时间的信息结构  
    [StructLayout(LayoutKind.Sequential)]
    public struct SYSTEMTIME_INFO
    {
        public ushort wYear;
        public ushort wMonth;
        public ushort wDayOfWeek;
        public ushort wDay;
        public ushort wHour;
        public ushort wMinute;
        public ushort wSecond;
        public ushort wMilliseconds;
    }
}
