
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <sys/socket.h> 
#include <net/if.h> 
#include <netinet/in.h> 
#include <net/if_arp.h> 
#include <arpa/inet.h>

#include "linux_util.h"

#define MAXINTERFACES   16 
void GetRightPath(char* szPath)
{
	size_t len = strlen(szPath);
	char * charTemp = szPath;
	for(int i = 0; i < len; ++i)
	{
		if (*charTemp == '\\')
			*charTemp = '/';
		++charTemp;
	}
}

int GetIPMacAddr(unsigned char macAddr[], char ip[])
{ 
	int fd, intrface, retn = 0; 
	struct ifreq buf[MAXINTERFACES]; 
	struct arpreq arp; 
	struct ifconf ifc; 

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) { 
		ifc.ifc_len = sizeof buf; 
		ifc.ifc_buf = (caddr_t) buf; 
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc)) { 
			intrface = ifc.ifc_len / sizeof (struct ifreq); 
			//printf("interface num is intrface=%d\n\n\n",intrface); 
			while (intrface-- > 0) 
			{ 
				//printf ("net device %s\n", buf[intrface].ifr_name); 

				if(strcmp(buf[intrface].ifr_name, "eth0") != 0) continue;

				/*Jugde whether the net card status is promisc  */ 
				if (!(ioctl (fd, SIOCGIFFLAGS, (char *) &buf[intrface]))) { 
					if (buf[intrface].ifr_flags & IFF_PROMISC) { 
						//puts ("the interface is PROMISC"); 
						retn++; 
					} 
				} else { 
					//char str[256]; 

					//rt2_sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					//perror (str); 
				} 

				/*Jugde whether the net card status is up       */ 
				if (buf[intrface].ifr_flags & IFF_UP) { 
					//puts("the interface status is UP"); 
				} 
				else { 
					//puts("the interface status is DOWN"); 
				} 

				/*Get IP of the net card */ 
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface]))) 
				{ 
					char* ipaddr = inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
					memcpy((void*)ip, (void*)ipaddr, strlen(ipaddr));
					//puts ("IP address is:");					
					//puts(ipaddr); 
					//puts(""); 
					//-----------------puts (buf[intrface].ifr_addr.sa_data); 
				} 
				else { 
					//char str[256]; 

					//rt2_sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					//perror (str); 
				} 

				/*Get HW ADDRESS of the net card */ 
				if (!(ioctl (fd, SIOCGIFHWADDR, (char *) &buf[intrface]))) 
				{ 
					//puts ("HW address is:"); 

					//printf("%02x:%02x:%02x:%02x:%02x:%02x\n", 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[0], 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[1], 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[2], 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[3], 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[4], 
					//	(unsigned char)buf[intrface].ifr_hwaddr.sa_data[5]); 

					puts(""); 
					puts(""); 

					macAddr[0] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[0];
					macAddr[1] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[1];
					macAddr[2] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[2];
					macAddr[3] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[3];
					macAddr[4] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[4];
					macAddr[5] = (unsigned char)buf[intrface].ifr_hwaddr.sa_data[5];
				} 

				else { 
					//char str[256]; 

					//rt2_sprintf (str, "cpm: ioctl device %s", buf[intrface].ifr_name); 
					//perror (str); 
				} 
			} 
		} else 
			perror ("cpm: ioctl"); 

	} else 
		perror ("cpm: socket"); 

	close (fd); 
	return retn;
}
#endif //LINUX
