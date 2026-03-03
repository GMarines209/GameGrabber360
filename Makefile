CC = gcc
CFLAGS = -Wall -Wextra
TARGET = GameGrabber360.exe

$(TARGET): main.o utils.o file_browser.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o utils.o file_browser.o -lole32 -luuid

main.o: main.c structs.h utils.h file_browser.h
	$(CC) $(CFLAGS) -c main.c

clean:
	del /Q *.o $(TARGET)