#ifndef REQUEST_H
#define REQUEST_H

#include <stdint.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "internals.h"

#define TYPE_A   1
#define TYPE_NS  2

void    resolve(char* domain_name, uint16_t record_type, char answer[]);

#endif // REQUEST_H
