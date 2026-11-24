#include "../../include/netforge/net/ipv4_addr.h"
#include <arpa/inet.h>

int init_ipv4(ipv4_t *ip)
{
	if (ip == NULL)
	{
		LOG_WARN("cannot initialize NULL ipv4");
		return -1;
	}

	ip->ip = 0; 
	return 0;
}

int ipv4_from_string(ipv4_t *ip, const char *str){
    struct in_addr addr;

	if (ip == NULL || str == NULL)
		return -1;

	if (inet_pton(AF_INET, str, &addr) != 1)
	{
		LOG_WARN("invalid ipv4 string: %s", str);
		return -1;
	}

	ip->ip = ntohl(addr.s_addr);   // inet_pton отдаёт network order, храним host order
	return 0;
}

void ipv4_to_string(ipv4_t ip, char buf[16]){
    	snprintf(buf, 16, "%u.%u.%u.%u",
	         (ip.ip >> 24) & 0xFF,
	         (ip.ip >> 16) & 0xFF,
	         (ip.ip >> 8)  & 0xFF,
	          ip.ip        & 0xFF);
}