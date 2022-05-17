TARGET_EXEC ?= test_bit_seq_reader

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

# Find files
SRCS := $(shell find $(SRC_DIRS) -name *.c)

# File.c turns into File.c.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# File.c.o turns into File.c.d
DEPS := $(OBJS:.o=.d)

# Every subfolder in src need to be passed
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

# Add "I" prefix to INC_DIRS
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= -std=c99 -g -Wall -Wextra -Werror $(INC_FLAGS)

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
