#pragma once

// Has the Essential Classes, Structs, Definitions and Includes

#define _WIN32_WINNIT 0x601

#define SERVER_PORT 55555
#define SERVER_IP_ADDR "127.0.0.1"

#define SERVER_ERORR_NO_RETURN -1

// Server Includes(Using WinStock due to Windows)
#include <winsock2.h>
#include <ws2tcpip.h>

#include <assert.h>

#define PRINT(x)        \
    {                   \
        std::cout << x; \
    }

#define SERVER_PRINT(x)                  \
    {                                    \
        PRINT("[SERVER]: " << x << "\n") \
    }
#define ASSERT(x)  \
    {              \
        assert(x); \
    }

#define ERROR(x)                       \
    {                                  \
        PRINT(x << "\n")                       \
        ASSERT(false && "Assert Failed") \
    }

#define SERVER_ERROR(x)                    \
    {                                      \
        SERVER_PRINT(x);                   \
        ASSERT(false && "Assert Failed!!") \
    }

enum class ServerProtocols
{
    SERVER_PROTCOL_TCP,
    SERVER_PROTCOL_UDP
};

// Lists the Various Families Protocols supported by the network
enum NetWorkAddrFamily
{
    // Families supporting both IPv6 and IPv4
    NETWORK_ADDR_FAMILY_IPV_6,
    NETWORK_ADDR_FAMILY_AF_INET_6 = NETWORK_ADDR_FAMILY_IPV_6,

    // Families supporting only IPv4
    NETWORK_ADDR_FAMILY_IPV_4,
    NETWORK_ADDR_FAMILY_IPV = NETWORK_ADDR_FAMILY_IPV_4,
    NETWORK_ADDR_FAMILY_AF_INET = NETWORK_ADDR_FAMILY_IPV_4,
};

int GetNetWorkFamily(NetWorkAddrFamily Family);
const char *NetWorkFamily2Str(NetWorkAddrFamily Family);
const char *Protocl2Str(ServerProtocols Protocol);
int GetWinProtocl(ServerProtocols Protocol);
