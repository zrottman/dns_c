#ifndef INTERNALS_H
#define INTERNALS_H

#define CLASS_IN 1
#define MAX_BUFFER_SIZE 1000

/* -------------------------------------------
 * Struct Definitions
 * ------------------------------------------- */

/* The following structs are used to structure DNS queries and response packets.
 * All integer members are stored in network byte order.
 */
typedef struct DNSQuery
{
	size_t  len;
	char   *s;
} DNSQuery;

typedef struct DNSHeader
{
    uint16_t id;
    uint16_t flags;
    uint16_t num_questions;
    uint16_t num_answers;
    uint16_t num_authorities;
    uint16_t num_additionals;
} DNSHeader;

typedef struct DNSQuestion
{
    char               *name;
    uint16_t            type;       // set to TYPE_A=1 (A Record)
    uint16_t            class;      // set to CLASS_IN=1 (for internet)
    struct DNSQuestion *next;       // linked list next pointer
} DNSQuestion;

typedef struct DNSRecord
{
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

/* -------------------------------------------
 * DNS Header
 * ------------------------------------------- */
static DNSHeader*   NewDNSHeader(uint16_t id, uint16_t flags, uint16_t num_questions);
static size_t       parse_header(const unsigned char* response_bytes, DNSHeader *header);
static void         header_to_bytes(DNSHeader *header, char *header_bytes);
static int          destroy_DNSHeader(DNSHeader** header);
static void         display_DNSHeader(DNSHeader *header);

/* -------------------------------------------
 * DNS Question
 * ------------------------------------------- */
static DNSQuestion* NewDNSQuestion(char *encoded_name, uint16_t type, uint16_t class);
static int          destroy_DNSQuestion(DNSQuestion** question);
static void         display_DNSQuestion(DNSQuestion *question);
static void         question_to_bytes(DNSQuestion *question, char *question_bytes);
static int          parse_questions(const unsigned char *response_bytes, int bytes_read, int num_questions, DNSQuestion **head);
static int          parse_question(const unsigned char* response_bytes, int bytes_in, DNSQuestion *question);

/* -------------------------------------------
 * DNS Query
 * ------------------------------------------- */
static DNSQuery*    NewDNSQuery(char *domain_name, uint16_t record_type);
static int	        destroy_DNSQuery(DNSQuery** query);

/* -------------------------------------------
 * DNS Record
 * ------------------------------------------- */
static int          parse_records(const unsigned char *response_bytes, int bytes_read, int num_records, DNSRecord **head);
static int          parse_record(const unsigned char* response_bytes, int bytes_in, DNSRecord *record);
static int          destroy_DNSRecord(DNSRecord** record);
static void         display_DNSRecord(DNSRecord *record);

/* -------------------------------------------
 * DNS Packet
 * ------------------------------------------- */
static DNSPacket*   NewDNSPacket(const unsigned char *response_bytes);
static int          destroy_DNSPacket(DNSPacket** packet);
static void         display_DNSPacket(DNSPacket *packet);

/* -------------------------------------------
 * DNS Resolution
 * ------------------------------------------- */
static DNSPacket*   send_query(char *addr, char *domain, u_int16_t type);
static int          get_answer(DNSPacket* packet, char buf[]);
static int          get_nameserver_ip(DNSPacket* packet, char buf[]);
static int          get_nameserver(DNSPacket* packet, char buf[]);

/* -------------------------------------------
 * Utilities
 * ------------------------------------------- */
static size_t       encode_dns_name(char* domain_name, char* res);
static int          decode_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name);
static int          decode_compressed_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name);

#endif // INTERNALS_H
