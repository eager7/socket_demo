###########################################################################
#
# MODULE:             SocketCommunication - Makefile
#
# REVISION:           $Revision: 1.0 $
#
# DATED:              $Date: 2015-10-20 11:16:28 +0000 $
#
# AUTHOR:             PCT
#
###########################################################################
#
# Copyright panchangtao@gmail.com B.V. 2015. All rights reserved
#
###########################################################################

TARGET := Socket_Comm_Client

CC := gcc
RM := rm

SOURCE_DIR := .
CFLAGS += -I./
CFLAGS += -O2 -g -Wno-unused-but-set-variable -Wall
PROJ_DFLAGS := -D_REENTRANT
PROJ_LIB := -lpthread

SOURCE := $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCE))
OBJECTD := $(patsubst %.c,%.d,$(SOURCE))

.PHONY: all clean

all: $(TARGET)

$(TARGET):$(OBJECTS)
	$(CC) $(PROJ_DFLAGS) $^ $(CFLAGS) $(PROJ_LIB) -o $@
	
-include $(SOURCE:.c=.d)

%d:%c
	@set -e;$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's/.o:/.o $@:/g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

%.o:%.c utils.h 
	$(CC)  -I. $(CFLAGS) -c $< -o $@
	
clean:
	$(RM) $(TARGET) $(OBJECTS) *.d*
	
