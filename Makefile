CC = gcc
CFLAGS = -Wall -Iinclude

TARGET = bin/vault

SOURCES = src/main.c src/vault.c src/auth.c

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
