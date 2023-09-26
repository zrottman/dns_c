#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>

#define TYPE_A   1
#define CLASS_IN 1

// TODO: Consider defining structs in request.c but adding prototypes here, eg:
// typedef struct DNSQuery DNSQuery;
typedef struct DNSQuery
{
	size_t len;
	char*  s;
} DNSQuery;

typedef struct DNSHeader
{
    // stored in network byte order
    uint16_t id;
    uint16_t flags;
    uint16_t num_questions;
    uint16_t num_answers;
    uint16_t num_authorities;
    uint16_t num_additionals;
} DNSHeader;

typedef struct DNSQuestion
{
    // stored in network byte order
    char                *encoded_name;
    u_int16_t           type;       // set to TYPE_A=1 (A Record)
    u_int16_t           class;      // set to CLASS_IN=1 (for internet)
    struct DNSQuestion *next;       // linked list next pointer
} DNSQuestion;

typedef struct DNSRecord
{
    // stored in network byte order
    char             *name;        // domain name
    uint16_t          type;        // e.g., A record
    uint16_t          class;       // 1
    uint32_t          ttl;         // time to live
    uint16_t          data_len;    // NOTE: net-byte-order!
    uint8_t          *data_bytes;  // the record's content
    struct DNSRecord *next;        // linked list next pointer
} DNSRecord;

typedef struct DNSPacket
{
    DNSHeader       *header;
    DNSQuestion     *questions;
    DNSRecord       *answers;
    DNSRecord       *authorities;
    DNSRecord       *additionals;
} DNSPacket;

DNSHeader*   NewDNSHeader(uint16_t id, uint16_t flags, uint16_t num_questions);
DNSQuestion* NewDNSQuestion(char *encoded_name, uint16_t type, uint16_t class);
DNSQuery*    NewDNSQuery(char *domain_name, uint16_t record_type);
DNSPacket*   NewDNSPacket(const char *response_bytes);

void         display_DNSHeader(DNSHeader *header);
void         display_DNSQuestion(DNSQuestion *question);
void         display_DNSRecord(DNSRecord *record);
void         display_DNSPacket(DNSPacket *packet);

size_t        encode_dns_name(char* domain_name, char* res);
void         header_to_bytes(DNSHeader *header, char *header_bytes);
void         question_to_bytes(DNSQuestion *question, char *question_bytes);
size_t        parse_header(const char* response_bytes, DNSHeader *header);
int          parse_question(const char* response_bytes, int bytes_in, DNSQuestion *question);
int          parse_record(const char* response_bytes, int bytes_in, DNSRecord *record);
int          parse_records(const char *response_bytes, int bytes_read, int num_records, DNSRecord **head);
int          parse_questions(const char *response_bytes, int bytes_read, int num_questions, DNSQuestion **head);
int          decode_name(const char* response_bytes, int bytes_in, char *decoded_name);
int          decode_compressed_name(const char* response_bytes, int bytes_in, char *decoded_name);

#endif // REQUEST_H
