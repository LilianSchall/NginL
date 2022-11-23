#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include "config/vhost_config/vhost_config.h"
#include "http/http_request/http_request.h"
#include "http/http_status/http_status.h"

bool init_log(char *log_file_path, bool log);

void print_log(const char *format, ...);
void print_request(struct http_request *req, struct http_status *status,
                   const struct vhost_config *vhc, char *ip);
void print_response(struct http_request *req, struct http_status *status,
                    const struct vhost_config *vhc, char *ip);

void close_log(void);

#endif // LOG_H
