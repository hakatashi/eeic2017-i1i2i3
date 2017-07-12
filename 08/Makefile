COMPILER = clang
CFLAGS = -g -Werror -Wall -Wextra -D_GNU_SOURCE -std=c11
LDFLAGS = -lopus
INCLUDE = -I/usr/include/opus

all: bin/icmp_send bin/icmp_recv bin/ping_phone

%.o: %.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

bin/%: %.o
	@mkdir -p bin
	$(COMPILER) -o $@ $< $(LDFLAGS)
