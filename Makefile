.PHONY: build rebuild clean
CFLAGS = $(shell pkg-config --cflags libnotify)
LIBS = $(shell pkg-config --libs libnotify)
build:
	gcc -Wall -Wextra -O2 src/battery.c src/notify.c src/event.c -o build/application  $(CFLAGS) $(LIBS) -lsystemd


rebuild: clean build

clean:
	rm -f notify
