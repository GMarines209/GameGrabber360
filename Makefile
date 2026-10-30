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
	
test_utils.exe: tests/test_utils.c utils.c utils.h vendor/unity/src/unity.c vendor/unity/src/unity.h vendor/unity/src/unity_internals.h
	$(CC) $(CFLAGS) -I. -Ivendor/unity/src -o test_utils.exe tests/test_utils.c utils.c vendor/unity/src/unity.c

test: test_utils.exe
	.\test_utils.exe

.PHONY: test clean
clean:
	del /Q *.o $(TARGET) test_utils.exe