.PHONY: build rebuild clean
CFLAGS = $(shell pkg-config --cflags libnotify)
LIBS = $(shell pkg-config --libs libnotify)
build:
	gcc -Wall -Wextra -O2 main.c notify.c -o notify $(CFLAGS) $(LIBS)

rebuild: clean build

clean:
	rm -f notify
