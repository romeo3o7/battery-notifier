.PHONY: build rebuild clean
CFLAGS = $(shell pkg-config --cflags libnotify)
LIBS = $(shell pkg-config --libs libnotify)
buildtest:
	mkdir -p build/test
	gcc -Wall -Wextra -O2 src/message.c src/notify.c src/event.c -o build/test/application  $(CFLAGS) $(LIBS) -lsystemd

buildfinal:
	mkdir -p build/release
	gcc -Wall -Wextra -O2 src/message.c src/notify.c src/event.c -o build/release/application  $(CFLAGS) $(LIBS) -lsystemd

rebuild: clean buildtest

clean:
	rm -f build/release/* build/test/*
