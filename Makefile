CC = gcc
CFLAGS = -Wall -Wextra -g

all: ./src/main.c
	$(CC) $(CFLAGS) -o main ./src/main.c ./src/core/device.c ./src/core/nic.c ./src/core/port.c ./src/devices/host.c ./src/core/link.c