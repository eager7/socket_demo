
TARGET := epoll_server

SOURCE := $(TARGET).c

PROJ_DFLAGS := -D_REENTRANT
PROJ_LIBS += -lpthread -ljson-c -lsqlite3

CC := gcc
RM := -rm
CFLAGS += -g -Wno-unused-but-set-variable -Wall


OBJS := $(patsubst %.c,%.o,$(SOURCE))

.PHONY: all clean clean_obj
all: $(TARGET) clean_obj
default: all 

	
$(TARGET):$(OBJS)
	@$(CC) $(PROJ_DFLAGS) $^ $(PROJ_CFLAGS) $(CFLAGS) $(PROJ_LIBS) -o $@ 
	
%.o:%.c
	$(CC)  -I. $(CFLAGS) $(PROJ_CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(TARGET)

clean_obj:
	$(RM) $(OBJS)
 


