#include <iostream>
#include "Essentials.h"

int main(int argc, char const *argv[])
{
    auto Protocol = ServerProtocols::SERVER_PROTCOL_TCP;
    NetWorkAddrFamily AddrFamily = NETWORK_ADDR_FAMILY_IPV_4;

    SERVER_PRINT("Using: " << Protocl2Str(Protocol))
    SERVER_PRINT("NetWorkAddrFamily: " << NetWorkFamily2Str(AddrFamily))

    // Initialize WinStock
    WSADATA WData;
    int Result = WSAStartup(MAKEWORD(2, 2), &WData);

    SERVER_PRINT("WinStock Initialization: " << Result)

    // Create Server Socket
    SOCKET Client = INVALID_SOCKET;
    Client = socket(GetNetWorkFamily(AddrFamily), SOCK_STREAM, GetWinProtocl(Protocol));

    if (Client == INVALID_SOCKET)
    {
        SERVER_PRINT("Error: " << WSAGetLastError())
        WSACleanup();
        SERVER_ERROR("Socket Creation Failed")
    }

    // Connect to the Server
    sockaddr_in _SockAddr;
    _SockAddr.sin_port = htons(SERVER_PORT); // Port Number
    _SockAddr.sin_family = GetNetWorkFamily(AddrFamily);
    _SockAddr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP_ADDR);

    if (connect(Client, reinterpret_cast<SOCKADDR *>(&_SockAddr), sizeof(_SockAddr)) == INVALID_SOCKET)
    {
        WSACleanup();
        closesocket(Client);
        ERROR("Connection with Server at: " << SERVER_IP_ADDR
                                                   << " " << SERVER_PORT << " Failed!")
    }
    PRINT("Connection with Server at: " << SERVER_IP_ADDR
                                               << " " << SERVER_PORT << " Established!")


    closesocket(Client);
    WSACleanup();

    return 0;
}
