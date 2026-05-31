#include "battery.h"

int main () {
    char capacityBuff,statusBuff;
    _Bool highBatteryNotifed = 0, lowBatteryNotifed = 0;

    files fds = {
    .powerFD =  open("/sys/class/power_supply/BAT1/capacity" ,O_RDONLY),
    .statusFD = open("/sys/class/power_supply/BAT1/status"   ,O_RDONLY)
    };

    if (fds.powerFD ==  -1 ||fds.statusFD == -1 ) return cleanupLogic('f', &fds);

    struct pollfd pfd = {fds.powerFD , POLLPRI | POLLERR , 0};  // wait for the fd changes

    if ( notifyInit() != 0) return cleanupLogic('f', &fds);

    int percentage = 0;

    while (1) {

        percentage = 0;
        lseek(fds.powerFD  ,0, SEEK_SET);
        lseek(fds.statusFD ,0, SEEK_SET);

        while (1) {
            if (read(fds.powerFD, &capacityBuff, sizeof capacityBuff) != 1 ) {
                return cleanupLogic('r' , &fds);
            }
            if (capacityBuff != '\n') {
                percentage = percentage * 10 + ( capacityBuff - '0');
            } else break;
        }

        if (read(fds.statusFD , &statusBuff, sizeof statusBuff ) != 1 ){
            return cleanupLogic('r', &fds);
        }

        switch (statusBuff) {

            case'C':
            if (percentage >= 78 && !highBatteryNotifed) {
                highBatteryNotifed = 1;
                if (sendMessage("Battery charged, Unplug now! or else suffer") != 0){
                    return cleanupLogic('n' , &fds);
                }
            } break;

            default:
            if (percentage <= 25 && !lowBatteryNotifed) {
                lowBatteryNotifed = 1;
                if (sendMessage("quick.. ur machine is dying.. supply it with dc.. what are you waiting for?") != 0){
                    return cleanupLogic('n' , &fds);
                }
            } break;
        }

        if (percentage < 75) highBatteryNotifed = 0;
        if (percentage > 25) lowBatteryNotifed = 0;

        int pollResult = poll(&pfd, 1, 60000); //60 seconds and check
        if (pollResult == -1){
            if (errno == EINTR ) break; // EINTR = signal occured return 0;
            return cleanupLogic('n' , &fds); // here i want to return 1 if poll() fails
        }
    }

    return cleanupLogic('i' , &fds);
}

int cleanupLogic(char flag , files *fds) {
    switch (flag) {
        case'r':  // read error
        case'n':  // notify error
        notifyUninit();
        if (fds->powerFD   != -1) close(fds->powerFD); // making sure the fd is -1 otherwise 
        if (fds-> statusFD != -1) close(fds->statusFD);
        return 1;

        case'f':  // fd error
        if (fds->powerFD   != -1) close(fds->powerFD);
        if (fds-> statusFD != -1) close(fds->statusFD);
        return 1;

        case'i':  // interupt which is not an error
        notifyUninit();
        if (fds->powerFD   != -1) close(fds->powerFD);
        if (fds-> statusFD != -1) close(fds->statusFD);
        return 0;

        default:
        return -1;
    }
}
