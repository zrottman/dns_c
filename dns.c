#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "dns.h"

// Function:    NewDNSHeader
// Description: Instantiate and populate DNSHeader struct.
// Parameters:
//              id (uint16_t):
//              flags (uint16_t):
//              num_questions (uint16_t):
// Returns:    Pointer to malloc'ed DNSHeader
DNSHeader* NewDNSHeader(uint16_t id, uint16_t flags, uint16_t num_questions)
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
DNSQuestion* NewDNSQuestion(char *encoded_name, uint16_t type, uint16_t class)
{
    DNSQuestion *question  = calloc(1, sizeof(DNSQuestion));
    question->encoded_name = malloc(strlen(encoded_name) + 1);
    question->type         = htons(type);
    question->class        = htons(class);
    strcpy(question->encoded_name, encoded_name);

    return question;
}

// Function:    NewDNSQuery
// Description: Instantiate and populate DNSQuery struct as sequence of bytes
//              comprising DNSHeader and DNSQuestion structs.
// Parameters:
//              domain_name (char*): domain name to resolve in presentation format
//              record_type: e.g., TYPE_A = 1
// Returns:    Pointer to malloc'ed DNSQuery
DNSQuery *NewDNSQuery(char *domain_name, uint16_t record_type) 
{
    // create header
    int        id = 0; // TODO general random ID to pass to NewHeader()
    int        RECURSION_DESIRED = 1 << 8; 
    DNSHeader* header = NewDNSHeader(id, RECURSION_DESIRED, 1);

    // create question
    char         encoded_domain[100] = {0}; // buf for encoded domain
    size_t       encoded_len = encode_dns_name(domain_name, encoded_domain);
    DNSQuestion* question = NewDNSQuestion(encoded_domain, record_type, CLASS_IN);

    // build query
    DNSQuery* query = calloc(1, sizeof(DNSQuery));
    query->len = sizeof(*header) + 4 + strlen(question->encoded_name) + 1;
    query->s = malloc(query->len);
    
    header_to_bytes(header, query->s);
    question_to_bytes(question, query->s + sizeof(*header));

    return query;
}

DNSPacket *NewDNSPacket(char *response_bytes) {
    DNSPacket *packet = calloc(1, sizeof(DNSPacket));

    // parse response
    DNSHeader   *header   = calloc(1, sizeof(DNSHeader));
    DNSQuestion *questions_head   = NULL; // replace with: questions
    DNSQuestion *questions_tail   = NULL;
    DNSRecord   *answers_head     = NULL; // resplaace with: answers
    DNSRecord   *answers_tail     = NULL;
    /*
    DNSRecord   *authorities_head = NULL;
    DNSRecord   *authorities_tail = NULL;
    DNSRecord   *additionals_head = NULL;
    DNSRecord   *additionals_tail = NULL;
    */
    //DNSRecord   *record   = calloc(1, sizeof(DNSRecord));  // init record->next = NULL
    int          bytes_read;

    // parse header
    bytes_read = parse_header(response_bytes, header);

    // TODO:
    // Generalize below parsing loops (but need to check for type -- DNSQuestion vs DNS Record):
    //
    // bytes_in = parse(ll_head, num loops, buf, bytes_read, response_type)
    //
    // enum response_type { QUESTION, ANSWER, AUTHORITY, ADDITIONAL };
    //
    // ** include a switch statement in parse function to determine correct struct to use based on response type
    // ** allow generic parse function to handle tail management --> we don't need access to tail outside that func
    // 
    // bytes_in = parse(questions, ntohs(header->num_questions, buf, bytes_read, QUESTION)
    // bytes_in = parse(answers, ntohs(header->num_answers, buf, bytes_read, ANSWER)
    // bytes_in = parse(authorities, ntohs(header->num_authorities, buf, bytes_read, AUTHORITY)
    // bytes_in = parse(additionals, ntohs(header->num_additionals, buf, bytes_read, ADDITIONAL)
    
    // parse questions
    for (int i=0; i < ntohs(header->num_questions); ++i) {
        //printf("questions loop: %d\n", i);
        
        // malloc space for new question
        DNSQuestion *cur_question = calloc(1, sizeof(DNSQuestion)); // init question->next = NULL
                                                                     
        // parse current question
        bytes_read = parse_question(response_bytes, bytes_read, cur_question); // parse question

        // append cur question to end of questions linked list
        if (questions_head == NULL) {
            questions_head = questions_tail = cur_question;
        } else {
            questions_tail->next = cur_question;
            questions_tail = questions_tail->next;
        }
    }

    // parse answers
    for (int i=0; i < ntohs(header->num_answers); ++i) {
        
        // malloc space for new answer
        DNSRecord   *cur_answer = calloc(1, sizeof(DNSRecord));  // init record->next = NULL
                                                                     
        // parse current question
        bytes_read = parse_record(response_bytes, bytes_read, cur_answer);     // parse answer

        // append cur question to end of questions linked list
        if (answers_head == NULL) {
            answers_head = answers_tail = cur_answer;
        } else {
            answers_tail->next = cur_answer;
            answers_tail = answers_tail->next;
        }
    }

    // parse additionals
    
    // parse authorities

    // display structs
    display_DNSHeader(header);
    display_DNSQuestion(questions_head);
    display_DNSRecord(answers_head);
    //display_DNSRecord(additionals_head);
    //display_DNSRecord(authorities_head);

    return packet;
}

// Function:    encode_dns_name
// Description: Encodes DNS name in the following format:
//              www.example.com -> 3www7example3com
// Parameters:
//              domain_name (char*): domain name to encode
//              res (char*): buffer to hold result
// Returns:    Length of encoded name
size_t encode_dns_name(char *domain_name, char *res)
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
void header_to_bytes(DNSHeader *header, char *header_bytes) {
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
void question_to_bytes(DNSQuestion *question, char *question_bytes) {
    char *p;

    strcpy(question_bytes, question->encoded_name);

    p = question_bytes + strlen(question->encoded_name) + 1;
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
size_t parse_header(char* response_bytes, DNSHeader *header)
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
int parse_question(char* response_bytes, int bytes_in, DNSQuestion *question)
{
    char *decoded_name = malloc(strlen(response_bytes + bytes_in)); // look for the NULL byte

    bytes_in = decode_name(response_bytes, bytes_in, decoded_name);

    question->encoded_name = decoded_name;

    memcpy((void*)question + sizeof(question->encoded_name), response_bytes + bytes_in, 4);

    return bytes_in + 4;
}

// Function:    parse_record
// Description: Parse DNS record information from response bytes beginning from byte
//              specified by `bytes_in`, and populates empt DNSRecord struct
// Parameters:
//             record (DNSRecord*): pointer to empty DNSRecord struct for result
//             response_bytes (char*): pointer to response bytes
//             bytes_in (int): value representing number of bytes already consumed
// Returns:    Count of number of bytes of response consumed from parsing
int parse_record(char* response_bytes, int bytes_in, DNSRecord *record)
{
     char    *decoded_name = malloc(strlen(response_bytes + bytes_in)); // look for the NULL byte
                                                                       
    bytes_in = decode_name(response_bytes, bytes_in, decoded_name);

    record->name = decoded_name;

    memcpy((void*)record + sizeof(record->name), response_bytes + bytes_in, 10);
    bytes_in += 10;

    int      len = ntohs(record->data_len);
    uint8_t *data_bytes = malloc(len);

    memcpy(data_bytes, response_bytes + bytes_in, len);
    record->data_bytes = data_bytes;

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
int decode_name(char* response_bytes, int bytes_in, char *decoded_name)
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
    for(++bytes_in, p = 0; response_bytes[bytes_in] != '\0'; ++bytes_in, ++p){
        if (len == 0){
            len = response_bytes[bytes_in];
            decoded_name[p] = '.';
        } else {
            decoded_name[p] = response_bytes[bytes_in];
            len--;
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
int decode_compressed_name(char* response_bytes, int bytes_in, char *decoded_name)
{
    uint16_t pointer;

    pointer = (0xc0 ^ response_bytes[bytes_in]);
    pointer <<= 8;
    pointer |= response_bytes[bytes_in + 1];

    decode_name(response_bytes, pointer, decoded_name);

    return bytes_in + 2;
}
/* DISPLAY FUNCTIONS */
void display_DNSHeader(DNSHeader *header)
{
    printf("RESPONSE: HEADER\n");
    printf("header->id: %d\n", ntohs(header->id));
    printf("header->flags: %d\n", ntohs(header->flags));
    printf("header->num_questions: %d\n", ntohs(header->num_questions));
    printf("header->num_answers: %d\n", ntohs(header->num_answers));
    printf("header->num_authorities: %d\n", ntohs(header->num_authorities));
    printf("header->num_additionals: %d\n", ntohs(header->num_additionals));
    printf("\n\n");

}

void display_DNSQuestion(DNSQuestion *question)
{
    DNSQuestion *cur_question = question;
    while (cur_question != NULL) {

        printf("RESPONSE: QUESTION\n");
        printf("cur_question->encoded_name: %s\n", cur_question->encoded_name);
        printf("cur_question->type: %d\n", ntohs(cur_question->type));
        printf("cur_question->class: %d\n", ntohs(cur_question->class));
        printf("\n\n");
        
        cur_question = cur_question->next;
    }
}

void display_DNSRecord(DNSRecord *record)
{
    DNSRecord *cur_record = record;
    while (cur_record != NULL) {

        printf("RESPONSE: RECORD\n");
        printf("cur_record->name: %s\n", cur_record->name);
        printf("cur_record->type: %d\n", ntohs(cur_record->type));
        printf("cur_record->class: %d\n", ntohs(cur_record->class));
        printf("cur_record->ttl %d\n", ntohl(cur_record->ttl));
        printf("cur_record->data_len: %d\n", ntohs(cur_record->data_len));
        printf("cur_record->data_bytes: ");

        uint32_t ipv4 = 0;
        char ip[INET_ADDRSTRLEN];
        
        // Build IPv4 in Host Byte Order (why are the IP address bytes in host byte order?)
        for (int i = 0; i < ntohs(cur_record->data_len); i++) {
            ipv4 <<= 8;
            ipv4 |= cur_record->data_bytes[i];
        }

        // Convert IPv4 address to Network Byte Order
        ipv4 = htonl(ipv4);
        inet_ntop(AF_INET, &ipv4, ip, INET_ADDRSTRLEN);

        printf("%s\n" , ip);
        printf("\n\n");
        cur_record = cur_record->next;
    }
}


