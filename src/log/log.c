#include "log.h"

static FILE *log_file = NULL;

bool init_log(char *log_file_path, bool log)
{
    if (!log)
        return true;
    if (log_file_path)
        log_file = fopen(log_file_path, "w");
    else
        log_file = stdout;
    if (!log_file)
        return false;
    return true;
}

void print_log(const char *format, ...)
{
    if (!log_file)
        return;
    va_list ap;
    va_start(ap, format);
    vfprintf(log_file, format, ap);
    fflush(log_file);
}

void print_request(struct http_request *req, struct http_status *status,
                   const struct vhost_config *vhc, char *ip)
{
    if (!log_file)
        return;
    char date[4096] = { 0 };
    time_t current_time = time(NULL);
    struct tm *tm = gmtime(&current_time);
    strftime(date, 4096, "%a, %e %b %Y %H:%M:%S GMT", tm);
    char *method = NULL;

    if (status->code == BAD_REQUEST)
    {
        method = "Bad Request";
        fprintf(log_file, "%s [%s] received %s from %s\n", date,
                vhc->server_name, method, ip);
        fflush(log_file);
        return;
    }

    switch (req->method)
    {
    case GET:
        method = "GET";
        break;
    case HEAD:
        method = "HEAD";
        break;
    default:
        break;
    }
    fprintf(log_file, "%s [%s] received %s on '%s' from %s\n", date,
            vhc->server_name, method, req->target, ip);
    fflush(log_file);
}

void print_response(struct http_request *req, struct http_status *status,
                    const struct vhost_config *vhc, char *ip)
{
    if (!log_file)
        return;
    char date[4096] = { 0 };
    time_t current_time = time(NULL);
    struct tm *tm = gmtime(&current_time);
    strftime(date, 4096, "%a, %e %b %Y %H:%M:%S GMT", tm);
    char *method = NULL;

    if (status->code == BAD_REQUEST)
    {
        fprintf(log_file, "%s [%s] responding with %d to %s'\n", date,
                vhc->server_name, status->code, ip);

        fflush(log_file);
        return;
    }

    switch (req->method)
    {
    case GET:
        method = "GET";
        break;
    case HEAD:
        method = "HEAD";
        break;
    default:
        method = "UNKNOWN";
        break;
    }
    fprintf(log_file, "%s [%s] responding with %d to %s for %s on '%s'\n", date,
            vhc->server_name, status->code, ip, method, req->target);
    fflush(log_file);
}

void close_log(void)
{
    if (log_file)
    {
        fflush(log_file);
        fclose(log_file);
    }
}
