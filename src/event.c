#include <systemd/sd-bus.h>

int main() {
    sd_bus *bus = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;
    sd_bus_message *reply = NULL;

    sd_bus_open_system(&bus); // assigns me to the dbus

    if (sd_bus_call_method(bus,  // to get back the device i want path
            "org.freedesktop.UPower",
            "/org/freedesktop/UPower",
            "org.freedesktop.UPower",
            "GetDisplayDevice",
            &error,
            &reply,
            NULL) < 0) return 1;

    const char *device_path;
    sd_bus_message_read(reply, "o", &device_path); // read it as object in device path
    sd_bus_message_unref(reply);
    reply = NULL;

    if (sd_bus_get_property(bus, // get the percentage propety from upower
            "org.freedesktop.UPower",
            device_path,
            "org.freedesktop.UPower.Device",
            "Percentage",
            &error,
            &reply,
            "d") < 0) return 1;

    double percentage;
    sd_bus_message_read(reply, "d", &percentage);
    sd_bus_message_unref(reply);

    sd_bus_error_free(&error);
    sd_bus_unref(bus);

    printf("percentage = %d\n", (int)percentage);
    return 0;
}
