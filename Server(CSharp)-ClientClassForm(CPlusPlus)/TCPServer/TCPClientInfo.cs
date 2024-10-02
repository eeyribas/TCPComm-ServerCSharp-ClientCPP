using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace TCPServer
{
    public class TCPClientInfo
    {
        private Socket sock;

        private byte[] receiverBuffer, senderBuffer;
        private int receiverIndex, senderIndex;
        private int receiverLeft, senderLeft;
        private int clientNo;

        public TCPClientInfo()
        {
            receiverBuffer = new byte[1];
        }

        public Socket Sock
        {
            get
            {
                return sock;
            }
            set
            {
                sock = value;
            }
        }

        public byte[] ReceiverBuffer
        {
            get
            {
                return receiverBuffer;
            }
            set
            {
                receiverBuffer = value;
            }
        }

        public byte[] SenderBuffer
        {
            get
            {
                return senderBuffer;
            }
            set
            {
                senderBuffer = value;
            }
        }

        public int ReceiverIndex
        {
            get
            {
                return receiverIndex;
            }
            set
            {
                receiverIndex = value;
            }
        }

        public int SenderIndex
        {
            get
            {
                return senderIndex;
            }
            set
            {
                senderIndex = value;
            }
        }

        public int ReceiverLeft
        {
            get
            {
                return receiverLeft;
            }
            set
            {
                receiverLeft = value;
            }
        }

        public int SenderLeft
        {
            get
            {
                return senderLeft;
            }
            set
            {
                senderLeft = value;
            }
        }

        public int ClientNo
        {
            get
            {
                return clientNo;
            }
            set
            {
                clientNo = value;
            }
        }
    }
}
