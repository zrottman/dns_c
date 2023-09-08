# DNS Resolver (in progress)

This collaborative project is a toy DNS resolver written in C and based on Julia Evans' [Implement DNS in a Weekend](https://implement-dns.wizardzines.com/) Python guide.

The program features `DNSQuery`, `DNSHeader`, `DNSQuestion`, and `DNSRecord` structs that maintain the various component parts of a DNS request and the parsed DNS response. At a high level, the program currently performs as follows:
1. open a socket fild descriptor using `socket()`
1. instantiate and populate a `DNSQuery` struct with details about the DNS request using constructor function `NewDNSQuery()`
1. send above `DNSQuery` struct using `sendto()`
1. receive response with `recvfrom()`
1. close socket file descriptor with `close()
1. parse response into `DNSHeader`, `DNSQuestion`, and `DNSRecord` structs

For working branch, see [sendto](https://github.com/zrottman/dns_c/tree/sendto).

## Sandbox and Side Projects

The `sandbox/` directory contains a number of DNS-related and DNS-adjacent experients and toy projects designed to improve intuition about networking in general and C's networking systems calls.
