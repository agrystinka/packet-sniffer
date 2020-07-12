CC = gcc
CFLAGS = -std=gnu18 -O2 -Wall -Wextra -Wpedantic -pthread -lpcap
SNIFFER = packet-sniffer
CLI = cli

SRC_SNIFFER = main.c \
 	          logerr.c \
	          sniffer.c \
	          cmdhandler.c

SRC_CLI =  cli.c \
		   logerr.c

BUILDDIR = ./build
SRCDIR = ./src
INCDIR = ./inc

.PHONY: all clean tidy

all: $(BUILDDIR) $(BUILDDIR)/$(SNIFFER) $(BUILDDIR)/$(CLI)
	rm -rf socket
	rm -rf *.txt

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/$(SNIFFER): $(addprefix $(BUILDDIR)/,$(SRC_SNIFFER:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR)/$(CLI): $(addprefix $(BUILDDIR)/,$(SRC_CLI:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR):
	mkdir -p $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)

tidy: clean
	rm -rf *.txt

run:
	sudo $(BUILDDIR)/$(SNIFFER)
	#sudo $(CLI)

lint: | lint-splint lint-oclint

lint-oclint: $(SRCS)
		@echo -e "\e[1m\e[92m>>> OCLint report\e[38;5;130m"
		oclint 2>/dev/null $^ | head -n -2 | tail -n +1
		@echo -en "\e[0m"

lint-splint:$(SRCS)
		@echo -e "\e[1m\e[92m>>> SPLint report\e[38;5;130m"
		splint 2>&1 $^ | tail -n +2
		@echo -en "\e[0m"
