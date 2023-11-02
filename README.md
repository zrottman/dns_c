# DNS Resolver

This collaborative project is a toy DNS resolver written in C and based on Julia Evans' [Implement DNS in a Weekend](https://implement-dns.wizardzines.com/) Python guide.

![Demo](demo/dns_demo.gif)

## Overview
The program features `DNSQuery`, `DNSHeader`, `DNSQuestion`, and `DNSRecord` structs that structure the various component parts of a DNS query and the parsed DNS response. At a high level, the program recursively resolves a given domain using the `resolve()` function. Each iteration of resolve does the following:
1. open a socket file descriptor using `socket()`
1. instantiate and populate a `DNSQuery` struct with details about the DNS request using constructor function `NewDNSQuery()`
1. send above `DNSQuery` struct bytes using `sendto()`
1. receive response with `recvfrom()` and store response bytes in a buffer
1. close socket file descriptor with `close()
1. parse response into a `DNSPacket` struct comprising of `header`, `questions`, `answers`, `authorities`, and `additionals` members. With the exception of the `header` member (a `DNSHeader` struct), the other components are linked lists of `DNSQuestion` and `DNSRecord` structs.

Based on the response, `resolve()` does one of three things:
1. If the response packet's `answers` member contains an `DNSRecord` struct with a valid answer, we're done!
1. Else if the response packet's `additionals` member contains a `DNSRecord` struct with a valid A record, send a new query for the original domain name to that nameserver IP.
1. Else if the response packet's `authorities` member contains a `DNSRecord` struct with a valid NS (nameserver) record, recursively `resolve` that nameserver domain for its IP address and, once obtained, send a new query for the original domain name to that newly-obtained nameserver IP.

## Sandbox and Side Projects

The `sandbox/` directory contains a number of DNS-related and DNS-adjacent experients and toy projects designed to improve intuition about networking in general and C's networking systems calls.
