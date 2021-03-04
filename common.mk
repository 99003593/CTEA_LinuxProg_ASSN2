# Set Default Project name to a
PROJ ?= a
# Set default build directory to ./build
OBJ_DIR ?= ./build
# Add Include dirs to CFLAGS
CFLAGS += $(addprefix -I,$(INCS))
# Create list of objects from source files
OBJS := $(addprefix $(OBJ_DIR)/,$(SRCS:.c=.o))

# Final output executable program
OBJ := $(OBJ_DIR)/$(PROJ).out

# Control printing with verbosity
ifeq ($(V),1)
	Q :=
else
	Q := @
endif

# Protect against removing working directory
ifeq ($(abspath $(OBJ_DIR)),$(abspath .))
	RM_OBJS := $(OBJS) $(OBJ)
else
	RM_OBJS := $(OBJ_DIR)
endif

# Make these targets always
.PHONY: all run clean

all: $(OBJ)

run: $(OBJ)
	$(Q)./$(OBJ)

# Cleanup by removing object dir
clean:
	$(Q)rm -rf $(RM_OBJS)

# Default rule for linking output
$(OBJ): $(OBJS)
# Only print pretty if not vebose
ifneq ($(V),1)
	@echo "    CC \t$@"
endif
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

# Default rule for compiles object files
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
# Only print pretty if not vebose
ifneq ($(V),1)
	@echo "    CC \t$@"
endif
	$(Q)$(CC) -c $(CFLAGS) $< -o $@
