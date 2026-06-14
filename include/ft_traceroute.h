#ifndef FT_TRACEROUTE_
# define FT_TRACEROUTE

# define	_GNU_SOURCE

# include <stdio.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <netdb.h>
# include <string.h>
# include <stdlib.h>
# include <netinet/ip_icmp.h>
# include <unistd.h>
# include <sys/select.h>
# include <errno.h>
# include <signal.h>
# include <sys/time.h>
# include <ifaddrs.h>
# include <net/if.h>
# include <sys/ioctl.h>

// GENERAL PURPOSE MACROS
# define ft_traceroute_version	"0x1 <foundations.>"
# define failback_interface		"enp0s3"
# define def_packet_size		4000
# define max_interface_len		0x10	
# define max_addr_len		0x10
# define max_hostname_len		64
# define def_max_ttl		30
# define def_waittime		5
# define def_first_ttl		1
# define def_nqueries		3
# define max_nqueries_value		10
# define def_tos			0
# define icmp_types			((const char*[]){ "echoreply", "", "", "destination_unreachable", "source_quench", "redirect_msg", "", "", "echo_request", "router_ad", "router_solic", "time_to_live_exceeded", "bad_ip_header", "timestamp", "timestamp_reply", "info_request", "info_reply", "addr_mask_request", "addr_mask_reply"})

// ANSI COLORS
# define UND		"\033[4m"
# define CYN		"\x1B[36m"
# define NRM		"\x1B[0m"
# define WHT		"\x1B[37m"

// KBYTES UNITS IN BYTES
# define _64KB		0x10000
# define _32KB		0x8000
# define _16KB		0x4000
# define _08KB		0x2000
# define _04KB		0x1000

// UNSINGED INTEGER TYPES MAX VALUES
# define U64_MAX		0xffffffffffffffff
# define U32_MAX		0xffffffff
# define U16_MAX		0xffff
# define U08_MAX		0xff

# ifndef True 
# define True 		1
# endif

# ifndef False
# define False		0
# endif

typedef	int	bool;

typedef	struct {
	bool		version;		// -V / --version
	bool		help;		// -? / --help
	bool		numeric;		// -n / --numeric
	//
	bool		is_set_interface;
	bool		is_set_src_addr;
	bool		is_set_max_ttl;
	bool		is_set_first_ttl;
	bool		is_set_nqueries;
	bool		is_set_waittime;
	bool		is_set_tos;
	//
	char		interface[max_interface_len];
					// -i / --interface
	char		src_addr[max_addr_len];
					// -s / --source-addr
	uint32_t		max_ttl;		// -m / --max-ttl
	uint32_t		first_ttl;	// -f / --first-ttl
	uint8_t		nqueries;		// -q / --nqueries
	uint32_t		waittime;		// -w / --waittime
	uint8_t		tos;		// -t / --tos
}	_options;

typedef	struct {
	struct	addrinfo	addr;
	char		ip[max_addr_len];
	uint32_t		s_addr;
}	_src;

typedef	struct {
	struct	addrinfo	*addr;
	char		ip[max_addr_len];
	uint32_t		d_addr;
}	_dest;

typedef	struct {
	uint16_t		npid;
	uint8_t		ttl;
	uint8_t		max_ttl;
	uint8_t		nqueries;
	uint8_t		tos;
	struct	timeval	waittime;
}	_op_vars;

typedef	struct {
	int		sock;
	_src		src;
	_dest		dest;
	_options		input;
}	_data;

// print.c
void	display_help();
void	print_outgoing_packet();
void	print_incoming_packet();

// parse.c
bool	parse_params(int, char**, _data*);

// ft_traceroute.c
void	ft_traceroute(_data*, _op_vars*);

#endif
