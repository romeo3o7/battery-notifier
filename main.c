#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/poll.h>
#include <unistd.h>
#include <signal.h>
#include "notify.h"

volatile sig_atomic_t running = 1;

void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

int main () {
    char capacityBuff,statusBuff;
    _Bool highBatteryNotifed = 0, lowBatteryNotifed = 0;

    int powerFD =  open("/sys/class/power_supply/BAT1/capacity" ,O_RDONLY);
    int statusFD = open("/sys/class/power_supply/BAT1/status"   ,O_RDONLY);

    if (powerFD == -1) return 1;
    if (statusFD == -1) { close(powerFD); return 1; }

    struct pollfd pfd = {powerFD , POLLPRI | POLLERR , 0};

    if ( notifyInit() != 0) goto fdClean;

    struct sigaction sa = {0};
    sa.sa_handler = &handle_signal;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

       while (running) {
        int percentage = 0;

        lseek(powerFD  ,0, SEEK_SET);
        lseek(statusFD ,0, SEEK_SET);

        while (1) {
            if ( read(powerFD, &capacityBuff, sizeof capacityBuff) != 1 ) goto readErrorMessage;

            if (capacityBuff != '\n') {
            percentage = percentage * 10 + ( capacityBuff - '0');
            } else break;
        }

        if ( read(statusFD , &statusBuff, sizeof statusBuff ) != 1 ) goto readErrorMessage;

        switch (statusBuff) {
            case'C':
            if (percentage >= 78 && !highBatteryNotifed) {
                highBatteryNotifed = 1;
                if ( sendMessage("Battery charged, Unplug now! or else suffer") != 0) goto notifyClean;
            }
            break;

            default:
            if (percentage <= 25 && !lowBatteryNotifed) {
                lowBatteryNotifed = 1;
                if ( sendMessage("quick.. ur machine is dying.. its running out of energy, supply it with dc.. what are you waiting for?") != 0) goto notifyClean;
            }
            break;
        }

        if (percentage < 75) highBatteryNotifed = 0;
        if (percentage > 25) lowBatteryNotifed = 0;

        int pollResult = poll(&pfd, 1, -1);
        if (pollResult == -1 && errno == EINTR ) goto interrupted;
    }

    readErrorMessage:
    fprintf(stderr,"failed to read battery sysfs information\n");
    notifyClean:
    notifyUninit();
    fdClean:
    close(powerFD); close(statusFD);
    return 1;

    interrupted:
    notifyUninit();
    close(powerFD); close(statusFD);
    return 0;
}
