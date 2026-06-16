/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_traceroute.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: beddinao <beddinao@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 01:16:14 by beddinao          #+#    #+#             */
/*   Updated: 2026/06/14 19:18:01 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_traceroute.h>

unsigned short csum(unsigned short *buff, int words_n) {
	uint64_t sum = 0;
	while (words_n--)
		sum += *buff++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (~(unsigned short)sum);
}

void	dump_in_packet(unsigned char *packet, struct sockaddr_in *addr) {
	printf("\n==== recved pack: ====\n");
	printf("ip_source: [%s]\n", inet_ntoa(addr->sin_addr));
	printf("out_ip_hdr   : ");
	uint8_t iphdr_len = (packet[0] & 0xf) * 4;
	struct iphdr* iphdr = (struct iphdr*)(packet + iphdr_len + sizeof(struct icmphdr));
	for (uint8_t hd = 0; hd < iphdr_len; hd++) {
		printf("%02x ", packet[hd]);
	}
	printf("\nout_icmp_hdr : ");
	for (uint8_t hd = iphdr_len; hd < iphdr_len + sizeof(struct icmphdr); hd++) {
		printf("%02x ", packet[hd]);
	}

	printf("\nin_ip_hdr    : ");
	uint8_t in_iphdr_len = sizeof(struct iphdr);
	for (uint8_t hd = iphdr_len+sizeof(struct icmphdr); hd < iphdr_len+sizeof(struct icmphdr)+in_iphdr_len; hd++) {
		printf("%02x ", packet[hd]);
	}

	printf("\nin_icmp_hdr  : ");
	for (uint8_t hd = iphdr_len+sizeof(struct icmphdr)+in_iphdr_len; hd < iphdr_len + sizeof(struct icmphdr)*2+in_iphdr_len; hd++) {
		printf("%02x ", packet[hd]);
	}

	printf("\n");
}

void	dump_out_packet(unsigned char *packet, struct sockaddr_in *addr) {
	printf("\n==== sent pack: ====\n");
	printf("ip_dest: [%s]\n", inet_ntoa(addr->sin_addr));
	printf("out_ip_hdr   : ");
	uint8_t iphdr_len = (packet[0] & 0xf) * 4;
	for (uint8_t hd = 0; hd < iphdr_len; hd++) {
		printf("%02x ", packet[hd]);
	}
	printf("\nout_icmp_hdr : ");
	for (uint8_t hd = iphdr_len; hd < iphdr_len + sizeof(struct icmphdr); hd++) {
		printf("%02x ", packet[hd]);
	}
	printf("\n");
}

double/*ms*/_ping(_data* data, char *packet_out, char *packet_in, struct sockaddr_in *addr, double waittime, bool expecting) {
	socklen_t		addr_s = sizeof(struct sockaddr_in);
	size_t		icmphdr_len = sizeof(struct icmphdr);
	size_t		recv_iphdr_len, sent_iphdr_len, recv_tot_len;
	struct	timeval	tstart = {0}, tend = {0}, timeout = {0};
	struct	icmphdr*	icmphdr;
	struct	iphdr*	iphdr;
	double		rtt;
	int		_ops_res;
	fd_set		r_set;

	FD_ZERO(&r_set);
	ft_memset(addr, 0, sizeof(struct sockaddr_in));
	timeout.tv_sec = waittime / 1000;
	timeout.tv_usec = ((size_t)waittime % 1000) * 1000;

	if (expecting) {
		//dump_out_packet((unsigned char*)packet_out, (struct sockaddr_in*)data->dest.addr->ai_addr);
		gettimeofday(&tstart, NULL);
		_ops_res = sendto(data->sock, packet_out,
				sizeof(struct iphdr) + sizeof(struct icmphdr) + payload_size, 0,
				data->dest.addr->ai_addr, sizeof(struct addrinfo)
		);
		if (_ops_res < 0) {
			perror("ft_traceroute: sendto()");
			return _ops_res;
		}
	}
	else	gettimeofday(&tstart, NULL);

	FD_SET(data->sock, &r_set);
	_ops_res = select(data->sock + 1, &r_set, NULL, NULL, &timeout);
	if (_ops_res < 0)
		perror("ft_traceroute: select()");
	if (_ops_res <= 0)
		return _ops_res;
	gettimeofday(&tend, NULL);

	ft_memset(packet_in, 0, sizeof(def_packet_size));
	_ops_res = recvfrom(data->sock, packet_in, def_packet_size, 0, addr, &addr_s);
	if (_ops_res < 0) {
		perror("ft_traceroute: recvfrom()");
		return _ops_res;
	}

	//dump_in_packet((unsigned char*)packet_in, addr);

	recv_iphdr_len = (packet_in[0] & 0xf) * 4;
	sent_iphdr_len = (packet_out[0] & 0xf) * 4;
	recv_tot_len = (packet_in[2] << 0x8 | packet_in[3]) & U16_MAX;
	icmphdr = (struct icmphdr*)(packet_in + recv_iphdr_len);
	rtt = ((double)(tend.tv_sec - tstart.tv_sec) * 1000) + ((double)(tend.tv_usec - tstart.tv_usec) / 1000);
	
	if (icmphdr->type != ICMP_ECHOREPLY)
		icmphdr = (struct icmphdr*)(packet_in + recv_iphdr_len + icmphdr_len + sent_iphdr_len);

	//printf("[%#x]==[%#x]-> ", ((packet_out[sent_iphdr_len + 5] << 8 | packet_out[sent_iphdr_len + 4]) & U16_MAX), icmphdr->un.echo.id);

	if (icmphdr->un.echo.id == ((packet_out[sent_iphdr_len + 5] << 8 | packet_out[sent_iphdr_len + 4]) & U16_MAX)) {
		icmphdr = (struct icmphdr*)(packet_in + recv_iphdr_len);
		//printf("[MINE]\n");
		if ( csum((unsigned short*)packet_in, recv_iphdr_len /2) == 0x0
			/*&& csum((unsigned short*)(packet_in + recv_iphdr_len), ((recv_tot_len-recv_iphdr_len) /2)) == 0x0*/
			&& (icmphdr->type == ICMP_ECHOREPLY || icmphdr->type == ICMP_TIME_EXCEEDED)) {
			return rtt;	
		}
		else
			return false;
	}
	//else	printf("[NOT_MINE]\n");

	waittime -= rtt;

	if (waittime > 0) 
		return _ping(data, packet_out, packet_in, addr, waittime, false);

	return false;
}

void	ft_traceroute(_data *data, _op_vars *op_vars) {
	char		packet_out[def_packet_size];
	char		packet_in[def_packet_size];
	struct	icmphdr	*icmphdr_out;
	struct	iphdr	*iphdr_out;
	struct	sockaddr_in re_addr;


	char		hostname[max_hostname_len];
	char		last_seen_ip[max_addr_len];
	char		*res_ip;

	size_t		icmphdr_len = sizeof(struct icmphdr);
	size_t		iphdr_len = sizeof(struct iphdr);
	socklen_t		addr_len = sizeof(struct sockaddr_in);

	uint16_t		seq;
	uint8_t		query;
	double		rtt;

	ft_memset(packet_out, 0, def_packet_size);
	iphdr_out = (struct iphdr*)packet_out;
	iphdr_out->version = 4;
	iphdr_out->ihl = iphdr_len / 4;
	iphdr_out->id = htons(op_vars->npid);
	iphdr_out->tos = op_vars->tos;
	iphdr_out->protocol = IPPROTO_ICMP;
	iphdr_out->saddr = data->src.s_addr;
	iphdr_out->daddr = data->dest.d_addr;
	iphdr_out->tot_len = htons(iphdr_len + icmphdr_len + payload_size);


	icmphdr_out = (struct icmphdr*)(packet_out + iphdr_len);
	icmphdr_out->type = ICMP_ECHO;
	icmphdr_out->un.echo.id = htons(op_vars->npid);

	for (uint8_t cu_by = 0; cu_by < payload_size; cu_by += 2) {
		ft_memset(packet_out + iphdr_len + icmphdr_len + cu_by, payload_char_1, 1);
		ft_memset(packet_out + iphdr_len + icmphdr_len + cu_by + 1, payload_char_2, 1);
	}

	for (;;) {
		iphdr_out->check = 0;
		iphdr_out->ttl = op_vars->ttl;
		iphdr_out->check = csum((unsigned short*)packet_out, iphdr_len/2);

		ft_memset(last_seen_ip, 0, max_addr_len);
		for (query=0, seq=1; query < op_vars->nqueries; query++, seq++) {

			icmphdr_out->checksum = 0;
			icmphdr_out->un.echo.sequence = htons(seq);
			icmphdr_out->checksum = csum((unsigned short*)(packet_out + iphdr_len), (icmphdr_len+payload_size)/2);


			if (!query) {
				//printf("\n\n");
				print_ttl(op_vars->ttl);
				//printf("\n");
			}

			rtt = _ping(data, packet_out, packet_in, &re_addr, op_vars->waittime.tv_sec * 1000, true);
			if (rtt < 0) return;

			if (!rtt) printf("  *");
			else {
				res_ip = inet_ntoa(re_addr.sin_addr);
				if (ft_strcmp(last_seen_ip, res_ip)) {
					if (!data->input.numeric
						&& !getnameinfo((struct sockaddr*)&re_addr, addr_len,
							hostname, max_hostname_len, NULL, 0, 0)) 
						printf(" %s", hostname);
					printf(" (%s)", res_ip);
				}
				ft_memcpy(last_seen_ip, res_ip, ft_strlen(res_ip));
				printf("  %0.3f ms", rtt);
			}
			printf(" %c", query+1 == op_vars->nqueries ? '\n' : '\b');
		}

		op_vars->ttl += 1;
		if (op_vars->ttl >= op_vars->max_ttl)
			break;
		
		if (rtt && packet_in[(packet_in[0]&0xf)*4] == ICMP_ECHOREPLY)
			break;
	}
}
