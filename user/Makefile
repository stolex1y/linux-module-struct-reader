TARGET=user
CC=gcc
CFLAGS=-g --std=c17 -Wall -pedantic -ggdb -Wextra -Werror

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $@
	
run: all
	sudo ./$(TARGET)
	

clean:
	rm -rf $(TARGET)

