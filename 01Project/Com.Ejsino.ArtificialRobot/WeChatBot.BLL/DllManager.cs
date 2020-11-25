using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WeChatBot.BLL
{
    public static class DllManager
    {
        private static readonly List<DllDescription> dllDescriptions = new List<DllDescription>();

        internal static void Add(DllDescription des)
        {
            dllDescriptions.Add(des);
        }

        public static DllDescription Get(string name)
        {
            return dllDescriptions.SingleOrDefault(o => o.Name == name);
        }

        public static List<DllDescription> GetAll()
        {
            return dllDescriptions;
        }
    }

    public class DllDescription
    {
        public DllDescription(string name, string path, uint baseAddr)
        {
            if (string.IsNullOrEmpty(name))
            {
                throw new ArgumentException("名称不能为空", nameof(name));
            }

            if (string.IsNullOrEmpty(path))
            {
                throw new ArgumentException("dll路径不能为空", nameof(path));
            }

            if (baseAddr == 0)
            {
                throw new ArgumentException($"dll({name})加载基址不能为0", nameof(baseAddr));
            }

            Name = name;
            Path = path;
            BaseAddr = baseAddr;
        }

        public string Name { get; }

        public string Path { get; }

        public uint BaseAddr { get; }
    }
}
