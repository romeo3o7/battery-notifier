#include <systemd/sd-bus.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>


int parsing(struct sd_bus_message *m, void *userdata, struct sd_bus_error *error)  {
        puts("signal");
        return 0;
    }
int main() {
    sd_bus *bus = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *m = NULL;

    sd_bus_open_system(&bus); // assigns me to the dbus
    
    int busFd = sd_bus_get_fd(bus);
    
    int epfd = epoll_create1(0);

    struct epoll_event ev = {
        .events = EPOLLIN ,
        .data.fd =busFd 
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD,busFd, &ev) < 0) return -1;
    void *userdata = NULL;
    sd_bus_match_signal(bus, 
                NULL, 
                "org.freedesktop.UPower", 
                "/org/freedesktop/UPower/devices/battery_BAT1", 
                "org.freedesktop.DBus.Properties", 
                "PropertiesChanged",
                parsing,
                &userdata);

     for(;;) {   
        epoll_wait(epfd, &ev, 1, -1);
        while (sd_bus_process(bus,NULL)> 0 );
     }
    cleanup:
    sd_bus_error_free(&error);
    sd_bus_unref(bus);
    if (epfd >= 0) close(epfd);

    return 0;
}
   
