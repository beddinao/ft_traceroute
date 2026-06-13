#include <ft_traceroute.h>

bool 	op_general_check(bool op_var) {
	return (op_var);
}

bool	op_numeric(int c, char **v, int *current_arg, _data* data) {
	if (op_general_check(data->input.numeric)) return False;
	data->input.numeric = True;
	return True;
}

bool 	op_interface(int c, char **v, int *current_arg, _data* data) {
	if (op_general_check(data->input.is_set_interface)
		|| c - (*current_arg + 1) < 2) return False;

	(*current_arg)++;
	size_t	inp_size = strlen(v[*current_arg]);
	if (inp_size > max_interface_len) {
		printf("invalid \"Interface\" value: %s\n", v[*current_arg]);
		return False;
	}

	data->input.is_set_interface = True;
	memcpy(data->input.interface, v[*current_arg], inp_size);
	return True;
}

bool 	op_src_addr(int c, char **v, int *current_arg, _data* data) {
	if (op_general_check(data->input.is_set_src_addr)
		|| c - (*current_arg + 1) < 2) return False;

	(*current_arg)++;
	size_t	inp_len = strlen(v[*current_arg]);
	if (inp_len > max_addr_len) {
		printf("invalid \"Source Address\" value: %s\n", v[*current_arg]);
		return False;
	}

	data->input.is_set_src_addr = True;
	memcpy(data->input.src_addr, v[*current_arg], inp_len);
	return True;
}

bool	op_max_ttl(int c, char **v, int *current_arg, _data* data) {
	if (op_general_check(data->input.is_set_max_ttl)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	long	max_ttl_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| max_ttl_nm < 0 || max_ttl_nm >= U32_MAX) {
		printf("invalid \"Max TTL\" value: %s\n", v[*current_arg]);
		return False;
	}
	data->input.is_set_max_ttl = True;
	data->input.max_ttl = max_ttl_nm;
	return True;
}

bool	op_first_ttl(int c, char **v, int *current_arg, _data *data) {
	if (op_general_check(data->input.is_set_first_ttl)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	long	first_ttl_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| first_ttl_nm < 0 || first_ttl_nm > U32_MAX) {
		printf("invalid \"First TTL\" value: %s\n", v[*current_arg]);
		return False;
	}
	data->input.is_set_first_ttl = True;
	data->input.first_ttl = first_ttl_nm;
	return True;
}

bool	op_nqueries(int c, char **v, int *current_arg, _data *data) {
	if (op_general_check(data->input.is_set_nqueries)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	long	nqueries_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| nqueries_nm < 0 || nqueries_nm > U08_MAX) {
		printf("invalid \"N queries\" value: %s\n", v[*current_arg]);
		return False;
	}
	data->input.is_set_nqueries = True;
	data->input.nqueries = nqueries_nm;
	return True;
}

bool	op_waittime(int c, char **v, int *current_arg, _data *data) {
	if (op_general_check(data->input.is_set_waittime)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	long	waittime_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| waittime_nm < 0 || waittime_nm > U32_MAX) {
		printf("invalid \"waittime\" value: %s\n", v[*current_arg]);
		return False;
	}
	data->input.is_set_waittime = True;
	data->input.waittime = waittime_nm;
	return True;
}

bool	op_tos(int c, char **v, int *current_arg, _data *data) {
	if (op_general_check(data->input.is_set_tos)
		|| c - (*current_arg + 1) < 2) return False;
	errno = 0;
	(*current_arg)++;
	char	*endptr;
	long	tos_nm = strtol(v[*current_arg], &endptr, 0xa);
	if (errno == ERANGE
		|| endptr == v[*current_arg] || *endptr != '\0'
		|| tos_nm < 0 || tos_nm > U08_MAX) {
		printf("invalid \"Type Of Service (TOS/QOS)\" value: %s\n", v[*current_arg]);
		return False;
	}
	data->input.is_set_tos = True;
	data->input.tos = tos_nm;
	return True;
}

bool parse_params(int c, char **v, _data *data) {

	bool	valid_arg;
	char	possible_one_char_param[0x3];
	char	*one_char_args = "-n-i-s-m-f-q-w-t";
	char	*full_name_args[0x8] = {
		"--numeric", "--interface", "--source-addr", "--max-ttl",
		"--first-ttl", "--nqueries", "--waittime", "--tos"
	};
	bool	(*param_ops[0x9])(int, char**, int*, _data*) = {
		op_numeric, op_interface, op_src_addr, op_max_ttl,
		op_first_ttl, op_nqueries, op_waittime, op_tos
	};


	if (c == 2 && (!strcmp(v[c-1], "-V") || !strcmp(v[c-1], "--version"))) {
		printf("ft_traceroute: %s\n\n", ft_traceroute_version);
		exit(False);
	}
	else if (c == 2 && (!strcmp(v[c-1], "-?") || !strcmp(v[c-1], "--help")))
		return False;

	for (int current_arg = 1; current_arg < c - 1; current_arg+=1) {
		valid_arg = False;
		for (uint8_t current_possible_param = 0; current_possible_param < 0x9; current_possible_param += 1) {
			possible_one_char_param[0] = one_char_args[current_possible_param*2];
			possible_one_char_param[1] = one_char_args[current_possible_param*2+1];
			possible_one_char_param[2] = '\0';
			if (!strcmp(v[current_arg], full_name_args[current_possible_param])
				|| !strcmp(v[current_arg], possible_one_char_param)) {
				valid_arg = param_ops[current_possible_param](c, v, &current_arg, data);
				break;
			}
		}
		if (!valid_arg)
			return False;
	}

	return True;
}
