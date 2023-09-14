#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "request.h"

int main(int argc, char **argv)
{

    /* send request */
    int                     sockfd;
    struct sockaddr_in      dest;
    char                    buf[512];
    struct sockaddr_storage from;
    socklen_t               fromlen;

    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = 0x08080808; // 8.8.8.8
    dest.sin_family = AF_INET;


    // validate argc
    if (argc != 2) {
        printf("Expected usage: ./dns <domain name>\n");
        exit(1);
    }

    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // create query
    DNSQuery query = NewDNSQuery(argv[1], TYPE_A);
    
    // send query
    ssize_t bytes_sent = sendto(sockfd, query.s, query.len, 0, (struct sockaddr *)&dest, sizeof dest);
    if (bytes_sent == -1)
        perror("sendto");

    // print query bytes
    printf("%ld bytes sent: \n", bytes_sent);
    for (int i=0; i<query.len; ++i) {
        printf("%x/", (char)query.s[i]);
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

    // close socket
    close(sockfd);

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
    bytes_read = parse_header(buf, header);

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
        bytes_read = parse_question(cur_question, buf, bytes_read); // parse question

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
        bytes_read = parse_record(cur_answer, buf, bytes_read);     // parse answer

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

    return 0;
}
