
CC = gcc
TARGET = flash
SRC = flashlearn.c

$(TARGET): $(SRC)
	$(CC) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)