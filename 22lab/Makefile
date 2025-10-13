CC = gcc
CFLAGS = -Wall -pthread
SOURCES = main.c driver.c
TARGET = taxi_system

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: clean