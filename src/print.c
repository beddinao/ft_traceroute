#include <ft_traceroute.h>

void	display_help() {
	printf("\nUsage\n  ft_traceroute [options] <destination>\n");
	printf("\nOptions:\n");
	printf("  <destination>               dns name or ip address\n");
	printf("  -i --interface <ifr>\n");
	printf("                              specifies the interface through which ft_traceroute should send packets\n");
	printf("                              by default the interface is selected according to the routing table\n");
	printf("                              this option is ignored if [-s/--source-addr] is present.\n");
	printf("  -s --source <source_addr>\n");
	printf("                              chooses an alternative source address.\n");
	printf("                              must be of one of the interfaces.\n");
	printf("                              by default the address of the chosen interface is used.\n");
	printf("  -m --max-hops <max_ttl>\n");
	printf("                              specifies the maximum number of hops (max time-to-live value).\n");
	printf("                              must be > 0 and less than %#x.\n", U08_MAX);
	printf("                              the default is %i.\n", def_max_ttl);
	printf("  -f --first <first_ttl>\n");
	printf("                              specifies with what TTL to start.\n");
	printf("                              must be >= 0 and less than %#x.\n", U08_MAX);
	printf("                              the default is %i.\n", def_first_ttl);
	printf("  -q --queries <nqueries>\n");
	printf("                              sets the number of probe packets per hop/ttl.\n");
	printf("                              must be >= 1. rounded to %#x if greater.\n", max_nqueries_value);
	printf("                              the default is %i.\n", def_nqueries);
	printf("  -w --wait <s_timeout>\n");
	printf("                              determines how many seconds to wait for a response to a probe\n");
	printf("                              must be less than %#x.\n", U32_MAX);
	printf("                              the default is %i\n", def_waittime);
	printf("  -t --tos <tos>\n");
	printf("                              set the Type Of Service (TOS).\n");
	printf("                              must be >= 0 and less than %#x\n", U08_MAX);
	printf("                              the default is %i\n\n", def_tos);
	printf("  -n --numeric                do not try to map IP addr to host names\n");
	printf("  -V --version                prints version and exits\n");
	printf("  -? --help                   shows this help and exits\n\n");
}

void	print_ttl(uint8_t ttl) {
	if (ttl < 10) printf(" ");
	printf("%i ", ttl);
}

void	print_current_time () {
	char buff[100] = {0};

	time_t now = time(NULL);
	strftime(buff, 100, "[%Y-%m-%d %H:%M:%S]", localtime(&now));
	printf("%s%s%s", WHT, buff, NRM);
}


void	print_header_intro(_data *data, _op_vars* op_vars, char* tar) {
	printf("ft_traceroute to");
	if (!data->input.numeric)
		printf(" %s", tar);
	printf(" (%s), %i hops max, %i byte packets\n", data->dest.ip, op_vars->max_ttl, payload_size);
}

void	dump_in_packet(_data *data, unsigned char *packet, struct sockaddr_in *addr) {
	if (!data->input.verbose)
		return;
	uint8_t	iphdr_len, hd;
	size_t	icmphdr_len = sizeof(struct icmphdr);
	size_t	in_iphdr_len = sizeof(struct iphdr);
	struct	iphdr* iphdr;

	printf("\n");
	print_current_time();
	printf("  ==== recved pack: ====\n");
	printf("ip_source: [%s]\n", inet_ntoa(addr->sin_addr));
	printf("out_ip_hdr   : ");
	iphdr_len = (packet[0] & 0xf) * 4;
	iphdr = (struct iphdr*)(packet + iphdr_len + icmphdr_len);
	for (hd = 0; hd < iphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\nout_icmp_hdr : ");
	for (hd = iphdr_len; hd < iphdr_len + icmphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\nin_ip_hdr    : ");
	for (hd = iphdr_len+icmphdr_len; hd < iphdr_len+icmphdr_len+in_iphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\nin_icmp_hdr  : ");
	for (hd = iphdr_len+icmphdr_len+in_iphdr_len; hd < iphdr_len+icmphdr_len*2+in_iphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\n");
}

void	dump_out_packet(_data *data, unsigned char *packet, struct sockaddr_in *addr) {
	if (!data->input.verbose)
		return;
	uint8_t	iphdr_len = (packet[0] & 0xf) * 4;
	size_t	icmphdr_len = sizeof(struct icmphdr);
	uint8_t	hd;

	printf("\n");
	print_current_time();
	printf("  ==== sent pack: ====\n");
	printf("ip_dest: [%s]\n", inet_ntoa(addr->sin_addr));
	printf("out_ip_hdr   : ");
	for (hd = 0; hd < iphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\nout_icmp_hdr : ");
	for (hd = iphdr_len; hd < iphdr_len + icmphdr_len; hd++) printf("%02x ", packet[hd]);
	printf("\n");
}

