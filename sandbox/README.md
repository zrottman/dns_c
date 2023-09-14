# Sandbox

- btoi.c: toy program to convert bitstring[32] to uint32_t
- connect.c: experimenting with opening socket descriptor, connecting to 8.8.8.8, and sending DNS query
- getaddrinfo.c: get to know `getaddrinfo()` function by using it to take human-readable domains (e.g., www.example.com) from REPL input and resolve IP address(es). Based on example from [Beej's guide](https://beej.us/guide/bgnet/html/split/system-calls-or-bust.html#system-calls-or-bust)
- inet_pton.c: toy implementation of `inet_pton()`, which takes IPv4 domain from REPL input (e.g., 123.45.6.78) and converts it to uint32_t, both in host and network byte order.
- ipv4_to_i.c: another toy implementation of an `inet_pton()`-like function, that takes an IPv4 domain from REPL input and converts to uint32_t in network byt order. Unlike `inet_pton.c`, this program does not utilize `htonl()`.
- structs_explore.c: short program that prints out the size and addresses of the following structs and their members in an effort to gain better intuition for them: `adderinfo`, `sockaddr`, `sockaddr_in`, `in_addr`. 
