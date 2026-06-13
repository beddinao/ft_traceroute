#include <ft_traceroute.h>

bool	resolve_addr(char *host) {
	struct	addrinfo	hints;
	int		i;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE|AI_CANONNAME;
	i = getaddrinfo(host, NULL, &hints, &g_vars.dest);
	if (i != 0 || !g_vars.dest) {
		printf("ft_ping: %s: Name or service not known\n", host);
		return False;
	}

	g_vars.dest_ip = inet_ntoa(((struct sockaddr_in*)g_vars.dest->ai_addr)->sin_addr);

	printf("FT_PING %s (%s)\n", host, g_vars.dest_ip ? g_vars.dest_ip : "??");
	return True;
}

bool	check_root_privileges(uid_t r, uid_t e, uid_t s) {
	return getresuid(&r, &e, &s) < 0 || e;
}

void	signal_handler(int sig_num) {
	if (g_vars.sock > 0) close(g_vars.sock);
	if (g_vars.dest) freeaddrinfo(g_vars.dest);
	exit(sig_num);
}

int main(int c, char **v) {
	if (check_root_privileges(0, 0, 0)) {
		printf("ft_traceroute: error: need root privileges\n");
		return 1;
	}

	_data		data;
	memset(&data, 0, sizeof(_data));

	if (c < 2 || c > 0xff || (c >= 2 && !parse_params(c, v, &data))) {
		display_help();
		return 1;
	}

	signal(SIGINT, signal_handler);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	g_vars.sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (g_vars.sock < 0) {
		perror("ft_traceroute: socket()");
		return 1;
	}

	if (!resolve_addr(v[c-1]))
		return 1;

	ft_traceroute(&data);
	signal_handler(0);
}
