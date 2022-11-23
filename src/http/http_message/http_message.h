#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "commons/commons.h"
#include "hash_map/hash_map.h"
#include "http/delimiter.h"
#include "http/http_request/http_request.h"
#include "http/http_status/http_status.h"
#include "log/log.h"

#define STD_NB_HEADERS 10
#define BODY_BUFFER_SIZE 4096
#define NB_TOKENS_HEADER 2

enum line_type
{
    REQUEST,
    STATUS,
};

union http_line
{
    struct http_request *request;
    struct http_status *status;
};

struct start_line
{
    enum line_type type;
    union http_line line;
};

// this struct is used to contain a parsed http_message
// - start_line is the first_line of the message
// - header hashmap contain every header as the following structure
//      key : value
// - body is the content of the message
struct http_message
{
    struct start_line start_line;
    struct hash_map *headers;
    // mandatory whitespace
    char *body;
    int file_to_serve;
    int file_size;
};

union http_line create_http_line(struct http_request *request,
                                 struct http_status *status);

struct start_line create_start_line(union http_line line, enum line_type type);

void free_start_line(struct start_line start_line);

void print_message(struct http_message *message);

struct http_message *create_http_message(struct start_line start_line,
                                         struct hash_map *headers, char *body);

void free_http_message(void *data);

// this function parse a message from an incoming stream
// stream is a derive of a socket file descriptor
// it needs to be open with fdopen
// the first line will always be a request
struct http_message *parse_message(FILE *stream, enum status_code *code);

struct http_request *get_request(struct http_message *message);
struct http_status *get_status(struct http_message *message);

#endif /* !HTTP_MESSAGE_H */
