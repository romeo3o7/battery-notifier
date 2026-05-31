#include <sys/socket.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>

int main() {
	struct sockaddr_nl a;
	memset(&a,0,sizeof a);

	a.nl_family = AF_NETLINK;
	a.nl_groups = 1;

	int socketfd = socket(AF_NETLINK, SOCK_RAW ,NETLINK_KOBJECT_UEVENT);

	if (socketfd == -1) return -1;

	if (bind(socketfd , (struct sockaddr*) &a ,sizeof a) < 0 ) return -1;

	unsigned int percentage = 0;
	char buffer[5000];
	while (1) {
		percentage = 0;
		ssize_t messageStatus = recv(socketfd, buffer, sizeof buffer, 0);
		if (messageStatus == -1 ) return -1;

		if (messageStatus < sizeof buffer) buffer[messageStatus] = '\0';
		else return 1;

		char *result = strstr(buffer , "POWER_SUPPLY_CAPACITY=");

		if (result == NULL) continue;

		result += strlen("POWER_SUPPLY_CAPACITY=");
		while (*result >= '0' && *result <= '9') {
			percentage = percentage * 10 + (*result - '0');
			result += 1;
		}
		printf(" message = %s\n" , buffer);
	}
	close(socketfd);
	return 0;
}
