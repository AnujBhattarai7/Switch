#include <iostream>
#include "Essentials.h"

/*
Switch Architecture:
    - Server Sends a Message to the Client
    - Clients recieves and process it
    - Client sends a message back
*/

int main(int argc, char const *argv[])
{
    SERVER_PRINT("Weclome to Switch!")

    auto Protocol = ServerProtocols::SERVER_PROTCOL_TCP;
    NetWorkAddrFamily AddrFamily = NETWORK_ADDR_FAMILY_IPV_4;

    SERVER_PRINT("Using: " << Protocl2Str(Protocol))
    SERVER_PRINT("NetWorkAddrFamily: " << NetWorkFamily2Str(AddrFamily))

    // Initialize WinStock
    WSADATA WData;
    int Result = WSAStartup(MAKEWORD(2, 2), &WData);

    SERVER_PRINT("WinStock Initialization: " << Result)

    // Create Server Socket
    SOCKET Socket = INVALID_SOCKET;
    Socket = socket(GetNetWorkFamily(AddrFamily), SOCK_STREAM, GetWinProtocl(Protocol));

    if (Socket == INVALID_SOCKET)
    {
        SERVER_ERROR("Socket Creation Failed")
    }

    // Set the SO_REUSEADDR optin to True to Reuse Addresses
    {
        bool SoReuseAddrOpt = true;

        if (setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, (char *)&SoReuseAddrOpt, sizeof(SoReuseAddrOpt)) < 0)
        {
            closesocket(Socket);
            WSACleanup();
            std::cout << "Error: " << WSAGetLastError();
            SERVER_ERROR("Set SoReuseAddrOpt True failed")
        }
    }

    SERVER_PRINT("Socket: " << Socket)

    // Binding the Socket to a Port and Addr
    sockaddr_in SockAddr;
    SockAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDR);
    SockAddr.sin_port = htons(SERVER_PORT);
    SockAddr.sin_family = GetNetWorkFamily(AddrFamily);

    int SockAddrLen = sizeof(SockAddr);

    if (bind(Socket, reinterpret_cast<SOCKADDR *>(&SockAddr), sizeof(SockAddr)) == SOCKET_ERROR)
    {
        closesocket(Socket);
        WSACleanup();
        std::cout << "Error: " << WSAGetLastError();
        SERVER_ERROR("bind() failed");
    }

    SERVER_PRINT("Socket Bind on: " << SERVER_IP_ADDR << " " << SERVER_PORT)

    if (listen(Socket, SOMAXCONN) < 0)
    {
        closesocket(Socket);
        WSACleanup();
        std::cout << "Error: " << WSAGetLastError();
        SERVER_ERROR("Listen failed");
    }

    closesocket(Socket);
    WSACleanup();

    return 0;
}
