#include <ft_traceroute.h>

bool	validate_source_addr(_data *data) {
	if (data->input.verbose)
		{print_current_time(); printf(" Detected Source Address, Validating...\n");}
	if (inet_pton(AF_INET, data->input.src_addr, &data->src.s_addr) <= 0) {
		print_current_time();
		printf("ft_traceroute: invalid IPv4 source address: %s\n", data->input.src_addr);
		return false;
	}
	char	*found_addr;
	bool	found_it = false;
	struct	ifaddrs	*o_ifa_list, *u_ifa_list;	
	if (getifaddrs(&o_ifa_list)) {
		perror("ft_traceroute: getifaddrs()");
		return false;
	}
	u_ifa_list = o_ifa_list;
	for(; u_ifa_list->ifa_next != NULL; u_ifa_list = u_ifa_list->ifa_next) {
		if (u_ifa_list->ifa_flags & IFF_LOOPBACK || !(u_ifa_list->ifa_flags & IFF_UP)
			|| u_ifa_list->ifa_addr->sa_family != AF_INET)
			continue;
		found_addr = inet_ntoa(((struct sockaddr_in*)u_ifa_list->ifa_addr)->sin_addr), data->input.src_addr;
		if (data->input.verbose) {
			print_current_time();
			printf(" Found a valid local IPv4 Interface %s (%s) checking Compatiblity..\n", u_ifa_list->ifa_name, found_addr);
			print_current_time(); printf(" Conditions to apply: [IFF_UP], ![IFF_LOOPBACK], [IFF.sa_family:AF_INET(IPv4)]\n");
		}
		if (found_addr && !ft_strcmp(data->input.src_addr, found_addr)) {
			if (data->input.verbose) {
				print_current_time();
				printf(" Given Address matches the Interface \"%s\", Using it!\n", u_ifa_list->ifa_name);
			}
			found_it = true;
			break;
		}
	}
	freeifaddrs(o_ifa_list);
	if (!found_it) {
		print_current_time();
		printf(" ft_traceroute: that Source Address does not belong to a valid interface\n");
	}
	return found_it;
}

bool	resolve_dest_addr(char *host, _data* data) {
	struct	addrinfo	hints;
	char		*_ip;
	int		i;
	
	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_flags = AI_PASSIVE|AI_CANONNAME;
	i = getaddrinfo(host, NULL, &hints, &data->dest.addr);
	if (i != 0 || !data->dest.addr) {
		printf("ft_traceroute: %s: Name or service not known\n", host);
		return false;
	}

	_ip = inet_ntoa(((struct sockaddr_in*)data->dest.addr->ai_addr)->sin_addr);
	data->dest.d_addr = ((struct sockaddr_in*)data->dest.addr->ai_addr)->sin_addr.s_addr;
	ft_memcpy(data->dest.ip, _ip, ft_strlen(_ip));
	return true;
}

bool	resolve_src_addr(_data *data) {
	if (data->input.is_set_src_addr) 
		return validate_source_addr(data);

	struct	ifreq	interface;
	ft_memset(&interface, 0, sizeof(interface));
	if (data->input.is_set_interface) { 
		if (data->input.verbose) {
			print_current_time();
			printf(" Detected given Interface \"%s\", using it!!\n", data->input.interface);
		}
		ft_memcpy(interface.ifr_name, data->input.interface, ft_strlen(data->input.interface));
	}
	else {
		bool	found_it = false;
		struct	ifaddrs	*o_ifa_list, *u_ifa_list;
		if (data->input.verbose) {
			print_current_time(); printf(" No Source Addr/Interface were given\n");
			print_current_time(); printf(" Looking for a valid Interface on this System..\n");
			print_current_time(); printf(" Conditions to apply: [IFF_UP], ![IFF_LOOPBACK], [IFF.sa_family:AF_INET(IPv4)]\n");
		}
		if (getifaddrs(&o_ifa_list)) {
			perror("ft_traceroute: getifaddrs()");
			return false;
		}
		u_ifa_list = o_ifa_list;
		for (; u_ifa_list->ifa_next != NULL; u_ifa_list = u_ifa_list->ifa_next) {
			if (u_ifa_list->ifa_flags & IFF_LOOPBACK || !(u_ifa_list->ifa_flags & IFF_UP)
				|| u_ifa_list->ifa_addr->sa_family != AF_INET)
				continue;
			ft_memcpy(interface.ifr_name, u_ifa_list->ifa_name, ft_strlen(u_ifa_list->ifa_name));
			if (data->input.verbose) {
				print_current_time();
				printf(" Found a Valid IPv4 Interface \"%s\"\n", u_ifa_list->ifa_name);
			}
			found_it = true;
			break;
		}
		freeifaddrs(o_ifa_list);
		if (!found_it) {
			print_current_time();
			printf("ft_traceroute: could not find a usable IPv4 interface!!\n");
			return false;
		}
	}
	
	if (ioctl(data->sock, SIOCGIFADDR, &interface) != 0) {
		print_current_time();
		if (data->input.is_set_interface) 
			printf(" ft_traceroute: invalid interface: %s\n", data->input.interface);
		else	perror(" ft_traceroute: ioctl()");
		return false;
	}
	char *_ip = inet_ntoa(((struct sockaddr_in*)&interface.ifr_addr)->sin_addr);
	data->src.s_addr = ((struct sockaddr_in*)&interface.ifr_addr)->sin_addr.s_addr;
	ft_memcpy(data->src.ip, _ip, ft_strlen(_ip));
	if (data->input.verbose) {
		print_current_time();
		printf(" Resolved \"%s\" Address: %s\n", interface.ifr_name, data->src.ip);
	}
	return true;
}

void	set_op_vars(_data *data, _op_vars *op_vars) {
	op_vars->ttl = data->input.is_set_first_ttl ? data->input.first_ttl : def_first_ttl;
	op_vars->max_ttl = data->input.is_set_max_ttl ? data->input.max_ttl : def_max_ttl;
	op_vars->nqueries = data->input.is_set_nqueries ? data->input.nqueries : def_nqueries;
	op_vars->tos = data->input.is_set_tos ? data->input.tos : def_tos;
	op_vars->waittime = data->input.is_set_waittime ? data->input.waittime*1000 : def_waittime*1000;
	/*
		op_vars->waittime<uint64_t> max: 0xffffffffffffffff 18446744073709551615
		data->input.waittime<uint32_t> max: 0xffffffff 4294967295
		uint32_t max * 1000 = 4.2949673 * (10 ** 12) == 4294967299999.9995 < 18446744073709551615
		so that operation "uint32_t * 1000 stored in a uint64_t" is safe;

	 */
	op_vars->npid = getpid();
}

void	enable_ip_hdr_mnp(_data *data) {
	int	_ops_res;
	int	_incd_ip_hdr = 1;
	
	_ops_res = setsockopt(data->sock, IPPROTO_IP, IP_HDRINCL, &_incd_ip_hdr, sizeof(_incd_ip_hdr));
	if (_ops_res < 0) {
		perror("ft_traceroute: setsockopt()");
		exit(1);
	}
}

bool	check_root_privileges(uid_t r, uid_t e, uid_t s) {
	return getresuid(&r, &e, &s) < 0 || e;
}

bool	gracefull_exit(_data *data) {
	if (data->sock > 0)
		close(data->sock);
	if (data->dest.addr)
		freeaddrinfo(data->dest.addr);
	return 0;
}

int main(int c, char **v) {
	if (check_root_privileges(0, 0, 0)) {
		printf("ft_traceroute: error: need root privileges\n");
		return 1;
	}

	_data	data;
	ft_memset(&data, 0, sizeof(_data));

	if (c < 2 || c > 0xff) {
		display_usage();
		return 1;
	}

	if (c >= 2 && !parse_params(c, v, &data)) {
		display_help();
		return 1;
	}

	data.sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (data.sock < 0) {
		perror("ft_traceroute: socket()");
		return 1;
	}

	if (!resolve_dest_addr(v[c-1], &data)) {
		close(data.sock);
		return 1;
	}

	if (!resolve_src_addr(&data))
		return gracefull_exit(&data);

	_op_vars		op_vars;
	ft_memset(&op_vars, 0, sizeof(op_vars));
	
	set_op_vars(&data, &op_vars);
	enable_ip_hdr_mnp(&data);
	print_header_intro(&data, &op_vars, v[c-1]);
	ft_traceroute(&data, &op_vars);
	return gracefull_exit(&data);
}
