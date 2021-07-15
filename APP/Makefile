# the compiler
CC = gcc

# compiler flags:
CFLAGS  = -g -Wall

# the build target executable:
TARGET = module_test_app

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
