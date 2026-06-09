#include "../include/notify.h"
#include "../include/battery.h"
int logic(status *s) {
    if (s->per >= 78 && !s->hpn) {
        sendMessage("quick the battery is about to explode! unplug");
        s->hpn = true;
    }
    if (s->per <= 25 && !s->lpn) {
        sendMessage("battery is thrisry");
        s->lpn = true;
    }

    if (s->per < 77) s->hpn = false;
    if (s->per > 26 ) s->lpn = false;

    return 0;
}
