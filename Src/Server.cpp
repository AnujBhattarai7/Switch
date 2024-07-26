#include <iostream>
#include "Switch.h"
#include <unordered_map>
#include <vector>

/*
Switch Architecture:
    - Server Sends a Message to the Client
    - Clients recieves and process it
    - Client sends a message back
*/

int main(int argc, char const *argv[])
{
    SERVER_PRINT("Weclome to Switch!")

    Switch::User Server{Switch::UserInfo{0, Switch::ACCOUNT_FLAGS_SERVER, Switch::ACCOUNT_TYPE_SERVER, Switch::__UUID__()}};
    // Initialize WinStock
    WSADATA WData;
    int Result = Switch::InitWSA(2, 2, WData);

    SERVER_PRINT("WinStock Initialization: " << Result)

    {
        Switch::SocketInfo SocketInfo;
        SocketInfo.AddrFamily = Switch::NETWORK_ADDR_FAMILY_IPV_4;
        SocketInfo.Protocol = Switch::ServerProtocols::SERVER_PROTCOL_TCP;
        SocketInfo.Type = SOCK_STREAM;

        SERVER_PRINT("Using: " << Switch::Protocl2Str(SocketInfo.Protocol))
        SERVER_PRINT("NetWorkAddrFamily: " << Switch::NetWorkFamily2Str(SocketInfo.AddrFamily))

        Server.InitSocket(SocketInfo);
    } // Create Server Socket

    // Set the SO_REUSEADDR optin to True to Reuse Addresses
    {
        bool SoReuseAddrOpt = true;

        if (Switch::EnableSockOpt(Server, SO_REUSEADDR) < 0)
        {
            Server.CleanSocket();
            WSACleanup();
            std::cout << "Error: " << WSAGetLastError();
            SERVER_ERROR("Set SoReuseAddrOpt True failed")
        }
    }
    SERVER_PRINT("Enabled SO_REUSEADDR: " << Switch::IsSockOptEnabled(Server, SO_REUSEADDR))

    SERVER_PRINT("Socket: " << Server)

    Switch::SockAddrInfo SockAddrInfo{SERVER_PORT, SERVER_IP_ADDR};
    auto SockAddr = SockAddrInfo.GetIpv4();
    int SockAddrLen = sizeof(SockAddr);

    if (Switch::BindSocket(Server, {SERVER_PORT, SERVER_IP_ADDR}) == SOCKET_ERROR)
    {
        Server.CleanSocket();
        WSACleanup();
        std::cout << "Error: " << WSAGetLastError();
        SERVER_ERROR("bind() failed");
    }

    SERVER_PRINT("Socket Bind on: " << SERVER_IP_ADDR << " " << SERVER_PORT)

    if (listen(Server, SOMAXCONN) < 0)
    {
        Server.CleanSocket();
        WSACleanup();
        std::cout << "Error: " << WSAGetLastError();
        SERVER_ERROR("Listen failed");
    }

    Switch::PacketSpecs SendSpec, RecvSpec;

    Switch::__UUID__ ServerID;

    std::unordered_map<Switch::__UUID__, SOCKET> Clients;
    Clients.reserve(10);

    WSAPOLLFD ServerFD{
        .fd = Server,
        .events = POLLRDNORM,
    };

    std::vector<WSAPOLLFD> fds;
    fds.push_back(ServerFD);

    SOCKET NewClient;

    bool Run = true;
    while (Run)
    {
        // fprintf(stderr, "Size of fds before WSAPoll %llu\n", fds.size());
        int result = WSAPoll(fds.data(), fds.size(), 1000); // no timeout, wait indefinitely

        if (fds[0].revents & POLLRDNORM)
        {
            if ((NewClient = accept(Server,
                                    (struct sockaddr *)&SockAddr, (socklen_t *)&SockAddrLen)) < 0)
            {
                Run = false;
                break;
            }
            WSAPOLLFD NewClientFD{
                .fd = NewClient,
                .events = POLLRDNORM,
            };
            Switch::__UUID__ ClientID;

            fds.push_back(NewClientFD);

            printf("Adding to list of sockets as %d\n", NewClient);

            SendSpec.SenderID = ServerID;
            SendSpec.RecieverID = ClientID;
            SendSpec.MainPacketFlag = Switch::PACKET_FLAG_CONNECTION_INIT;

            SendSpec.AdditionalPacketFlags = Switch::PACKET_FLAG_MESSAGE;
            char Message[] = "Welcome to Switch the first message between Server and Client!!";
            SendSpec.BufferSize = sizeof(Message);

            send(NewClient, (char *)&SendSpec, sizeof(Switch::PacketSpecs), 0);
            send(NewClient, (char *)Message, SendSpec.BufferSize, 0);

            SendSpec.MainPacketFlag = Switch::PACKET_FLAG_CONNECTION_CLOSE;

            send(NewClient, (char *)&SendSpec, sizeof(Switch::PacketSpecs), 0);
            send(NewClient, (char *)Message, SendSpec.BufferSize, 0);

            SERVER_PRINT("New connection,  socket fd is " << NewClientFD.fd << ", ip is : " << inet_ntoa(SockAddr.sin_addr)
                                                          << ", port : " << ntohs(SockAddr.sin_port))

            fprintf(stderr, "\tSize of fds after insert client %llu %llu\n\n", fds.size(), NewClientFD.fd);
        }

        // for (uint32_t i = 1; i < fds.size(); ++i) {
        for (auto iter = fds.begin() + 1; iter != fds.end();)
        {
            // for (size_t i = 1; i < fds.size(); ++i) {
            if (iter->revents & (POLLRDNORM | POLLERR | POLLHUP))
            {
                // Echo back the message that came in
                recv(iter->fd, (char *)&RecvSpec, sizeof(Switch::PacketSpecs), 0);

                if (RecvSpec.MainPacketFlag == Switch::PACKET_FLAG_CONNECTION_CLOSE)
                {
                    // Somebody disconnected , get his details and print
                    getpeername(iter->fd, (struct sockaddr *)&SockAddr,
                                (socklen_t *)&SockAddrLen);
                    printf("Host disconnected , ip %s , port %d \n",
                           inet_ntoa(SockAddr.sin_addr), ntohs(SockAddr.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    closesocket(iter->fd);
                    Clients.erase(RecvSpec.SenderID);
                    SERVER_PRINT("Client Removed: " << RecvSpec.SenderID)
                }
                else if (IsPacketFlagsPresent(RecvSpec, Switch::PACKET_FLAG_MESSAGE))
                {
                    if (RecvSpec.RecieverID == ServerID)
                    {
                        char Message[RecvSpec.BufferSize] = {0};
                        recv(iter->fd, (char *)&Message, RecvSpec.BufferSize, 0);

                        SERVER_PRINT("Message: " << Message << " from: " << RecvSpec.SenderID)
                    }
                }
            }
            iter++;
        }
    }

    Server.CleanSocket();
    WSACleanup();

    return 0;
}
