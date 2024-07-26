#include <iostream>
#include "Switch.h"

#include <random>
#include <unordered_map>

static std::random_device s_RandomDevice;
static std::mt19937_64 s_Engine(s_RandomDevice());
static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

namespace Switch
{
    __UUID__::__UUID__()
        : _ID(s_UniformDistribution(s_Engine))
    {
    }

    int GetNetWorkFamily(NetWorkAddrFamily Family)
    {
        if (Family == NETWORK_ADDR_FAMILY_IPV_4)
            return AF_INET;
        if (Family == NETWORK_ADDR_FAMILY_IPV_6)
            return AF_INET6;
        return SERVER_ERORR_NO_RETURN;
    }

    const char *NetWorkFamily2Str(NetWorkAddrFamily Family)
    {
        if (Family == NETWORK_ADDR_FAMILY_IPV_4)
            return "IPv4";
        if (Family == NETWORK_ADDR_FAMILY_IPV_6)
            return "IPv6";
    }

    const char *Protocl2Str(ServerProtocols Protocol)
    {
        if (Protocol == ServerProtocols::SERVER_PROTCOL_TCP)
            return "SERVER_PROTCOL_TCP";
        else
            return "SERVER_PROTCOL_UDP";
        return nullptr;
    }

    int GetWinProtocl(ServerProtocols Protocol)
    {
        if (Protocol == ServerProtocols::SERVER_PROTCOL_TCP)
            return IPPROTO_TCP;
        if (Protocol == ServerProtocols::SERVER_PROTCOL_UDP)
            return IPPROTO_UDP;
        return SERVER_ERORR_NO_RETURN;
    }

    int InitWSA(int a, int b, WSAData &Data)
    {
        return WSAStartup(MAKEWORD(a, b), &Data);
    }

    SOCKET CreateSocket(const SocketInfo &Info)
    {
        if(Info.Protocol ==  ServerProtocols::SERVER_PROTCOL_UDP)
            SWITCH_ERROR("Only Supports SERVER_PROTCOL_TCP!!", WSACleanup());
        return socket(GetNetWorkFamily(Info.AddrFamily), Info.Type, GetWinProtocl(Info.Protocol));
    }

    int EnableSockOpt(SOCKET Socket, int OptName)
    {
        bool OptVal = true;
        return setsockopt(Socket, SOL_SOCKET, OptName, (char *)&OptVal, sizeof(OptVal));
    }

    int DisableSockOpt(SOCKET Socket, int OptName)
    {
        bool OptVal = false;
        return setsockopt(Socket, SOL_SOCKET, OptName, (char *)&OptVal, sizeof(OptVal));
    }

    bool IsSockOptEnabled(SOCKET Socket, int OptName)
    {
        bool OptVal = false;
        int SizeOptVal = sizeof(OptVal);
        getsockopt(Socket, SOL_SOCKET, OptName, (char *)&OptVal, &SizeOptVal);

        return OptVal;
    }

    int BindSocket(const User &Server, const SockAddrInfo &Info)
    {
        if (Server.GetInfo().AccType == ACCOUNT_TYPE_SERVER)
        {
            if (Server.GetSocketInfo().AddrFamily == NETWORK_ADDR_FAMILY_IPV_4)
            {
                auto SockAddr = Info.GetIpv4();
                return bind(Server, reinterpret_cast<SOCKADDR *>(&SockAddr), sizeof(SockAddr));
            }
            else
            {
                auto SockAddr = Info.GetIpv6();
                return bind(Server, reinterpret_cast<SOCKADDR *>(&SockAddr), sizeof(SockAddr));
            }
        }

        return SWITCH_RETURN_ERROR;
    }

    int ConnectSocket(const User &Client, const SockAddrInfo &Info)
    {
        if (Client.GetInfo().AccType == ACCOUNT_TYPE_CLIENT)
        {
            if (Client.GetSocketInfo().AddrFamily == NETWORK_ADDR_FAMILY_IPV_4)
            {
                auto SockAddr = Info.GetIpv4();
                return connect(Client, reinterpret_cast<SOCKADDR *>(&SockAddr), sizeof(SockAddr));
            }
            else
            {
                auto SockAddr = Info.GetIpv6();
                return connect(Client, reinterpret_cast<SOCKADDR *>(&SockAddr), sizeof(SockAddr));
            }
        }

        return SWITCH_RETURN_ERROR;
    }

    sockaddr_in6 SockAddrInfo::GetIpv6() const
    {
        sockaddr_in6 SockAddr;
        SockAddr.sin6_port = htons(Port);
        SockAddr.sin6_family = AF_INET6;
        // SockAddr.sin6_addr.u.

        return SockAddr;
    }

    sockaddr_in SockAddrInfo::GetIpv4() const
    {
        sockaddr_in SockAddr;
        SockAddr.sin_family = AF_INET;
        SockAddr.sin_addr.S_un.S_addr = inet_addr(IpAddr);
        SockAddr.sin_port = htons(Port);

        return SockAddr;
    }

    int User::InitSocket(const SocketInfo &Info)
    {
        this->_Info.Socket = CreateSocket(Info);
        _SocketInfo = Info;

        if (_Info.Socket == INVALID_SOCKET)
            return SWITCH_INVALID_USER;
        return true;
    }

    int User::CleanSocket()
    {
        return closesocket(_Info.Socket);
    }

} // namespace Switch
