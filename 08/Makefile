CC ?= clang
CFLAGS = -g -Werror -Wall -Wextra -D_GNU_SOURCE -std=c11
LDFLAGS = -lopus
INCLUDE = -I/usr/include/opus
TARGETS = bin/icmp_send bin/icmp_recv bin/ping_phone

all: $(TARGETS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

bin/%: %.o
	@mkdir -p bin
	$(CC) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(TARGETS)
