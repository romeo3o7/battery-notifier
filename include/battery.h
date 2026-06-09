#ifndef BATTERY_H
#define BATTERY_H
#include <stdbool.h>
typedef struct  { 
    bool hpn;
    bool lpn;
    double per;
}status;
int logic (status * s);
#endif
