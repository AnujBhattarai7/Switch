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

#define __PRINT__(x)    \
    {                   \
        std::cout << x; \
    }

#define SWITCH_PRINT(x)                       \
    {                                         \
        __PRINT__("[SWITCH]: " << x << "\n"); \
    }

#define SERVER_PRINT(x) \
    {                   \
        __PRINT__("[SERVER]: " << x << "\n")}
#define ASSERT(x)  \
    {              \
        assert(x); \
    }

#define ERROR(x)             \
    {                        \
        __PRINT__(x << "\n") \
            ASSERT(false && "Assert Failed")}

#define SERVER_ERROR(x)                    \
    {                                      \
        SERVER_PRINT(x);                   \
        ASSERT(false && "Assert Failed!!") \
    }

#define SWITCH_ERROR(x, ExitFunc) \
    {                             \
        ExitFunc;                 \
        ERROR(x);                 \
    }

#include <stdint.h>
#define BIT(x) (1 << x)

#define SWITCH_RETURN_ERROR -1
#define SWITCH_INVALID_USER 99

namespace Switch
{
    enum class ServerProtocols
    {
        SERVER_PROTCOL_TCP,
        SERVER_PROTCOL_UDP
    };

    enum AccountTypes
    {
        ACCOUNT_TYPE_SERVER,
        ACCOUNT_TYPE_CLIENT,
    };

    // Flags and Features allowed for each Account
    enum AccountFlags
    {
        // The Flags allowing for the account to control the server
        ACCOUNT_FLAGS_SERVER,
        // The Flags allowing for the account to control the client
        ACCOUNT_FLAGS_CLIENT,
        // Flag if set the account wouldnot be able to send any message
        ACCOUNT_FLAGS_NO_MESSAGE,
    };

    enum PacketFlags
    {
        PACKET_FLAG_MESSAGE = BIT(0),
        PACKET_FLAG_CONNECTION_INIT = BIT(1),
        PACKET_FLAG_CONNECTION_CLOSE = BIT(2),
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

    //
    struct __UUID__
    {
    public:
        __UUID__(uint64_t ID) : _ID(ID) {}
        __UUID__();
        ~__UUID__() {}

        operator uint64_t() const { return _ID; }

    private:
        uint64_t _ID;
    };

    struct PacketSpecs
    {
        __UUID__ RecieverID;
        __UUID__ SenderID;

        AccountFlags SenderAccountFlag;
        // The Main packet flag for the Packet
        PacketFlags MainPacketFlag;
        // Additional flags to process for the server
        int AdditionalPacketFlags = 0;

        /*
        The Variable which stores the size in bytes of the buffer that is to be sent:
            -if there is no MESSAGE Flag then it will function as a additional date storage
            -else just stores the size of the buffer to be sent after it
        */
        int BufferSize = 0;
    };

    static bool IsPacketFlagsPresent(const PacketSpecs &Spec, PacketFlags Flags)
    {
        return Spec.MainPacketFlag == Flags || Spec.AdditionalPacketFlags && Flags;
    }

    struct SocketInfo
    {
        NetWorkAddrFamily AddrFamily;
        int Type;
        ServerProtocols Protocol;
    };

    struct SockAddrInfo
    {
        int Port;
        const char *IpAddr;

        sockaddr_in6 GetIpv6() const;
        sockaddr_in GetIpv4() const;
    };

    struct UserInfo
    {
        SOCKET Socket;
        int AccFlags;
        AccountTypes AccType;
        __UUID__ ID;
    };

    struct User
    {
    public:
        User(const UserInfo &Info = UserInfo()) : _Info(Info) {}
        ~User() {}

        const UserInfo &GetInfo() const { return _Info; }
        const SocketInfo &GetSocketInfo() const { return _SocketInfo; }
        const __UUID__ &GetID() const { return _Info.ID; }

        operator SOCKET() const { return _Info.Socket; }

        int InitSocket(const SocketInfo &Info);
        int CleanSocket();

    private:
        UserInfo _Info;
        SocketInfo _SocketInfo;
    };

    int InitWSA(int a, int b, WSAData &Data);

    SOCKET CreateSocket(const SocketInfo &Info);

    int EnableSockOpt(SOCKET Socket, int OptName);
    int DisableSockOpt(SOCKET Socket, int OptName);
    bool IsSockOptEnabled(SOCKET Socket, int OptName);

    // Server Socket Related
    int BindSocket(const User &Server, const SockAddrInfo &Info);

    // Client Socket Related
    int ConnectSocket(const User &Client, const SockAddrInfo &Info);
}

namespace std
{
    template <typename T>
    struct hash;

    template <>
    struct hash<Switch::__UUID__>
    {
        size_t operator()(const Switch::__UUID__ &uuid) const
        {
            return (uint64_t)uuid;
        }
    };
}
