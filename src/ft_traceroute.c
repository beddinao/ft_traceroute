#include <ft_traceroute.h>

unsigned short csum(unsigned short *buff, int words_n) {
	uint64_t sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (~(unsigned short)sum);
}

double/*ms*/_ping(_data* data, char *packet, char **packet_in, struct timeval *waittime) {
	struct	timeval	tstart = {0};
	struct	timeval	tend = {0};
	int		_ops_res;
	fd_set		r_set;

	FD_ZERO(&r_set);
	gettimeofday(&tstart, NULL);
	_ops_res = sendto(
			data->sock,
			packet,
			sizeof(struct iphdr) + sizeof(struct icmphdr) + 64,
			data->dest.addr,
			sizeof(struct addrinfo)
	);
	if (_ops_res < 0) {
		perror("ft_traceroute: sendto()");
		return _ops_res;
	}
	FD_SET(data->sock, &r_set);
	_ops_res = select(
			data->sock + 1,
			&r_set,
			NULL, NULL,
			waittime
	);
	if (_ops_res < 0)
		perror("ft_traceroute: select()");
	if (_ops_res <= 0)
		return _ops_res;
	gettimeofday(&tend, NULL);
}

void	ft_traceroute(_data *data, _op_vars *op_vars) {
	char		packet_out[def_packet_size];
	char		packet_in[def_packet_size];
	struct	iphdr	*iphdr_out, *iphdr_in;
	struct	icmphdr	*icmphdr_out, *icmphdr_in;

	size_t		icmphdr_len = sizeof(struct icmphdr);
	size_t		iphdr_len = sizeof(struct iphdr);

	for (;;) {
		memset(packet_out, 0, def_packet_size);
		iphdr_out = (struct iphdr*)packet_out;
		iphdr_out->version = 4;
		iphdr_out->id = op_vars->npid;
		iphdr_out->ttl = op_vars->ttl;
		iphdr_out->protocol = IPPROTO_ICMP;
		iphdr_out->saddr = data->src.s_addr;
		iphdr_out->daddr = data->dest.d_addr;

		icmphdr_out = (struct icmphdr*)(packet_out + iphdr_len);
		icmphdr_out->type = ICMP_ECHO;
		icmphdr_out->un.echo.id = op_vars->npid;
		memset(packet_out + icmphdr_len, 'a', 64);
		icmphdr_out->checksum = csum((unsigned short*)(packet_out + iphdr_len), (icmphdr_len+64)/2);

		iphdr_out->ihl = iphdr_len / 4;
		iphdr_out->tot_len = iphdr_len + icmphdr_len + 64;
		iphdr_out->check = csum((unsigned short*)packet, iphdr_len/2);

	}
}

void	ft_traceroute(struct timeval *timeout, _data *data) {
	/*char		packet_out[def_packet_size], packet_in[def_packet_size];
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
	*/
}
