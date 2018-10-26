/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    iptypes.h

--*/

#ifndef IP_TYPES_INCLUDED
#define IP_TYPES_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#pragma warning(push)
#pragma warning(disable:4201)

#include <time.h>

// Definitions and structures used by getnetworkparams and getadaptersinfo apis

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
#define DEFAULT_MINIMUM_ENTITIES        32  // arb.
#define MAX_HOSTNAME_LEN                128 // arb.
#define MAX_DOMAIN_NAME_LEN             128 // arb.
#define MAX_SCOPE_ID_LEN                256 // arb.

//
// types
//

// Node Type

#define BROADCAST_NODETYPE              1
#define PEER_TO_PEER_NODETYPE           2
#define MIXED_NODETYPE                  4
#define HYBRID_NODETYPE                 8

//
// IP_ADDRESS_STRING - store an IP address as a dotted decimal string
//

typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

//
// IP_ADDR_STRING - store an IP address with its corresponding subnet mask,
// both as dotted decimal strings
//

typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

//
// ADAPTER_INFO - per-adapter information. All IP addresses are stored as
// strings
//

typedef struct _IP_ADAPTER_INFO {
    struct _IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    PIP_ADDR_STRING CurrentIpAddress;
    IP_ADDR_STRING IpAddressList;
    IP_ADDR_STRING GatewayList;
    IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    IP_ADDR_STRING PrimaryWinsServer;
    IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

#ifdef _WINSOCK2API_

//
// The following types require Winsock2.
//

typedef enum {
    IpPrefixOriginOther = 0,
    IpPrefixOriginManual,
    IpPrefixOriginWellKnown,
    IpPrefixOriginDhcp,
    IpPrefixOriginRouterAdvertisement,
} IP_PREFIX_ORIGIN;

typedef enum {
    IpSuffixOriginOther = 0,
    IpSuffixOriginManual,
    IpSuffixOriginWellKnown,
    IpSuffixOriginDhcp,
    IpSuffixOriginLinkLayerAddress,
    IpSuffixOriginRandom,
} IP_SUFFIX_ORIGIN;

typedef enum {
    IpDadStateInvalid    = 0,
    IpDadStateTentative,
    IpDadStateDuplicate,
    IpDadStateDeprecated,
    IpDadStatePreferred,
} IP_DAD_STATE;

typedef struct _IP_ADAPTER_UNICAST_ADDRESS {
    union {
        ULONGLONG Alignment;
        struct { 
            ULONG Length;
            DWORD Flags;
        };
    };
    struct _IP_ADAPTER_UNICAST_ADDRESS *Next;
    SOCKET_ADDRESS Address;

    IP_PREFIX_ORIGIN PrefixOrigin;
    IP_SUFFIX_ORIGIN SuffixOrigin;
    IP_DAD_STATE DadState;

    ULONG ValidLifetime;
    ULONG PreferredLifetime;
    ULONG LeaseLifetime;
} IP_ADAPTER_UNICAST_ADDRESS, *PIP_ADAPTER_UNICAST_ADDRESS;

typedef struct _IP_ADAPTER_ANYCAST_ADDRESS {
    union {
        ULONGLONG Alignment;
        struct { 
            ULONG Length;
            DWORD Flags;
        };
    };
    struct _IP_ADAPTER_ANYCAST_ADDRESS *Next;
    SOCKET_ADDRESS Address;
} IP_ADAPTER_ANYCAST_ADDRESS, *PIP_ADAPTER_ANYCAST_ADDRESS;

typedef struct _IP_ADAPTER_MULTICAST_ADDRESS {
    union {
        ULONGLONG Alignment;
        struct {
            ULONG Length;
            DWORD Flags;
        };
    };
    struct _IP_ADAPTER_MULTICAST_ADDRESS *Next;
    SOCKET_ADDRESS Address;
} IP_ADAPTER_MULTICAST_ADDRESS, *PIP_ADAPTER_MULTICAST_ADDRESS;

//
// Per-address Flags
//
#define IP_ADAPTER_ADDRESS_DNS_ELIGIBLE 0x01
#define IP_ADAPTER_ADDRESS_TRANSIENT    0x02
#define IP_ADAPTER_ADDRESS_PRIMARY      0x04

typedef struct _IP_ADAPTER_DNS_SERVER_ADDRESS {
    union {
        ULONGLONG Alignment;
        struct {
            ULONG Length;
            DWORD Reserved;
        };
    };
    struct _IP_ADAPTER_DNS_SERVER_ADDRESS *Next;
    SOCKET_ADDRESS Address;
} IP_ADAPTER_DNS_SERVER_ADDRESS, *PIP_ADAPTER_DNS_SERVER_ADDRESS;

typedef struct _IP_ADAPTER_PREFIX {
    union {
        ULONGLONG Alignment;
        struct {
            ULONG Length;
            DWORD Flags;
        };
    };
    struct _IP_ADAPTER_PREFIX *Next;
    SOCKET_ADDRESS Address;
    ULONG PrefixLength;
} IP_ADAPTER_PREFIX, *PIP_ADAPTER_PREFIX;

//
// Per-adapter Flags
//
#define IP_ADAPTER_DDNS_ENABLED               0x01
#define IP_ADAPTER_REGISTER_ADAPTER_SUFFIX    0x02
#define IP_ADAPTER_DHCP_ENABLED               0x04
#define IP_ADAPTER_RECEIVE_ONLY               0x08
#define IP_ADAPTER_NO_MULTICAST               0x10
#define IP_ADAPTER_IPV6_OTHER_STATEFUL_CONFIG 0x20

//
// OperStatus values from RFC 2863
//
typedef enum {
    IfOperStatusUp = 1,
    IfOperStatusDown,
    IfOperStatusTesting,
    IfOperStatusUnknown,
    IfOperStatusDormant,
    IfOperStatusNotPresent,
    IfOperStatusLowerLayerDown
} IF_OPER_STATUS;

//
// Scope levels from RFC 2373 used with ZoneIndices array.
//
typedef enum {
    ScopeLevelInterface    = 1,
    ScopeLevelLink         = 2,
    ScopeLevelSubnet       = 3,
    ScopeLevelAdmin        = 4,
    ScopeLevelSite         = 5,
    ScopeLevelOrganization = 8,
    ScopeLevelGlobal       = 14
} SCOPE_LEVEL;

typedef struct _IP_ADAPTER_ADDRESSES {
    union {
        ULONGLONG Alignment;
        struct {
            ULONG Length;
            DWORD IfIndex;
        };
    };
    struct _IP_ADAPTER_ADDRESSES *Next;
    PCHAR AdapterName;
    PIP_ADAPTER_UNICAST_ADDRESS FirstUnicastAddress;
    PIP_ADAPTER_ANYCAST_ADDRESS FirstAnycastAddress;
    PIP_ADAPTER_MULTICAST_ADDRESS FirstMulticastAddress;
    PIP_ADAPTER_DNS_SERVER_ADDRESS FirstDnsServerAddress;
    PWCHAR DnsSuffix;
    PWCHAR Description;
    PWCHAR FriendlyName;
    BYTE PhysicalAddress[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD PhysicalAddressLength;
    DWORD Flags;
    DWORD Mtu;
    DWORD IfType;
    IF_OPER_STATUS OperStatus;
    DWORD Ipv6IfIndex;
    DWORD ZoneIndices[16];
    PIP_ADAPTER_PREFIX FirstPrefix;
} IP_ADAPTER_ADDRESSES, *PIP_ADAPTER_ADDRESSES;

//
// Flags used as argument to GetAdaptersAddresses().
// "SKIP" flags are added when the default is to include the information.
// "INCLUDE" flags are added when the default is to skip the information.
//
#define GAA_FLAG_SKIP_UNICAST       0x0001
#define GAA_FLAG_SKIP_ANYCAST       0x0002
#define GAA_FLAG_SKIP_MULTICAST     0x0004
#define GAA_FLAG_SKIP_DNS_SERVER    0x0008
#define GAA_FLAG_INCLUDE_PREFIX     0x0010
#define GAA_FLAG_SKIP_FRIENDLY_NAME 0x0020

#endif /* _WINSOCK2API_ */

//
// IP_PER_ADAPTER_INFO - per-adapter IP information such as DNS server list.
//

typedef struct _IP_PER_ADAPTER_INFO {
    UINT AutoconfigEnabled;
    UINT AutoconfigActive;
    PIP_ADDR_STRING CurrentDnsServer;
    IP_ADDR_STRING DnsServerList;
} IP_PER_ADAPTER_INFO, *PIP_PER_ADAPTER_INFO;

//
// FIXED_INFO - the set of IP-related information which does not depend on DHCP
//

typedef struct {
    char HostName[MAX_HOSTNAME_LEN + 4] ;
    char DomainName[MAX_DOMAIN_NAME_LEN + 4];
    PIP_ADDR_STRING CurrentDnsServer;
    IP_ADDR_STRING DnsServerList;
    UINT NodeType;
    char ScopeId[MAX_SCOPE_ID_LEN + 4];
    UINT EnableRouting;
    UINT EnableProxy;
    UINT EnableDns;
} FIXED_INFO, *PFIXED_INFO;

#ifndef IP_INTERFACE_NAME_INFO_DEFINED
#define IP_INTERFACE_NAME_INFO_DEFINED

typedef struct ip_interface_name_info {
    ULONG           Index;      // Interface Index
    ULONG           MediaType;  // Interface Types - see ipifcons.h
    UCHAR           ConnectionType;
    UCHAR           AccessType;
    GUID            DeviceGuid; // Device GUID is the guid of the device
                                // that IP exposes
    GUID            InterfaceGuid; // Interface GUID, if not GUID_NULL is the
                                // GUID for the interface mapped to the device.
} IP_INTERFACE_NAME_INFO, *PIP_INTERFACE_NAME_INFO;

#endif

#pragma warning(pop)

#ifdef __cplusplus
}
#endif

#endif

