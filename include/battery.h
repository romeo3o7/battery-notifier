#ifndef BATTERY_H
#define BATTERY_H

#include <errno.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>
#include "notify.h"

typedef struct files {
    int powerFD;
    int statusFD;
}files;

int cleanupLogic(char flag, files *fd);
 
#endif
