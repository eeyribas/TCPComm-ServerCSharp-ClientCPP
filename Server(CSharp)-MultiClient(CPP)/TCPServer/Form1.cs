using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TCPServer
{
    public partial class Form1 : Form
    {
        public TCPClientInfo[] tcpClientInfo = new TCPClientInfo[2];
        private Socket serverSocket;
        private int port;

        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            port = 5120;
            for (int clientIndex = 0; clientIndex < tcpClientInfo.Length; clientIndex++)
                tcpClientInfo[clientIndex] = new TCPClientInfo();

            serverSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            serverSocket.Bind(new IPEndPoint(IPAddress.Any, port));
            serverSocket.Listen(10);

            serverSocket.BeginAccept(new AsyncCallback(AcceptProcess), null);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SendMsgToClient(tcpClientInfo[0], "1");
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SendMsgToClient(tcpClientInfo[1], "1");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SendMsgToClient(tcpClientInfo[0], "2");
        }

        private void button4_Click(object sender, EventArgs e)
        {
            SendMsgToClient(tcpClientInfo[1], "2");
        }

        private void AcceptProcess(IAsyncResult iar)
        {
            serverSocket.BeginAccept(new AsyncCallback(AcceptProcess), null);

            TcpClient tmpClient = new TcpClient();
            tmpClient.Client = serverSocket.EndAccept(iar);

            IPEndPoint ipep = (IPEndPoint)tmpClient.Client.RemoteEndPoint;
            IPAddress ipa = ipep.Address;
            string ipAddress = ipa.ToString();
            byte[] asBytes = ipa.GetAddressBytes();
            const byte ipAdressOfServer = 10;
            byte IPLastPart = asBytes[3];

            int clientNo = (IPLastPart - ipAdressOfServer);
            if (ipAddress.StartsWith("192.168.1."))
            {
                tcpClientInfo[clientNo] = new TCPClientInfo();
                tcpClientInfo[clientNo].ClientNo = clientNo;
                tcpClientInfo[clientNo].Sock = tmpClient.Client;
                tcpClientInfo[clientNo].ReceiverLeft = 2;
                tcpClientInfo[clientNo].ReceiverIndex = 0;

                tcpClientInfo[clientNo].Sock.BeginReceive(tcpClientInfo[clientNo].ReceiverBuffer, tcpClientInfo[clientNo].ReceiverIndex, tcpClientInfo[clientNo].ReceiverLeft, 
                                                          SocketFlags.None, new AsyncCallback(ReceiverProcess), tcpClientInfo[clientNo]);
                clientNo++;
            }
        }

        private void ReceiverProcess(IAsyncResult iar)
        {
            TCPClientInfo ci = (TCPClientInfo)iar.AsyncState;
            int n = 0;

            try
            {
                n = ci.Sock.EndReceive(iar);
                ci.ReceiverIndex += n;
                ci.ReceiverLeft -= n;

                if (ci.ReceiverLeft == 0)
                {
                    if (ci.ReceiverBuffer[0] == 10)
                        TextLabel(label1, ci.ReceiverBuffer[1].ToString());

                    if (ci.ReceiverBuffer[0] == 11)
                        TextLabel(label2, ci.ReceiverBuffer[1].ToString());

                    ci.ReceiverIndex = 0;
                    ci.ReceiverLeft = 2;
                }

                ci.Sock.BeginReceive(ci.ReceiverBuffer, ci.ReceiverIndex, ci.ReceiverLeft, 
                                     SocketFlags.None, new AsyncCallback(ReceiverProcess), ci);
            }
            catch (Exception)
            {
                ci.Sock.Dispose();
                lock (tcpClientInfo)
                    tcpClientInfo[ci.ClientNo] = null;
            }
        }

        private void SendMsgToClient(TCPClientInfo ci, string sendData)
        {
            try
            {
                ci.SenderLeft = 1;
                ci.SenderIndex = 0;
                ci.SenderBuffer = new byte[1];

                ci.SenderBuffer = Encoding.ASCII.GetBytes(sendData);
                ci.Sock.BeginSend(ci.SenderBuffer, ci.SenderIndex, ci.SenderLeft, 
                                  SocketFlags.None, new AsyncCallback(SendProcess), ci);
            }
            catch (Exception)
            {
                ci.Sock.Dispose();
                lock (tcpClientInfo)
                    tcpClientInfo[ci.ClientNo] = null;
            }
        }

        private void SendProcess(IAsyncResult iar)
        {
            TCPClientInfo ci = (TCPClientInfo)iar.AsyncState;
            int n = ci.Sock.EndSend(iar);
            ci.SenderIndex += n;
            ci.SenderLeft -= n;

            try
            {
                if (ci.SenderLeft != 0) {
                    ci.Sock.BeginSend(ci.SenderBuffer, ci.SenderIndex, ci.SenderLeft, 
                                      SocketFlags.None, new AsyncCallback(SendProcess), ci);
                }
            }
            catch (Exception)
            {
                ci.Sock.Dispose();
                lock (tcpClientInfo)
                    tcpClientInfo[ci.ClientNo] = null;
            }
        }

        delegate void TextLabelCallback(Label lb, string text);
        private void TextLabel(Label lb, string text)
        {
            if (lb.InvokeRequired)
            {
                TextLabelCallback d = new TextLabelCallback(_TextLabel);
                lb.Invoke(d, new object[] { lb, text });
            }
            else
            {
                _TextLabel(lb, text);
            }
        }

        private void _TextLabel(Label lb, string text)
        {
            lb.Text = text;
        }
    }
}
