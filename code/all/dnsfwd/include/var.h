#define BLOCK 1024

#define LISTEN_PORT 53
//#define NS1 "ns1.n1ck.name"
//#define NS1_IP "213.248.54.145"
//#define NS2 "ns2.n1ck.name"
//#define NS2_IP "213.248.62.7"
//#define FWD "213.248.62.7"
#define MX "mail.ainmarh.com"
struct in_addr ip_fwd,ip_ns1,ip_ns2;
#define AD "v=spf1 a mx ~all"
#define SRV "\x04""xmpp""\x07""ainmarh""\x03""com""\x00"

#include "include.h"

#if defined  DEFINE
#include "define.h"
#endif


#if defined  NET
#include "net.h"
#endif


//EOF

