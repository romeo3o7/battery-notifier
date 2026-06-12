#include "../include/global.h"

int logic(battery *state) {
    // 1 = charging
    // 2 = discharging
    if (state->per >= 78 && !state->hpn && state->isCharging == 1) {
        if ( sendMessage("quick the battery is about to explode! unplug") < 0 ) return -1;
        state->hpn = true;
    }
    if (state->per <= 25 && !state->lpn && state->isCharging == 2) {
        if (sendMessage("battery is thrisry") < 0 ) return -1;
        state->lpn = true;
    }

    if (state->per < 77) state->hpn = false;
    if (state->per > 26 ) state->lpn = false;

    return 0;
}

int parsing(sd_bus_message *m, void *userdata, sd_bus_error *error) {
    bool triggerCheck = false;
    (void) error;
    battery *state = (battery*)userdata;
    const char *interface;
    //we read to skip it
    sd_bus_message_read(m, "s", &interface);

    // enter the a{sv} array
    sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{sv}");

    while (sd_bus_message_enter_container(m, SD_BUS_TYPE_DICT_ENTRY, "sv") > 0) { // loop as much it returns
        const char *key;
        sd_bus_message_read(m, "s", &key);  // read the dictonary key
        if (strcmp(key, "Percentage") == 0) {
            sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, "d");
            sd_bus_message_read(m, "d", &state->per);
            sd_bus_message_exit_container(m);  // exit variant
            triggerCheck = true;
            goto next;
        } 
        if (strcmp(key, "State") == 0) {

            sd_bus_message_enter_container(m,SD_BUS_TYPE_VARIANT, "u");
            sd_bus_message_read(m, "u", &state->isCharging);
            sd_bus_message_exit_container(m);
            goto next;
        } 

        sd_bus_message_skip(m, "v");  // skip unneeded variant
        
        next:
        sd_bus_message_exit_container(m);  // exit the current {sv} dictonary entry
    }

     sd_bus_message_exit_container(m);  // exit the a{sv} array
     if (triggerCheck) {
         if (logic(state) < 0) return -1;
     }
     return 0;
    }
