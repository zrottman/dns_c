#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>
#include <arpa/inet.h>

#define TYPE_A   1
#define TYPE_NS  2
#define CLASS_IN 1
#define MAX_BUFFER_SIZE 1000

// TODO: Consider defining structs in request.c but adding prototypes here, eg:
// typedef struct DNSQuery DNSQuery;
typedef struct DNSQuery
{
	size_t  len;
	char   *s;
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
    char               *name;
    uint16_t            type;       // set to TYPE_A=1 (A Record)
    uint16_t            class;      // set to CLASS_IN=1 (for internet)
    struct DNSQuestion *next;       // linked list next pointer
} DNSQuestion;

typedef struct DNSRecord
{
    // stored in network byte order
    char             *name;       // domain name
    uint16_t          type;       // e.g., A record
    uint16_t          class;      // 1
    uint32_t          ttl;        // time to live
    uint16_t          bytes_len;  // length of data bytes NOTE: net-byte-order!
    size_t            data_len;   // length of human readable data
    uint8_t          *data_bytes; // the record's content
    char             *data;       // human readable data 
    struct DNSRecord *next;       // linked list next pointer
} DNSRecord;

typedef struct DNSPacket
{
    DNSHeader   *header;
    DNSQuestion *questions;
    DNSRecord   *answers;
    DNSRecord   *authorities;
    DNSRecord   *additionals;
} DNSPacket;

DNSHeader*   NewDNSHeader(uint16_t id, uint16_t flags, uint16_t num_questions);
DNSQuestion* NewDNSQuestion(char *encoded_name, uint16_t type, uint16_t class);
DNSQuery*    NewDNSQuery(char *domain_name, uint16_t record_type);
DNSPacket*   NewDNSPacket(const unsigned char *response_bytes);

int          destroy_DNSPacket(DNSPacket** packet);
int          destroy_DNSHeader(DNSHeader** header);
int	     destroy_DNSQuery(DNSQuery** query);
int          destroy_DNSQuestion(DNSQuestion** question);
int          destroy_DNSRecord(DNSRecord** record);

void         display_DNSHeader(DNSHeader *header);
void         display_DNSQuestion(DNSQuestion *question);
void         display_DNSRecord(DNSRecord *record);
void         display_DNSPacket(DNSPacket *packet);

size_t       encode_dns_name(char* domain_name, char* res);
void         header_to_bytes(DNSHeader *header, char *header_bytes);
void         question_to_bytes(DNSQuestion *question, char *question_bytes);
size_t       parse_header(const unsigned char* response_bytes, DNSHeader *header);
int          parse_questions(const unsigned char *response_bytes, int bytes_read, int num_questions, DNSQuestion **head);
int          parse_question(const unsigned char* response_bytes, int bytes_in, DNSQuestion *question);
int          parse_records(const unsigned char *response_bytes, int bytes_read, int num_records, DNSRecord **head);
int          parse_record(const unsigned char* response_bytes, int bytes_in, DNSRecord *record);

int          decode_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name);
int          decode_compressed_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name);

int get_answer(DNSPacket* packet, char buf[]);
int get_nameserver_ip(DNSPacket* packet, char buf[]);
int get_nameserver(DNSPacket* packet, char buf[]);
void resolve(char* domain_name, uint16_t record_type, char answer[]);


DNSPacket * send_query(char *addr, char *domain, u_int16_t type);
#endif // REQUEST_H
