# *`ft_traceroute`*
`42 cursus compliant`

A minimal command-line utility to trace the path packets take to reach a specified network destination. by sending packets with increasing time-to-live (TTL) values, displaying each hop along the route.

## usage

```
Usage
  ft_traceroute [options] <destination>

Options:
  <destination>               dns name or ip address
  -i --interface <ifr>
                              specifies the interface through which ft_traceroute should send packets
                              by default the interface is selected according to the routing table
                              this option is ignored if [-s/--source-addr] is present.
  -s --source <source_addr>
                              chooses an alternative source address.
                              must be of one of the interfaces.
                              by default the address of the chosen interface is used.
  -m --max-hops <max_ttl>
                              specifies the maximum number of hops (max time-to-live value).
                              must be > 0 and less than 0xff.
                              the default is 30.
  -f --first <first_ttl>
                              specifies with what TTL to start.
                              must be >= 0 and less than 0xff.
                              the default is 1.
  -q --queries <nqueries>
                              sets the number of probe packets per hop/ttl.
                              must be >= 1. rounded to 0x3e8 if greater.
                              the default is 3.
  -w --wait <s_timeout>
                              determines how many seconds to wait for a response to a probe
                              must be less than 0xffffffff.
                              the default is 3
  -t --tos <tos>
                              set the Type Of Service (TOS).
                              must be >= 0 and less than 0xff
                              the default is 0

  -n --numeric                do not try to map IP addr to host names
  -V --version                prints version and exits
  -v --verbose                be verbose
  -? --help                   shows this help and exits
```

## example output

```
$ ft_traceroute --max-hops 1 --queries 1 --verbose google.com
[2026-06-17 17:55:45] No Source Addr/Interface were given
[2026-06-17 17:55:45] Looking for a valid Interface on this System..
[2026-06-17 17:55:45] Conditions to apply: [IFF_UP], ![IFF_LOOPBACK], [IFF.sa_family:AF_INET(IPv4)]
[2026-06-17 17:55:45] Found a Valid IPv4 Interface "enp0s3"
[2026-06-17 17:55:45] Resolved "enp0s3" Address: 10.11.100.206
ft_traceroute to google.com (192.178.25.110), 1 hops max, 64 byte packets
 1 
[2026-06-17 17:55:45]  ==== sent pack: ====
ip_dest: [192.178.25.110]
out_ip_hdr   : 45 00 00 5c 04 6f 00 00 01 01 6c 39 0a 0b 64 ce c0 b2 19 6e 
out_icmp_hdr : 08 00 8c ad 04 6f 00 01 

[2026-06-17 17:55:45]  ==== recved pack: ====
ip_source: [10.11.254.254]
out_ip_hdr   : 45 00 00 38 84 6f 00 00 ff 01 bf 72 0a 0b fe fe 0a 0b 64 ce 
out_icmp_hdr : 0b 00 5b e2 00 00 00 00 
in_ip_hdr    : 45 00 00 5c 04 6f 00 00 01 01 6c 39 0a 0b 64 ce c0 b2 19 6e 
in_icmp_hdr  : 08 00 8c ad 04 6f 00 01 
[MINE]

 10.11.254.254 (10.11.254.254)  0.662 ms
```

```
$ ft_traceroute --max-hops 50 162.252.205.130
ft_traceroute to 162.252.205.130 (162.252.205.130), 50 hops max, 64 byte packets
 1  10.11.254.254 (10.11.254.254)  0.881 ms  0.583 ms  0.557 ms 
 2  nat.1337.ma (10.21.1.1)  0.376 ms  0.316 ms  0.308 ms 
 3  197.230.30.145 (197.230.30.145)  0.323 ms  0.303 ms  0.413 ms 
 4  10.43.88.105 (10.43.88.105)  1.680 ms  0.886 ms  0.769 ms 
 5  10.43.82.105 (10.43.82.105)  10.463 ms  10.483 ms  10.326 ms 
 6  10.43.82.106 (10.43.82.106)  10.548 ms  10.314 ms  10.621 ms 
 7  10.43.250.213 (10.43.250.213)  26.293 ms  26.099 ms  26.113 ms 
 8   *  *  * 
 9  centurylink.mad05.atlas.cogentco.com (130.117.14.46)  26.708 ms  47.424 ms  45.968 ms 
10  ae3.3.bar4.Toronto1.net.lumen.tech (4.69.140.130)  138.256 ms  140.379 ms  135.881 ms 
11  level3-gw.core02.tor1.prioritycolo.com (4.16.51.30)  167.057 ms  167.147 ms  167.241 ms 
12  67.223.96.90 (67.223.96.90)  160.287 ms  160.340 ms  160.494 ms 
13  bad.horse (162.252.205.130)  163.466 ms  163.043 ms  163.337 ms 
14  bad.horse (162.252.205.131)  168.771 ms  168.615 ms  168.703 ms 
15  bad.horse (162.252.205.132)  173.986 ms  172.724 ms  173.984 ms 
16  bad.horse (162.252.205.133)  181.618 ms  181.702 ms  181.588 ms 
17  he.rides.across.the.nation (162.252.205.134)  201.508 ms  200.927 ms  201.309 ms 
18  the.thoroughbred.of.sin (162.252.205.135)  187.098 ms  187.080 ms  187.086 ms 
19  he.got.the.application (162.252.205.136)  200.567 ms  200.770 ms  201.002 ms 
20  that.you.just.sent.in (162.252.205.137)  195.663 ms  195.560 ms  195.709 ms 
21  it.needs.evaluation (162.252.205.138)  203.970 ms  203.566 ms  203.742 ms 
22  so.let.the.games.begin (162.252.205.139)  206.074 ms  205.795 ms  206.527 ms 
23  a.heinous.crime (162.252.205.140)  211.442 ms  211.702 ms  211.896 ms 
24  a.show.of.force (162.252.205.141)  220.578 ms  224.452 ms  220.308 ms 
25  a.murder.would.be.nice.of.course (162.252.205.142)  239.953 ms  239.357 ms  239.357 ms 
26  bad.horse (162.252.205.143)  222.989 ms  223.539 ms  223.213 ms 
27  bad.horse (162.252.205.144)  230.290 ms  230.058 ms  230.144 ms 
28  bad.horse (162.252.205.145)  250.671 ms  250.497 ms  250.619 ms 
29  he-s.bad (162.252.205.146)  243.920 ms  244.396 ms  243.805 ms 
30  the.evil.league.of.evil (162.252.205.147)  247.082 ms  247.470 ms  246.974 ms 
31  is.watching.so.beware (162.252.205.148)  252.281 ms  265.330 ms  252.545 ms 
32  the.grade.that.you.receive (162.252.205.149)  258.762 ms  258.502 ms  258.578 ms 
33  will.be.your.last.we.swear (162.252.205.150)  265.163 ms  265.092 ms  265.774 ms 
34  so.make.the.bad.horse.gleeful (162.252.205.151)  274.297 ms  *  270.792 ms 
35  or.he-ll.make.you.his.mare (162.252.205.152)  275.435 ms  275.514 ms  275.716 ms 
36  o_o (162.252.205.153)  280.364 ms  280.909 ms  280.185 ms 
37  you-re.saddled.up (162.252.205.154)  282.464 ms  282.804 ms  282.136 ms 
38  there-s.no.recourse (162.252.205.155)  296.081 ms  289.981 ms  289.418 ms 
39  it-s.hi-ho.silver (162.252.205.156)  294.211 ms  294.299 ms  293.638 ms 
40  bad.horse (162.252.205.130)  294.232 ms  292.961 ms  293.062 ms
```

## installation

1. clone this repository:
   ```bash
   git clone https://github.com/beddinao/ft_traceroute && cd ft_traceroute
   ```
2. build and install (requires root to install):
   ```bash
   make
   sudo make install
   ```
3. run as root:
   ```bash
   sudo ft_traceroute -?
   ```

## requirements

- Linux
- Root privileges (required for using raw sockets)
- A C compiler and `make`
