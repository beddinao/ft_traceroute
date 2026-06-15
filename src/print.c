/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: beddinao <beddinao@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/13 01:16:14 by beddinao          #+#    #+#             */
/*   Updated: 2026/06/14 19:18:01 by beddinao         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
