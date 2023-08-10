#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <string.h>
#include "request.h"

DNSHeader* NewDNSHeader(u_int16_t id, u_int16_t flags, u_int16_t num_questions)
{
    DNSHeader *header     = calloc(1, sizeof(DNSHeader));
    header->id            = htons(id);
    header->flags         = htons(flags);
    header->num_questions = htons(num_questions);

    return header;
};

DNSQuestion* NewDNSQuestion(char *encoded_name, int type, int class)
{
    DNSQuestion *question  = calloc(1, sizeof(DNSQuestion));
    question->encoded_name = malloc(strlen(encoded_name) + 1);
    question->type         = htons(type);
    question->class        = htons(class);
    strcpy(question->encoded_name, encoded_name);

    return question;
}

Query NewDNSQuery(char *domain_name, int record_type)
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
    size_t query_len    = sizeof(*header) + 4 + strlen(question->encoded_name) + 1;
    char*  query_string = malloc(query_len);
    Query  full_query   = { .len = query_len, .s = query_string };
    header_to_bytes(header, full_query.s);
    question_to_bytes(question, full_query.s + sizeof(*header));

    return full_query;
}

/* original version */
/*
size_t encode_dns_name(char *domain_name, char *res)
{
    char* token;
    char* p;
    char copy[100];
    int j;

    j  = 0;
    p = copy;

    memset(copy, '\0', 100);
    strcpy(copy, domain_name);
    
    while ((token = strsep(&p, ".")))
    {
        res[j++] = strlen(token);
        strcpy(res + j, token);
        j += strlen(token);
    }
    res[j] = '\0';
    return j + 1;
}
*/

/* miccah version */
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

void header_to_bytes(DNSHeader *header, char *header_bytes) {
    char *encoded = (char*)header;
    memcpy(header_bytes, encoded, sizeof *header);
}

void question_to_bytes(DNSQuestion *question, char *question_bytes) {
    char *p;

    strcpy(question_bytes, question->encoded_name);

    p = question_bytes + strlen(question->encoded_name) + 1;
    memcpy(p, &(question->type), sizeof question->type);

    p += sizeof question->type;
    memcpy(p, &(question->class), sizeof question->class);
}

