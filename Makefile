CC = gcc
CFLAGS = -Wall -Wextra
TARGET = GameGrabber360.exe
OBJ = main.o utils.o file_browser.o config.o

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) -lole32 -luuid

main.o: main.c structs.h utils.h file_browser.h config.h
	$(CC) $(CFLAGS) -c main.c

config.o: config.c config.h structs.h utils.h
	$(CC) $(CFLAGS) -c config.c

clean:
	del /Q *.o $(TARGET) 