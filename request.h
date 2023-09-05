#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

#define TYPE_A   1
#define CLASS_IN 1

typedef struct Query
{
	size_t len;
	char*  s;
} Query;

typedef struct DNSHeader
{
    // stored in network byte order
    u_int16_t id;
    u_int16_t flags;
    u_int16_t num_questions;
    u_int16_t num_answers;
    u_int16_t num_authorities;
    u_int16_t num_additionals;
} DNSHeader;

typedef struct DNSQuestion
{
    // stored in network byte order
    char      *encoded_name;
    u_int16_t type;   // set to TYPE_A=1 (A Record?)
    u_int16_t class;  // set to CLASS_IN=1 (for internet)
} DNSQuestion;

typedef struct DNSRecord
{
    // stored in network byte order
    char     *name; // domain name
    uint16_t type;  // e.g., A record
    uint16_t class; // 1
    uint32_t ttl;   // time to live
    uint16_t data_len; // NOTE: net-byte-order!
    uint8_t  *data_bytes; // the record's content
} DNSRecord;

DNSHeader*   NewDNSHeader(u_int16_t id, u_int16_t flags, u_int16_t num_questions);
DNSQuestion* NewDNSQuestion(char *encoded_name, int type, int class);
Query        NewDNSQuery(char *domain_name, int record_type);

void         display_DNSHeader(DNSHeader *header);
void         display_DNSQuestion(DNSQuestion *question);

size_t       encode_dns_name(char* domain_name, char* res);
void         header_to_bytes(DNSHeader *header, char *header_bytes);
void         question_to_bytes(DNSQuestion *question, char *question_bytes);
size_t       parse_header(char* response_bytes, DNSHeader *header);
int          decode_name(char* response_bytes, char *decoded_name, int bytes_in);
int          parse_question(DNSQuestion *question, char* response_bytes, int bytes_in);
int          decode_compressed_name(char* response_bytes, int bytes_in);

#endif // REQUEST_H
