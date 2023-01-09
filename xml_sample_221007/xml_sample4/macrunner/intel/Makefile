
CC = gcc
AR = ar
CFLAGS = -Wall -DNR5G

SRC_DIR = ./
INC_DIR = ./
OUT_DIR = ./

TARGET = libintel.a
INC = -I$(INC_DIR)

SRCS = $(wildcard $(SRC_DIR)*.c)
OBJS = $(patsubst %.c,$(OUT_DIR)%.o,$(notdir $(SRCS)))

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(AR) ur $(OUT_DIR)$(TARGET) $?

$(OUT_DIR)%.o : $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

clean :
	rm -f $(OUT_DIR)$(TARGET)
	rm -f $(OBJS)
