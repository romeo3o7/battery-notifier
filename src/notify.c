#include <libnotify/notify.h>
#include <stdio.h>

int notifyInit() {
    if (!notify_init("battery_app")) {
        fprintf(stderr,"failed to init the application\n");
        return -1;
    }
    return 0;
}

int sendMessage(const char *c) {
    NotifyNotification *m = notify_notification_new("Charge", c, NULL);

    if (m == NULL) {
        fprintf(stderr, "failed to create notification object\n");
        return -1;
    }
    if (!notify_notification_show(m, NULL)) {
        fprintf(stderr, "failed to send notification object\n");
        g_object_unref(G_OBJECT(m));
        return -1;
    }
    g_object_unref(G_OBJECT(m));
    return 0;
}

void notifyUninit() {
    notify_uninit();
}
