
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "dns.h"

// Function:    NewDNSHeader
// Description: Instantiate and populate DNSHeader struct.
// Parameters:
//              id (uint16_t):
//              flags (uint16_t):
//              num_questions (uint16_t):
// Returns:    Pointer to malloc'ed DNSHeader
static DNSHeader* NewDNSHeader(uint16_t id, uint16_t flags, uint16_t num_questions)
{
    DNSHeader *header     = calloc(1, sizeof(DNSHeader));
    header->id            = htons(id);
    header->flags         = htons(flags);
    header->num_questions = htons(num_questions);

    return header;
};

// Function:    NewDNSQuestion
// Description: Instantiate and populate DNSQuestion struct.
// Parameters:
//              encoded_name (char*): domain name as encoded by encode_dns_name()
//              type (uint16_t): e.g., TYPE_A = 1
//              class (uint16_t): e.g., CLASS_IN=1 (for internet)
// Returns:    Pointer to malloc'ed DNSQuestion
static DNSQuestion* NewDNSQuestion(char *encoded_name, uint16_t type, uint16_t class)
{
    DNSQuestion *question  = calloc(1, sizeof(DNSQuestion));
    question->name = malloc(strlen(encoded_name) + 1);
    question->type         = htons(type);
    question->class        = htons(class);
    strcpy(question->name, encoded_name);

    return question;
}

// Function:    NewDNSQuery
// Description: Instantiate and populate DNSQuery struct as sequence of bytes
//              comprising DNSHeader and DNSQuestion structs.
// Parameters:
//              domain_name (char*): domain name to resolve in presentation format
//              record_type: e.g., TYPE_A = 1
// Returns:    Pointer to malloc'ed DNSQuery
static DNSQuery *NewDNSQuery(char *domain_name, uint16_t record_type) 
{
    // create header
    int        id = 0; // TODO general random ID to pass to NewHeader()
    // int        RECURSION_DESIRED = 1 << 8; 
    DNSHeader* header = NewDNSHeader(id, 0, 1);

    // create question
    char         encoded_domain[100] = {0}; // buf for encoded domain
    size_t       encoded_len = encode_dns_name(domain_name, encoded_domain);
    DNSQuestion* question = NewDNSQuestion(encoded_domain, record_type, CLASS_IN);

    // build query
    DNSQuery* query = calloc(1, sizeof(DNSQuery));
    query->len = sizeof(*header) + 4 + strlen(question->name) + 1;
    query->s = malloc(query->len);
    
    header_to_bytes(header, query->s);
    question_to_bytes(question, query->s + sizeof(*header));
    
    destroy_DNSHeader(&header);
    destroy_DNSQuestion(&question);
    return query;
}

static DNSPacket *NewDNSPacket(const unsigned char *response_bytes) {
    DNSPacket   *packet      = calloc(1, sizeof(DNSPacket));
    int          bytes_read;

    // parse response
    DNSHeader   *header      = calloc(1, sizeof(DNSHeader));
    DNSQuestion *questions   = NULL;
    DNSRecord   *answers     = NULL;
    DNSRecord   *authorities = NULL;
    DNSRecord   *additionals = NULL;

    // parse header
    bytes_read = parse_header(response_bytes, header);
    
    // parse questions
    bytes_read = parse_questions(response_bytes, bytes_read, ntohs(header->num_questions), &questions);

    // parse answers
    bytes_read = parse_records(response_bytes, bytes_read, ntohs(header->num_answers), &answers);

    // parse authorities
    bytes_read = parse_records(response_bytes, bytes_read, ntohs(header->num_authorities), &authorities);

    // parse additionals
    bytes_read = parse_records(response_bytes, bytes_read, ntohs(header->num_additionals), &additionals);

    packet->header = header;
    packet->questions = questions;
    packet->answers = answers;
    packet->authorities = authorities;
    packet->additionals = additionals;

    return packet;
}

// Function:    encode_dns_name
// Description: Encodes DNS name in the following format:
//              www.example.com -> 3www7example3com
// Parameters:
//              domain_name (char*): domain name to encode
//              res (char*): buffer to hold result
// Returns:    Length of encoded name
static size_t encode_dns_name(char *domain_name, char *res)
{
    int dni = strlen(domain_name);
	// The result is shifted over by one, so set the null terminator.
	res[dni + 1] = 0;

	// Iterate in reverse, keeping track of the count of characters we see
	// before encountering a separator. Start the loop by decrementing dni
	// to skip the null terminator.
	int count = 0;
	for (--dni; dni >= 0; --dni) {
		if (domain_name[dni] == '.') {
			// Replace the separator with the count and reset.
			res[dni + 1] = count;
			count = 0;
			continue;
		}
		// Shift the non-separator character over one.
		res[dni + 1] = domain_name[dni];
		count++;
	}
	// Set the last count.
	res[0] = count;

    return dni + 1;
}

// Function:    header_to_bytes
// Description: Casts DNSHeader struct to char* and copies bytes to output buffer
//              using memcpy
// Parameters:
//              header (DNSHeader*): pointer to DNSHeader to encode
//              header_bytes (char*): buffer to hold encoded result
// Returns:    void
static void header_to_bytes(DNSHeader *header, char *header_bytes) {
    char *encoded = (char*)header;
    memcpy(header_bytes, encoded, sizeof *header);
}

// Function:    question_to_bytes
// Description: Copies bytes comprising DNSQuestion (including the encoded DNS name)
//              to output buffer 
// Parameters:
//              question (DNSQuestion*): pointer to DNSQuestion to encode
//              question_bytes (char*): buffer to hold encoded result
// Returns:    void
static void question_to_bytes(DNSQuestion *question, char *question_bytes) {
    char *p;

    strcpy(question_bytes, question->name);

    p = question_bytes + strlen(question->name) + 1;
    memcpy(p, &(question->type), sizeof question->type);

    p += sizeof question->type;
    memcpy(p, &(question->class), sizeof question->class);
}

// Function:    parse_header
// Description: Parses header information from response bytes and populates empty
//              DNSHeader struct
// Parameters:
//              response_bytes (char*): pointer to response bytes
//              header (DNSHeader*): pointer to empty DNSHeader struct for result
// Returns:    Count of number of bytes of response consumed from header parsing
static size_t parse_header(const unsigned char* response_bytes, DNSHeader *header)
{
    memcpy(header, response_bytes, sizeof(*header));
    return 12; 
}

// Function:    parse_question
// Description: Parse question information from response bytes beginning from byte
//              specified by `bytes_in`, and populates empty DNSQuestion struct
// Parameters:
//             question (DNSQuestion*): pointer to empty DNSQuestion struct for result
//             response_bytes (char*): pointer to response bytes
//             bytes_in (int): value representing number of bytes already consumed
// Returns:    Count of number of bytes of response consumed from parsing
static int parse_question(const unsigned char* response_bytes, int bytes_in, DNSQuestion *question)
{
    char decoded_name[MAX_BUFFER_SIZE] = {0};

    bytes_in = decode_name(response_bytes, bytes_in, decoded_name);

    size_t len = strlen(decoded_name) + 1;
    question->name = (char *)malloc(len);

    strlcpy(question->name, decoded_name, len);

    memcpy((void*)question + sizeof(question->name), response_bytes + bytes_in, 4);

    return bytes_in + 4;
}

/* num_records should be converted newtwork to host by caller */
static int parse_records(const unsigned char *response_bytes, int bytes_read, int num_records, DNSRecord **head)
{
    *head = NULL;
    DNSRecord *tail = NULL;
    for (int i=0; i < num_records; ++i) {

        // malloc space for new record
        DNSRecord   *cur_record = calloc(1, sizeof(*tail));  // init record->next = NULL                                                                     
        // parse current record
        bytes_read = parse_record(response_bytes, bytes_read, cur_record);     // parse record

        // append cur record to end of records linked list
        if (*head == NULL) {
            *head = tail = cur_record;
        } else {
            tail->next = cur_record;
            tail = tail->next;
        }
    }
    return bytes_read;
}

static int parse_questions(const unsigned char *response_bytes, int bytes_read, int num_questions, DNSQuestion **head)
{
    *head = NULL;
    DNSQuestion *tail = NULL;
    for (int i=0; i < num_questions; ++i) {
        // malloc space for new question
        DNSQuestion   *cur_question = calloc(1, sizeof(*tail));  // init question->next = NULL                                                                     
        // parse current question
        bytes_read = parse_question(response_bytes, bytes_read, cur_question);     // parse question

        // append cur question to end of questions linked list
        if (*head == NULL) {
            *head = tail = cur_question;
        } else {
            tail->next = cur_question;
            tail = tail->next;
        }
    }
    return bytes_read;
}

// Function:    parse_record
// Description: Parse DNS record information from response bytes beginning from byte
//              specified by `bytes_in`, and populates empt DNSRecord struct
// Parameters:
//             record (DNSRecord*): pointer to empty DNSRecord struct for result
//             response_bytes (char*): pointer to response bytes
//             bytes_in (int): value representing number of bytes already consumed
// Returns:    Count of number of bytes of response consumed from parsing
static int parse_record(const unsigned char* response_bytes, int bytes_in, DNSRecord *record)
{
    char decoded_name[MAX_BUFFER_SIZE] = {0};
    char decoded_data[MAX_BUFFER_SIZE] = {0};



    bytes_in = decode_name(response_bytes, bytes_in, decoded_name);

    
    size_t decoded_len = strlen(decoded_name) + 1; // len + null terminator
    record->name = (char *)malloc(decoded_len);
    strlcpy(record->name, decoded_name, decoded_len);

    memcpy((void*)record + sizeof(record->name), response_bytes + bytes_in, 10);
    bytes_in += 10;

    int      len = ntohs(record->bytes_len);
    uint8_t *data_bytes = malloc(len);

    // TODO: we are going to store data_bytes in human readable format
    // 2.123.23.14 for ip addresses and www.example.com for domains
    //
    // when we get here, we are at the payload portion of the response_bytes.
    //
    // if we're in an a record, convert that to presentation and store human readable ip address at data
    //    53/f3/d3/04 -> 123.23.24.13
    //
    // if we're in an NS record, we need to decode that and store decoded at data
    //    03/www/07./example/03/com -> www.example.com
    //
    // either way, do the same process above: make a buffer, pass into a function to parse ip address or decode name,
    // and then malloc space from there based on string length and return pointer to that char*
    //
    // question: should record->bytes_len change to length of human readable versions above?
    //
    // consider revising struct to include original data length and data bytes AND parsed/decoded data length and data bytes
    //
    // and the reason we're doing this again is because when we recursively search for an NS record domain, we need to
    // pass in the human-readable domain name, and in the event that is compressed, we need to decode it first
    //
    // switch(record->type) {
    //     case TYPE_A:    
    //                     break;

    //     case TYPE_NS:   
    //                     break;

    //     default:        
    // }

    memcpy(data_bytes, response_bytes + bytes_in, len);
    record->data_bytes = data_bytes;
    uint32_t   ipv4 = 0;
    switch (ntohs(record->type))
    {
        case TYPE_A:
            // char     ip[INET_ADDRSTRLEN];
            // Build IPv4 in Host Byte Order (why are the IP address bytes in host byte order?)
            for (int i = 0; i < ntohs(record->bytes_len); i++) {
                ipv4 <<= 8;
                ipv4 |= record->data_bytes[i];
            }

            ipv4 = htonl(ipv4);
            inet_ntop(AF_INET, &ipv4, decoded_data, MAX_BUFFER_SIZE);
            break;
        case TYPE_NS:
            decode_name(response_bytes, bytes_in, decoded_data);
            break;
        default:

            //this is a palce holder. we don't yet know how to parse non TYPE_A or TYPE NS
            //decode_name(response_bytes, bytes_in, decoded_data);
            break;
    }

    record->data_len = strlen(decoded_data) + 1; // len + null terminator TODO: consider not adding 1
    //printf("record->data_len = %zu\n", record->data_len); // should be 1 on last time around
    record->data = (char *)malloc(record->data_len);
    strlcpy(record->data, decoded_data, record->data_len);

    return bytes_in + len;
}


// Function:    decode_name
// Description: Decodes domain name from response bytes from encoded format or
//              compressed format.
// Parameters:
//             decoded_name (char*): buffer for decoded result
//             response_bytes (char*): pointer to response bytes
//             bytes_in (int): value representing number of bytes already consumed
// Returns:    Count of number of bytes of response consumed from parsing
// Piya points out that funciton has become impossible to reason about. We should refactor it.
static int decode_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name)
{
    int name_bytes;
    int len = response_bytes[bytes_in];
    int p;

    // look for compression flag
    if ((0xc0 & response_bytes[bytes_in]) == 0xc0){
        return decode_compressed_name(response_bytes, bytes_in, decoded_name);
    }
    // TODO: consider splitting this into its own function
    //       maybe call it "int split_domain"
    //ex: 03www07example03com => www.example.come
    //ex: 02e.COMPRESSIONFLAG => decode_compressed_name(response_bytes, bytes_in, e.)
    //NOTE: we just refactored p = 0 to start with the length of decoded_name because 
    //decode_compressed_name corecursivly calls decode_name and therefore setting p to 0 overwrites decoded_name
    
    //Zach says the problem is that this loop assumes that the first byte is a length byte and then discards it
    //we should rewrite the logic here to account for the first byte and then we can get rid of the redundant check for compression flags
    for(++bytes_in, p = strlen(decoded_name); response_bytes[bytes_in] != '\0' && p < MAX_BUFFER_SIZE-1; ++bytes_in, ++p){
        // this addresses the address boundry error, but decrementing bytes_in errases some of dcoded_name
        // if ((0xc0 & response_bytes[--bytes_in]) == 0xc0){
        //     return decode_compressed_name(response_bytes, bytes_in, decoded_name);
        // }
        if (len == 0){
            len = response_bytes[bytes_in];
            decoded_name[p] = '.';
        } else {
            decoded_name[p] = response_bytes[bytes_in];
            len--;
        }
        if ((0xc0 & response_bytes[bytes_in]) == 0xc0){
            return decode_compressed_name(response_bytes, bytes_in, decoded_name);
        }
    }

    return bytes_in + 1;
}

// Function:    decode_compressed_name
// Description: Decodes domain name from response bytes in case that it is in 
//              compressed format (most significant two bits are `11`)
// Parameters:
//             decoded_name (char*): buffer for decoded result
//             response_bytes (char*): pointer to response bytes
//             bytes_in (int): value representing number of bytes already consumed
// Returns:    Count of number of bytes of response consumed from parsing
static int decode_compressed_name(const unsigned char* response_bytes, int bytes_in, char *decoded_name)
{
    uint16_t pointer;

    pointer = (0xc0 ^ response_bytes[bytes_in]);
    pointer <<= 8;
    pointer |= response_bytes[bytes_in + 1];


    decode_name(response_bytes, pointer, decoded_name);

    return bytes_in + 2;
}

static DNSPacket *send_query(char *addr, char *domain, uint16_t type) {
    /* send request */
    int                     sockfd;
    struct sockaddr_in      dest;
    unsigned char                    buf[512];
    struct sockaddr_storage from;
    socklen_t               fromlen;

    dest.sin_port = htons(53);
    dest.sin_family = AF_INET;
    inet_pton(dest.sin_family, addr, &(dest.sin_addr));

    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // create query
    DNSQuery *query = NewDNSQuery(domain, type);
    
    // send query
    ssize_t bytes_sent = sendto(sockfd, query->s, query->len, 0, (struct sockaddr *)&dest, sizeof dest);
    if (bytes_sent == -1)
        perror("sendto");


    // print query bytes
    printf("%ld bytes sent: \n", bytes_sent);
    for (int i=0; i<query->len; ++i) {
        printf("%x/", (char)query->s[i]);
    }
    printf("\n\n");

    // receive result
    ssize_t bytes_received = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&from, &fromlen);
    if (bytes_received == -1)
        perror("recvfrom");

    // print result bytes
    printf("%zd bytes received: \n", bytes_received);
    for (int i=0; i<bytes_received; ++i) {
        printf("%x/", (unsigned char)buf[i]);
    }
    printf("\n\n");

    destroy_DNSQuery(&query);
    
    // close socket
    close(sockfd);

    return NewDNSPacket(buf);
}

static int get_answer(DNSPacket* packet, char buf[]) {
    // return the first A record in the Answer section
    DNSRecord* cur_answer = packet->answers;

    while (cur_answer) {
        if (ntohs(cur_answer->type) == TYPE_A) {
            strlcpy(buf, cur_answer->data, cur_answer->data_len);
            return 0;
        }
        cur_answer = cur_answer->next;
    }

    return 1;
}

static int get_nameserver_ip(DNSPacket* packet, char buf[]) {
    // return the first A record in the Additional section
    DNSRecord* cur_additional = packet->additionals;

    while (cur_additional) {
        if (ntohs(cur_additional->type) == TYPE_A) {
            strlcpy(buf, cur_additional->data, cur_additional->data_len);
            return 0;
        }
        cur_additional = cur_additional->next;
    }

    return 1;
    
}

static int get_nameserver(DNSPacket* packet, char buf[]) {
    // return the first A record in the Additional section
    DNSRecord* cur_authority = packet->authorities;

    while (cur_authority) {
        if (ntohs(cur_authority->type) == TYPE_NS) {
            strlcpy(buf, cur_authority->data, cur_authority->data_len);
            return 0;
        }
        cur_authority = cur_authority->next;
    }

    return 1;
}

void resolve(char* domain_name, uint16_t record_type, char answer[]) {
    DNSPacket* packet = NULL;
    char nameserver[128] = {0};

    strcpy(nameserver, "198.41.0.4");
    
    char ns_domain[128] = {0};

    while (1) {
        printf("Querying %s for %s.\n", nameserver, domain_name);
        packet = send_query(nameserver, domain_name, record_type);
        display_DNSPacket(packet);
        if (get_answer(packet, answer) == 0) {
            destroy_DNSPacket(&packet);
            break;
        } else if (get_nameserver_ip(packet, nameserver) == 0) {
            destroy_DNSPacket(&packet);
        } else if (get_nameserver(packet, ns_domain) == 0) {
            destroy_DNSPacket(&packet);
            resolve(ns_domain, TYPE_A, nameserver);
        } else {
            destroy_DNSPacket(&packet);
            printf("Error resolving %s.\n", domain_name);
            break;
        }
    }

}




/* DISPLAY FUNCTIONS */
static void display_DNSHeader(DNSHeader *header)
{
    printf("DNSHeader(");
    printf("id=%d, ", ntohs(header->id));
    printf("flags=%d, ", ntohs(header->flags));
    printf("num_questions=%d, ", ntohs(header->num_questions));
    printf("num_answers=%d, ", ntohs(header->num_answers));
    printf("num_authorities=%d, ", ntohs(header->num_authorities));
    printf("num_additionals=%d", ntohs(header->num_additionals));
    printf(")\n\n");

}

static void display_DNSQuestion(DNSQuestion *question)
{
    DNSQuestion *cur_question = question;
    while (cur_question != NULL) {

        printf("DNSQuestion(");
        printf("name=%s, ", cur_question->name);
        printf("type=%d, ", ntohs(cur_question->type));
        printf("class=%d", ntohs(cur_question->class));
        printf(")\n");
        
        cur_question = cur_question->next;
    }
    printf("\n");
}

static void display_DNSRecord(DNSRecord *record)
{
    DNSRecord *cur_record = record;
    while (cur_record != NULL) {

        // uint32_t ipv4 = 0;
        // char     ip[INET_ADDRSTRLEN];

        // // Build IPv4 in Host Byte Order (why are the IP address bytes in host byte order?)
        // for (int i = 0; i < ntohs(cur_record->bytes_len); i++) {
        //     ipv4 <<= 8;
        //     ipv4 |= cur_record->data_bytes[i];
        // }

        // // Convert IPv4 address to Network Byte Order
        // ipv4 = htonl(ipv4);
        // inet_ntop(AF_INET, &ipv4, ip, INET_ADDRSTRLEN);

        

        printf("DNSRecord(");
        printf("name=%s, ", cur_record->name);
        printf("type=%d, ", ntohs(cur_record->type));
        printf("class=%d, ", ntohs(cur_record->class));
        printf("ttl=%d, ", ntohl(cur_record->ttl));
        printf("bytes_len=%d, ", ntohs(cur_record->bytes_len));
        printf("data_bytes=");
        for (int i = 0; i < ntohs(cur_record->bytes_len); i++)
        {
            printf("%x ", cur_record->data_bytes[i]); 
        }
        printf(", ");
        printf("data_len=%zu, ", cur_record->data_len);
        printf("data=%s", cur_record->data);
        printf(")\n");
        
        cur_record = cur_record->next;
    }
    printf("\n");
}

static void display_DNSPacket(DNSPacket *packet)
{
    printf("HEADER\n");
    display_DNSHeader(packet->header);
    printf("QUESTIONS\n");
    display_DNSQuestion(packet->questions);
    printf("ANSWERS\n");
    display_DNSRecord(packet->answers);
    printf("AUTHORITIES\n");
    display_DNSRecord(packet->authorities);
    printf("ADDITIONALS\n");
    display_DNSRecord(packet->additionals);
}

static int destroy_DNSPacket(DNSPacket **packet) {
    destroy_DNSHeader(&((*packet)->header));
    destroy_DNSQuestion(&((*packet)->questions));
    destroy_DNSRecord(&((*packet)->answers));
    destroy_DNSRecord(&((*packet)->authorities));
    destroy_DNSRecord(&((*packet)->additionals));
    free(*packet);
    (*packet) = NULL;
    return 0;
}

static int destroy_DNSHeader(DNSHeader **header) {
    free(*header);
    (*header) = NULL;
    return 0;
}

static int destroy_DNSQuestion(DNSQuestion **question) {
    // Am I correctly destroying the *next link? Is there a mem leak here?
    DNSQuestion *cur  = *question;
    DNSQuestion *next;
    
    while (cur != NULL) {
        next = cur->next; // hold reference to next, since current node will be freed

        free(cur->name);
        cur->name = NULL;

        free(cur->next);
       	cur->next = NULL;
        
        free(cur);
        cur = next;
    }

    *question = NULL;

    return 0;
}


static int destroy_DNSQuery(DNSQuery **query) {
    free((*query)->s);
    (*query)->s = NULL;

    free(*query);
    *query = NULL;
    return 0;
}

static int destroy_DNSRecord(DNSRecord **record) {
    // Am I correctly destroying the *next link? Is there a mem leak here?
    DNSRecord *cur  = *record;
    DNSRecord *next;

    while (cur != NULL) {
        next = cur->next; // hold reference to next, since current node will be freed

        free(cur->name);
        cur->name = NULL;

        free(cur->data_bytes);
        cur->data_bytes = NULL;

        free(cur->data);
        cur->data = NULL;

        cur->next = NULL;

        free(cur);
        cur = next;
    }

    *record = NULL;
    return 0;
}
