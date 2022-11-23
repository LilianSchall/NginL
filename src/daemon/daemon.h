#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "config/config.h"
#include "thread_pool/thread_pool.h"

#define COEFF_THREADS 8

void run_server(struct config *config);
int start_daemon(struct config *config);
void restart_daemon(struct config *config);
void reload_daemon(struct config *config);
void stop_daemon(struct config *config);

#endif /* !DAEMON_H */
