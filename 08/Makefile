COMPILER = gcc
CFLAGS = -Werror -Wextra
LDFLAGS = -lopus
INCLUDE = -I/usr/include/opus

all: icmp_send

%.o: %.c
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

icmp_send: icmp_send.o
	$(COMPILER) -o $@ $< $(LDFLAGS)
