#define BUS
#include "../include/global.h"
int main() {
    sd_bus *bus = NULL;
    if ( sd_bus_open_system(&bus) < 0) return 1; // assigns me to the dbus
    sd_event *event = NULL;
    if ( sd_event_default(&event) < 0) {// creates a event object
        sd_bus_unref(bus);
        return 1; 
    }
    if ( sd_bus_attach_event(bus,event,0) < 0) goto cleanup; // attack the bus to the event 
    sd_bus_error error =   SD_BUS_ERROR_NULL;
    if ( notifyInit() < 0) goto cleanup;

     battery state = {
           .hpn = false,
           .lpn = false,
           .isCharging = 0,
           .per = 0.0
    };
    // get battery state  
    if ( sd_bus_get_property_trivial(bus,
            "org.freedesktop.UPower",
            "/org/freedesktop/UPower/devices/battery_BAT1",
            "org.freedesktop.UPower.Device",
            "State",
            &error,
            'u',
            &state.isCharging) < 0) goto cleanup;
    // get percentage  
    if ( sd_bus_get_property_trivial(bus,
            "org.freedesktop.UPower",
            "/org/freedesktop/UPower/devices/battery_BAT1",
            "org.freedesktop.UPower.Device",
            "Percentage",
            &error,
            'd',
            &state.per) < 0) goto cleanup;

    if ( sd_bus_match_signal(bus, // regsites me for signals and runs my parsing when signal is recived
                NULL, 
                "org.freedesktop.UPower", 
                "/org/freedesktop/UPower/devices/battery_BAT1", 
                "org.freedesktop.DBus.Properties", 
                "PropertiesChanged",
                parsing,
                &state) < 0) goto cleanup;

    sd_event_loop(event); // event loop 
    
    cleanup:
    sd_event_unref(event);
    sd_bus_unref(bus);
    notifyUninit();

    return 0;
}
