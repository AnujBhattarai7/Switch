#include <iostream>
#include "Switch.h"

int main(int argc, char const *argv[])
{
    // Initialize WinStock
    WSADATA WData;
    int Result = Switch::InitWSA(2, 2, WData);

    SERVER_PRINT("WinStock Initialization: " << Result)

    Switch::User Client{Switch::UserInfo{0, Switch::ACCOUNT_FLAGS_CLIENT, Switch::ACCOUNT_TYPE_CLIENT, Switch::__UUID__()}};

    {
        Switch::SocketInfo SocketInfo;
        SocketInfo.AddrFamily = Switch::NETWORK_ADDR_FAMILY_IPV_4;
        SocketInfo.Protocol = Switch::ServerProtocols::SERVER_PROTCOL_TCP;
        SocketInfo.Type = SOCK_STREAM;

        SERVER_PRINT("Using: " << Switch::Protocl2Str(SocketInfo.Protocol))
        SERVER_PRINT("NetWorkAddrFamily: " << Switch::NetWorkFamily2Str(SocketInfo.AddrFamily))

        Client.InitSocket(SocketInfo);
    } // Create Server Socket

    if (Switch::ConnectSocket(Client, {SERVER_PORT, SERVER_IP_ADDR}) == SOCKET_ERROR)
        SWITCH_ERROR("Connection with Server at: " << SERVER_IP_ADDR
                                                   << " " << SERVER_PORT << " Failed!",
                     Client.CleanSocket();
                     WSACleanup();
                     )
    __PRINT__("Connection with Server at: " << SERVER_IP_ADDR
                                            << " " << SERVER_PORT << " Established!\n")

    Switch::__UUID__ ServerID, ClientID;

    Switch::PacketSpecs SendSpec, RecvSpec;

    // Initial welcome message by the server
    recv(Client, (char *)&RecvSpec, sizeof(Switch::PacketSpecs), 0);

    if (RecvSpec.MainPacketFlag == Switch::PACKET_FLAG_MESSAGE || RecvSpec.AdditionalPacketFlags && Switch::PACKET_FLAG_MESSAGE)
    {
        char Message[RecvSpec.BufferSize] = {0};
        recv(Client, (char *)Message, RecvSpec.BufferSize, 0);

        __PRINT__("Message: " << Message << "\n")
    }

    ServerID = RecvSpec.SenderID;
    ClientID = RecvSpec.RecieverID;

    SendSpec.SenderID = ClientID;
    SendSpec.RecieverID = ServerID;

    WSAPOLLFD ClientFD{
        .fd = Client,
        .events = POLLRDNORM};

    bool Run = true;
    while (Run)
    {
        int result = WSAPoll(&ClientFD, 1, 1000); // no timeout, wait indefinitely

        if (ClientFD.revents & ((POLLRDNORM | POLLERR | POLLHUP)))
        {
            recv(Client, (char *)&RecvSpec, sizeof(Switch::PacketSpecs), 0);

            char Message[RecvSpec.BufferSize] = {0};

            recv(Client, (char *)Message, RecvSpec.BufferSize, 0);

            if (RecvSpec.MainPacketFlag == Switch::PACKET_FLAG_MESSAGE)
                SERVER_PRINT(Message << " From Server: " << RecvSpec.SenderID)
        }
    }

    Client.CleanSocket();
    WSACleanup();

    return 0;
}
