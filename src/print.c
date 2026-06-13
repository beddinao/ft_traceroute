#include <ft_traceroute.h>

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
	
}

void	print_incoming_packet()
{
	printf("\n");
}

