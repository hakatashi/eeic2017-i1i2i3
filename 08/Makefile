COMPILER = gcc
CFLAGS = -Werror -Wextra
LDFLAGS = -lopus
INCLUDE = -I/usr/include/opus

all: bin/icmp_send bin/icmp_recv

%.o: %.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

bin/%: %.o
	@mkdir -p bin
	$(COMPILER) -o $@ $< $(LDFLAGS)
