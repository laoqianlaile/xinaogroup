using System;
using System.Configuration;
using System.Text;
using System.Threading.Tasks;
using Com.Ejsino.Interface;
using DotNetty.Buffers;
using DotNetty.Common.Utilities;
using DotNetty.Transport.Channels;

namespace WeChatBot.BLL.Handlers
{
    public class RobotHandler : SimpleChannelInboundHandler<IByteBuffer>
    {
        private readonly IBLL _businessInterface;
        private readonly ILogWriter _log;

        public RobotHandler(IBLL businessInterface, ILogWriter logWriter)
        {
            _businessInterface = businessInterface;
            _log = logWriter;
        }

        public override void ChannelActive(IChannelHandlerContext context)
        {
            _log.Info("连接上客户端");
            IByteBuffer byteBuffer = Unpooled.Buffer(1024);
            var serialNumber = ConfigurationManager.AppSettings["SerialNumber"];
            _log.Info("序列号:" + serialNumber);
            var bytes = Encoding.UTF8.GetBytes("MAC:" + serialNumber);
            byteBuffer.WriteBytes(bytes);
            context.WriteAndFlushAsync(byteBuffer);
            base.ChannelActive(context);
        }

        public override void ChannelInactive(IChannelHandlerContext context)
        {
            _log.Info("与客户端断开连接");
            AttributeKey<string> attributeKey = AttributeKey<string>.ValueOf("MAC");
            _log.Info(context.Channel.GetAttribute(attributeKey) + "关闭");
            base.ChannelInactive(context);
        }

        public override void ExceptionCaught(IChannelHandlerContext context, Exception exception)
        {
            _log.Error(exception.ToString());
        }

        protected override async void ChannelRead0(IChannelHandlerContext ctx, IByteBuffer msg)
        {
            if (msg is IByteBuffer byteBuffer)
            {
                var requestMessage = byteBuffer.ToString(Encoding.UTF8);

                _log.Info(requestMessage);
                if (requestMessage.StartsWith("添加客户端失败"))
                {
                    await ctx.Channel.CloseAsync();
                }
                else if (requestMessage.Contains("{"))
                {
                    var callChainItem = Com.Ejsino.Utils.CallChain.CallChainHelper.GetCallChainItem(requestMessage);
                    var traceId = callChainItem.TraceId;
                    var spanId = callChainItem.SpanId;
                    var content = callChainItem.Content;
                    var response = await SendRequestToExecute(content);
                    //IByteBuffer returnByteBuffer = CreateResponse($"{traceId}:{spanId}:{response}");
                    //await ctx.WriteAndFlushAsync(returnByteBuffer);
                }
            }
            else
            {
                base.ChannelRead(ctx, msg);
            }
        }

        private static IByteBuffer CreateResponse(string response)
        {
            IByteBuffer returnbyteBuffer = Unpooled.Buffer(1024);
            var bytes = Encoding.UTF8.GetBytes(response);
            returnbyteBuffer.WriteBytes(bytes);
            return returnbyteBuffer;
        }

        public async Task<string> SendRequestToExecute(string request)
        {
            try
            {
                var response = await _businessInterface.ExecuteAsync(request);
                return response;
            }
            catch (Exception e)
            {
                _log.Error($"{nameof(SendRequestToExecute)}异常：{e}，请求串：{request}");
                //todo 返回需要报警的命令
                return null;
            }
        }
    }
}
