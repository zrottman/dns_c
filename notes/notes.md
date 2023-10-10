# 2023-10-10
Accomplished
- fixed buffer overflow issues in parse_question() and parse_record()
- revised DNSRecord struct to include members for parsed/human-readable version of payload as well as the length of that parsed content. The software seems to be correctly parsing A record responses as human-friendly IPv4 addresses, but we haven't tested it yet on an NS record to ensure that the domain name is correctly being decoded. One issue we encountered at the end, though, is some sort of seg fault when receiving larger responses. For instance, when querying 198.41.0.4 for "google.com" (per example in guide), we receive something like 488 bytes before a seg fault/memory violation of some kind occurs. Based on the fact that, in the example, this query returns 13 authorities and 12 additional, it makes sense that the response is going to be long and is overflowing something somewhere, however increasing the size of buf (where response bytes are stored) from 512 to 1000 did not create any new behavior. So this needs troubleshooting.

To Do
- implement destroy_packet() function that destroys all member structs and internal char * members (from last week)
- consider implementing a struct called payload that has a char\* and length which stores parsed human-readable payload for DNSRecord
- Rewrite functions so that caller is responsible for mallocing and freeing space (from last week)

# 2023-10-03
To do
-  Rewrite functions so that caller is responsible for mallocing and freeing space
- create destroy functions for all mallocs
- ask around about unexpected malloc/string behavior

# 2023-09-26
Accomplished
- abstracted out parse_questions() and parse_records() functions to handle linked list creation in NewDNSPacket() for DNSPacket->questions, DNSPacket->answers, DNSPacket->authorities, and DNSPacket->additionals members.

To do
- clean up code and open PR to merge parse_dns_packet into main
- move on to Part III

# 2023-09-23
Accomplished
- create DNSPacket struct and relocate packet parsing functions to NewDNSPacket() constructor

To do
- rewrite NewDNSQuery() so that it returns pointer to DNSQuery struct rather than a struct
- change file organization/naming: dns.c -> main.c, request.c -> dns.c, request.h -> dns.h
- relocate packet component display functions to display_packet()
- abstract out parsing functions for creating linked lists for questions, answers, authorities, additionals
- add comment about bitshift rationale in display_DNSRecord(), esp. how to maintain portability we build 32-bit int in host order and convert to network byte order to pass to inet_pton()
- after cleaning up NewDNSPacket() and factoring out linked list code, open PR to merge new branch parse_dns_packet into main

# 2023-09-12
Accomplished today

- nearly finished §2
- Added linked list support to DNSQuestion and DNSResponse structs
- Added support for multiple questions when parsing response
- Added support for multiple answers when parsing response

To do:
- Add support for additionals and authorities when parsing response
- Refactor parsing functions for linked list creation to single, generic parse function that takes an enum specifying what type of response (i.e., question, answer, authority, additional) so that the function can correctly allocate and populate DNSQuestion or DNSResponse structs
- finish §2
- merge sendto to main and create new branch for §3

# 2023-09-05
Accomplished today
- finished implementing decode_compressed_name() and integrate into decode_name()
- successfully parse and display question and record structs

To do
- pick up from section 2.8 of Implement DNS in a Weekend: Parse DNS packet:
- Create DNSPacket struct to hold DNSHeader, a list of DNSQuestion structs, and lists of DNSRecord structs for answers, authorities, and additionals
- Add \*next members to DNSQuestion and DNSRecord structs so they can be handled as linked lists in DNSPacket struct above
- add makefile for compiling
- update .gitignore to ignore a.out

# 2023-08-22
Accomplished
- implemented parse_question(), which populates DNSQuestion struct with decoded name(from decode_name()) plus next 4 bytes of response for type and class
- implemented a decode_compressed_name() function that decodes the next two bytes of the response, which contain pointer information for the domain name (re: DNS compression)

To Do:
- implement new decode_name() function that uses decode_compressed_name(). If a byte starts with 0xc0, then decode_compressed_name(), else decode as normal. (?)
- implement parse_record() function, which populates DNSRecord struct with decoded name and response bytes (type, class, ttl, and data)

# 2023-08-15
Accomplished
- Started implementing functions to parse response bytes.
- created DNSRecord struct that will eventually be populated with the DNS response details
- implemented parse_header(), which populates a DNSHeader struct with the first 12 bytes of the response and returns the number of the current byte (e.g., 12)
- implemented decode_name() function, which decodes the domain name chunk of the response into human-readable format (e.g., \x03www\x07example\x03com\x00 => www.example.com) and returns the number of the current byte

To do:
- implement parse_question() function, which populates DNSQuestion struct with decoded name (from decode_name() above) plus 4 bytes for type and class.
- implement parse_record() function, which populates DNSRecord struct with decoded name and response bytes (type, class, and data)

# 2023-08-08
Accomplished 
- concatenate header_bytes and question_bytes into a single query_bytes buffer
- send those bytes over UDP, to IPv4 address 8.8.8.8, port 53
- used wireshark to look at the response (really nice program, wow)
- took a bit of fiddling to find the right filter -- I think the one we settled on was ip.addr==8.8.8.8
- interesting tidbit: we were also seeing ICMP ~"destination does not exist" messages coming from our computer to 8.8.8.8, after the DNS request & response
- we then added a recvfrom call to the code, to grab the incoming bytes of the response. After that, these ICMP packets stopped showing up when we ran our program.
- seems like the operating system is deciding to do something behind the scenes when our program is asleep at the wheel for long enough? interesting

To do:
- parse the response, using code!
- "cheap, hacky way": it's just the last 4 bytes (lol)
- might also do it the "proper" way, for learning

# 2023-08-01
Accomplished
- implemented question_to_bytes()

To do
- finish implementing build_query(), which just needs to concatenate header_bytes and question_bytes
- send it

# 2023-07-25
Accomplished
- finished encode_dns_name()
- finished header_to_bytes()

To do: 
- implement question_to_bytes
- implement build_query that concatenates header_to_bytes() and question_to_bytes()
- send the result!

# 2023-07=18
Accomplished 
- debug embedding byte values in string literal: "\x07example\x03com" wasn't working as expected (we still don't know why, but we found a workaround)
- change sendall helper fn to use sendto for UDP instead of send for TCP
- make a (successful!!) query to 8.8.4.4 -- eyeball the req/resp in wireshark (very cool)
- define dns header structs, and constructor functions to allocate new ones
- implement encode_dns_name to turn example.com into 7example3com (but not the ASCII encoding of '7', just the raw byte value of 7)
- tested encode_dns_name with print statements and piping to hexdump -C (very cool)

To do:
-  use encode_dns_name and some C magic to stuff the struct data into a buffer
-  push the bytes in that buffer over the wire
-  test it and hopefully see a DNS response!
