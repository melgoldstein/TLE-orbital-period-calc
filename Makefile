CC = gcc
CFLAGS = -O2 -Wall -Wextra -Werror -std=c99
all: tle_calc
tle_calc: tle_calc.c
	$(CC) $(CFLAGS) -Iinclude tle_calc.c -lm -o $@
clean:
	rm -f tle_calc tle_calc.exe *.o
.PHONY: all clean


test: tle_calc
	python3 tests/test_cli.py
.PHONY: test
