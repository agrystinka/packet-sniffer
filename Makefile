CC = gcc
CFLAGS = -std=gnu18 -O2 -Wall -Wextra -Wpedantic -pthread -lpcap
SNIFFER = packet-sniffer
CLI = cli

SRC_SNIFFER = main.c \
	          sniffer.c \
			  logerr.c \
	          cmdhandler.c

SRC_CLI = cli.c

BUILDDIR = ./build
SRCDIR = ./src
INCDIR = ./inc

.PHONY: all clean tidy

all: $(BUILDDIR) $(SNIFFER) $(CLI)
	rm -rf socket
	rm -rf *.txt

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) $(addprefix -I,$(INCDIR)) -c $< -o $@

$(SNIFFER): $(addprefix $(BUILDDIR)/,$(SRC_SNIFFER:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(CLI): $(addprefix $(BUILDDIR)/,$(SRC_CLI:.c=.o))
	$(CC) $(CFLAGS) $^ -o $@

$(BUILDDIR):
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR)

tidy: clean
	rm -f $(CLI)
	rm -f $(SNIFFER)

run:
	sudo ./$(SNIFFER)

runcli:
	#use: sudo ./cli [command]
	sudo ./$(CLI) --help


lint: lint-clang-tidy

# target to run all linters
lint-all: | lint-clang-tidy lint-splint lint-oclint lint-cppcheck
_oclint := oclint
_splint := splint +checks $(addprefix -I ,$(INCDIRS))
_cppcheck := cppcheck -q -j$$(($$(nproc)+1)) $(addprefix -I,$(INCDIR)) \
	--platform=unix64 \
	--enable=warning,style,performance,portability,information \
	--std=c11 --language=c --verbose --inconclusive
_clang-tidy := clang-tidy --quiet --checks='*'

lint-oclint: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> OCLint report\e[38;5;130m"
	$(_oclint) 2>/dev/null $^ | head -n -2 | tail -n +1
	@echo -en "\e[0m"

lint-clang-tidy: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> Clang-Tidy report\e[38;5;130m"
	$(_clang-tidy) $^ -- $(addprefix -I,$(INCDIR)) $(CFLAGS) 2>/dev/null | cat
	@echo -en "\e[0m"

lint-splint: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> SPLint report\e[38;5;130m"
	$(_splint) 2>&1 $^ | tail -n +2
	@echo -en "\e[0m"

lint-cppcheck: $(addprefix $(SRCDIR)/,$(SRC))
	@echo -e "\e[1m\e[92m>>> CPPCheck report\e[38;5;130m"
	$(_cppcheck) $^ && echo
	@echo -en "\e[0m"
