#include <ft_ping.h>

unsigned short csum(unsigned short *buff, int words_n) {
	uint64_t sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (~(unsigned short)sum);
}


void	ft_ping(struct timeval *timeout, struct timeval *interval) {
	char		packet_out[def_packet_size], packet_in[def_packet_size];
	struct	icmphdr	*icmphdr_out;
	struct	icmphdr	icmphdr_in;
	struct	timeval	timeval_st;
	struct	timeval	timeval_end;
	struct	sockaddr_in in_dest_addr;
	socklen_t		addr_len = sizeof(struct sockaddr);
	socklen_t		in_addr_len = sizeof(in_dest_addr);
	uint16_t		icmphdr_len = sizeof(struct icmphdr);
	uint16_t		iphdr_len = sizeof(struct iphdr);
	uint16_t		sequence = 0, _ops_res;
	uint16_t		ops_id = htons(getpid());
	fd_set		r_set;

	g_vars.sent_packets = 0;
	icmphdr_out = (struct icmphdr*)packet_out;
	memset(packet_out, 0, sizeof(packet_out));
	icmphdr_out->type = ICMP_ECHO;
	icmphdr_out->un.echo.id = ops_id;
	memset(packet_out + icmphdr_len, 'a', 64);
	for (;;) {
		/////// // // SENDING
		icmphdr_out->un.echo.sequence = sequence++;
		memset(&icmphdr_out->checksum, 0, sizeof(icmphdr_out->checksum));
		icmphdr_out->checksum = csum((unsigned short *)packet_out, (icmphdr_len+64)/2);

		gettimeofday(&timeval_st, NULL);
		_ops_res = sendto(g_vars.sock, packet_out, icmphdr_len + 64, 0, g_vars.dest->ai_addr, addr_len);
		if (_ops_res < 0) {
			perror("ft_ping: sendto()");
			break;
		}
		print_outgoing_packet();
		g_vars.sent_packets += 1;
		if (g_vars.sent_packets >= U64_MAX)
			g_vars.sent_packets = 0;

		//// // / /// WAITING FOR RESPONSE
		FD_ZERO(&r_set);
		FD_SET(g_vars.sock, &r_set);
		_ops_res = select(g_vars.sock + 1, &r_set, NULL, NULL, timeout);
		if (_ops_res < 0) {
			perror("ft_ping: select()");
			break;
		}
		else if (!_ops_res) {
			if (!g_vars.input.flood) {
				printf("## timeout [%lis] retrying in 1s..\n", timeout->tv_sec);
				sleep(1);
			}
			continue;
		}
		gettimeofday(&timeval_end, NULL);
		g_vars.recv_packets += 1;
		if (g_vars.recv_packets >= U64_MAX)
			g_vars.recv_packets = 0;

		//// / // /// READING RESPONSE
		memset(packet_in, 0, sizeof(packet_in));
		_ops_res = recvfrom(g_vars.sock, packet_in, def_packet_size, 0, &in_dest_addr, &in_addr_len);
		if (_ops_res < 0) {
			perror("ft_ping: recvfrom()");
			break;
		}
		else if (!_ops_res) continue;

		memset(&icmphdr_in, 0, sizeof(icmphdr_in));
		memcpy(&icmphdr_in, packet_in + iphdr_len, icmphdr_len);

		print_incoming_packet(&in_dest_addr, &icmphdr_in, icmphdr_out, _ops_res - icmphdr_len - iphdr_len, &timeval_st, &timeval_end,
		/// // /// / VERIFYING INTEGRITY
			csum((unsigned short*)(packet_in + iphdr_len), (_ops_res - iphdr_len)/2) == 0x0);

		if (g_vars.input.is_set_count && g_vars.sent_packets >= g_vars.input.count)
			break;
		usleep((interval->tv_sec * 1000000) + interval->tv_usec);
	}
}
