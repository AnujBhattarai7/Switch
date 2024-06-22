#include "Essentials.h"

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
    if(Family == NETWORK_ADDR_FAMILY_IPV_4)
        return "IPv4";
    if(Family == NETWORK_ADDR_FAMILY_IPV_6)
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
