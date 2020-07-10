CC = gcc
CFLAGS = -std=gnu18 -O2 -Wall -Wextra -Wpedantic -pthread -lpcap
TARGET = server
TARGETCLI = cli
SRC = main.c \
 	  logerr.c \
	  sniffer.c \
	  cmdhandler.c

SRCCLI =  cli.c \
		  logerr.c

.PHONY: all clean tidy

all: $(TARGET) $(TARGETCLI)
	rm -rf socket*
	rm -rf *.txt
	#sudo ./server


$(TARGET): $(SRC:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

$(TARGETCLI): $(SRCCLI:.c=.o)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o

tidy: clean
	rm -rf $(TARGET)

run:
	$TARGET
	$TARGETCLI

lint: | lint-splint lint-oclint

lint-oclint: $(SRCS)
		@echo -e "\e[1m\e[92m>>> OCLint report\e[38;5;130m"
		oclint 2>/dev/null $^ | head -n -2 | tail -n +1
		@echo -en "\e[0m"

lint-splint:$(SRCS)
		@echo -e "\e[1m\e[92m>>> SPLint report\e[38;5;130m"
		splint 2>&1 $^ | tail -n +2
		@echo -en "\e[0m"
