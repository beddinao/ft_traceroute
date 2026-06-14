#include <ft_traceroute.h>

unsigned short csum(unsigned short *buff, int words_n) {
	uint64_t sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (~(unsigned short)sum);
}

double/*ms*/_ping(_data* data, char *packet, struct sockaddr_in *addr) {
	socklen_t		addr_s = sizeof(struct sockaddr_in);
	struct	timeval	tstart = {0}, tend = {0};
	struct	timeval	waittime = {0};
	int		_ops_res;
	fd_set		r_set;

	FD_ZERO(&r_set);
	waittime.tv_sec = data->input.is_set_waittime ? data->input.waittime : def_waittime;
	gettimeofday(&tstart, NULL);
	_ops_res = sendto(
			data->sock,
			packet,
			sizeof(struct iphdr) + sizeof(struct icmphdr) + 64,
			0,
			data->dest.addr->ai_addr,
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
			&waittime
	);
	if (_ops_res < 0)
		perror("ft_traceroute: select()");
	if (_ops_res <= 0)
		return _ops_res;
	gettimeofday(&tend, NULL);

	memset(packet, 0, sizeof(def_packet_size));
	_ops_res = recvfrom(
			data->sock,
			packet,
			def_packet_size,
			0,
			addr, &addr_s
	);
	if (_ops_res < 0) {
		perror("ft_traceroute: recvfrom()");
		return _ops_res;
	}

	return ((double)(tend.tv_sec - tstart.tv_sec) * 1000)
		+ ((double)(tend.tv_usec - tstart.tv_usec) / 1000);
}

void	print_ttl(uint8_t ttl) {
	if (ttl < 10) printf(" ");
	printf("%i ", ttl);
}

void	ft_traceroute(_data *data, _op_vars *op_vars) {
	char		packet[def_packet_size];
	struct	iphdr	*iphdr_out, *iphdr_in;
	struct	icmphdr	*icmphdr_out, *icmphdr_in;
	struct	sockaddr_in re_addr;


	char		hostname[max_hostname_len];
	char		last_seen_ip[max_addr_len];
	char		*res_ip;

	size_t		icmphdr_len = sizeof(struct icmphdr);
	size_t		iphdr_len = sizeof(struct iphdr);
	socklen_t		addr_len = sizeof(struct sockaddr_in);

	uint8_t		query;

	for (;;) {
		memset(packet, 0, def_packet_size);
		iphdr_out = (struct iphdr*)packet;
		iphdr_out->version = 4;
		iphdr_out->id = op_vars->npid;
		iphdr_out->tos = op_vars->tos;
		iphdr_out->ttl = op_vars->ttl;
		iphdr_out->protocol = IPPROTO_ICMP;
		iphdr_out->saddr = data->src.s_addr;
		iphdr_out->daddr = data->dest.d_addr;

		icmphdr_out = (struct icmphdr*)(packet + iphdr_len);
		icmphdr_out->type = ICMP_ECHO;
		icmphdr_out->un.echo.id = op_vars->npid;
		memset(packet + iphdr_len + icmphdr_len, 'a', 64);
		icmphdr_out->checksum = csum((unsigned short*)(packet + iphdr_len), (icmphdr_len+64)/2);

		iphdr_out->ihl = iphdr_len / 4;
		iphdr_out->tot_len = iphdr_len + icmphdr_len + 64;
		iphdr_out->check = csum((unsigned short*)packet, iphdr_len/2);


		memset(last_seen_ip, 0, max_addr_len);
		for (query = 0; query < op_vars->nqueries; query += 1) {
			memset(&re_addr, 0, sizeof(re_addr));

			double ttr = _ping(data, packet, &re_addr);
			if (ttr < 0) return;

			if (!query) 
				print_ttl(op_vars->ttl);

			res_ip = inet_ntoa(re_addr.sin_addr);
			if (strcmp(last_seen_ip, res_ip)) {

				if (!data->input.numeric
					&& !getnameinfo((struct sockaddr*)&re_addr, addr_len,
						hostname, max_hostname_len, NULL, 0, 0)) 
					printf(" %s", hostname);
				printf(" (%s)", res_ip);
			}

			memcpy(last_seen_ip, res_ip, strlen(res_ip));

			printf("  %0.3f ms %c", ttr, query+1 == op_vars->nqueries ? '\n' : '\b');
		}

		op_vars->ttl += 1;
		if (op_vars->ttl >= op_vars->max_ttl)
			break;

		icmphdr_in = (struct icmphdr*)(packet + iphdr_len);
		if (icmphdr_in->type == ICMP_ECHOREPLY)
			break;
	}
}
