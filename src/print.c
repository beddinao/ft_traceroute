#include <ft_ping.h>

void	display_help() {
	printf("\nUsage\n  ft_ping [options] <destination>\n");
	printf("\nOptions:\n");
	printf("  <destination>             dns name or ip address\n");
	printf("  -c --count <count>        stop after <count> replies\n");
	printf("                              0xffffffff > <count> >= 0\n");
	printf("                              default: infinite\n");
	printf("  -i --interval <interval>  time between sending each packet in seconds\n");
	printf("                              0xffffffff > <interval> >= 0\n");
	printf("                              default: 1\n");
	printf("  -W --timeout <timeout>    time to wait for response in milliseconds\n");
	printf("                              0xffffffff > <timeout> >= 0\n");
	printf("                              default: 10000 -> 10s\n");
	printf("  -Q --tos <tos>            set Quality Of Serivce bits\n");
	printf("                              0xff <= <tos> >= 0\n");
	printf("                              default: 0\n");
	printf("  -t --ttl <ttl>            set Time To Live <ttl> bits\n");
	printf("                              0xff <= <ttl> >= 0\n");
	printf("                              default: 64\n");
	printf("  -f --flood                flood ping\n");
	printf("  -n --numeric              no dns name resolution\n");
	printf("  -v --verbose              verbose output\n");
	printf("  -V --version              prints version and exits\n");
	printf("  -? --help                 shows this help and exits\n\n");
}

void	print_outgoing_packet() {
	if (g_vars.input.flood)
		write(1, ".", 1);
}

void	print_incoming_packet(
		struct sockaddr_in *in_dest,
		struct icmphdr *icmphdr, struct icmphdr *icmphdr_out,
		uint16_t received,
		struct timeval *start, struct timeval *end,
		bool valid_csum)
{
	if (g_vars.input.flood) {
		write(1, "\b", 1);
		return;
	}

	if (icmphdr->type != ICMP_ECHOREPLY && !g_vars.input.verbose && !g_vars.input.is_set_ttl)
		return;

	bool	hostname_ready = 1;

	if (!g_vars.input.numeric_only) {
		if (!g_vars.resolved_hostname || g_vars.input.is_set_ttl) {
			memset(g_vars.hostname, 0, sizeof(g_vars.hostname));
			hostname_ready = getnameinfo((struct sockaddr*)in_dest, sizeof(struct sockaddr_in), g_vars.hostname, sizeof(g_vars.hostname), NULL, 0, 0);
			g_vars.resolved_hostname = True;
		}
		else 	hostname_ready = 0;
	}

	if (!g_vars.input.is_set_ttl)
		printf("%u bytes f", received);
	else	printf("F");

	printf("rom %s (%s) %s: icmp_seq=%i", 
			(g_vars.input.numeric_only || hostname_ready) ? "\b" : g_vars.hostname,
			g_vars.dest_ip, 
			valid_csum ? "\b" : "(invalid icmp csum!)",
			icmphdr_out->un.echo.sequence);


	if (!g_vars.input.is_set_ttl)
		printf(" ttl=%i time=%0.3f ms",
			g_vars.input.is_set_ttl ? g_vars.input.ttl : def_ttl,
			((double)(end->tv_sec - start->tv_sec) * 1000) + ((double)(end->tv_usec - start->tv_usec) / 1000));

	if (g_vars.input.is_set_ttl)
		printf(" type=icmp_%s", icmphdr->type < 18 ? icmp_types[icmphdr->type] : "");
	printf("\n");
}

