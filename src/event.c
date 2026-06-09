#include "../include/battery.h"
#include "../include/notify.h"
#include <systemd/sd-bus.h>
#include <string.h>
status s = {
    .hpn = false,
    .lpn = false,
    .per = 0.0  
};

int parsing(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    (void) error; 
    (void) userdata;
    const char *iface;
    //we read to skip it
    sd_bus_message_read(m, "s", &iface);

    // enter the a{sv} array
    sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{sv}");

    while (sd_bus_message_enter_container(m, SD_BUS_TYPE_DICT_ENTRY, "sv") > 0) { // loop as much it returns
        const char *key;
        sd_bus_message_read(m, "s", &key);  // read the dictonary key

        if (strcmp(key, "Percentage") == 0) {
            // enter the variant continer 
            sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "d");
            sd_bus_message_read(m, "d", &s.per);
            sd_bus_message_exit_container(m);  // exit variant  
            logic(&s);
        } else {
            sd_bus_message_skip(m, "v");  // skip unknown variant 
        }

        sd_bus_message_exit_container(m);  // exit the current {sv} dictonary entry
    }

     sd_bus_message_exit_container(m);  // exit the a{sv} array
     return 0;
    }

int main() {
    sd_bus *bus = NULL;
    if ( sd_bus_open_system(&bus) < 0) return 1; // assigns me to the dbus
    sd_event *event = NULL;
    if ( sd_event_default(&event) < 0) {// creates a event object
        sd_bus_unref(bus);
        return 1; 
    }
    if ( sd_bus_attach_event(bus,event,0) < 0) goto cleanup; // attack the bus to the event 

    if ( notifyInit() < 0) goto cleanup;

    if ( sd_bus_match_signal(bus, // regsites me for signals and runs my parsing when signal is recived
                NULL, 
                "org.freedesktop.UPower", 
                "/org/freedesktop/UPower/devices/battery_BAT1", 
                "org.freedesktop.DBus.Properties", 
                "PropertiesChanged",
                parsing,
                NULL) < 0) goto cleanup;

    sd_event_loop(event); // event loop 
    
    cleanup:
    sd_bus_unref(bus);
    sd_event_unref(event);
    notifyUninit();

    return 0;
}
