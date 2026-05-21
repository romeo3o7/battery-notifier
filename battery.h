#ifndef BATTERY_H
#define BATTERY_H

#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>
#include <signal.h>
#include "notify.h"
#include <signal.h>

typedef struct files {
    int powerFD;
    int statusFD;
}files;
#endif
