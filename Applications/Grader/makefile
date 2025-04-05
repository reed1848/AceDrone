ifeq "$(SCOE_BASE)" ""
force:: ;
% : force
	@ echo ERROR: need a definition for SCOE_BASE
	@ exit 1
endif

ifeq "$(PLATFORM)" ""
force:: ;
% : force
	@ echo ERROR: need a definition for PLATFORM
	@ exit 1
endif
include $(SCOE_BASE)/bin/commonDefinitions.mak
include $(SCOE_BASE)/platforms/$(PLATFORM)/platformDefinitions.mak

TARGET := grader_main
APPLICATION_ENTRY_POINT := grader_main

BB_SIZE				 := -defsym TEXT_SECTION_SIZE=0x10000 \
						   -defsym DATA_SECTION_SIZE=0x1000 \
						   -defsym RODATA_SECTION_SIZE=0x1000 \
						   -defsym BSS_SECTION_SIZE=0x1000 \
						   -defsym BSS_PERSISTENT_SECTION_SIZE=0x1000 \
						   -defsym DATA_PERSISTENT_SECTION_SIZE=0x1000
# OPTIONAL: Program section size limits. Note that these sizes will vary between different
# architectures and compilers.
# Remove this and the corresponding linker flag if you're not familiar with this.

CFLAGS_COMMON := -Iinclude -I../Common/include $(CFLAGS_DEFAULT_INCLUDE) -DENTRY_POINT=$(APPLICATION_ENTRY_POINT) \
	-Wall -Werror -fno-builtin -fno-strict-aliasing -fno-crossjumping -lm
#   -DENTRY_POINT		  Define entry point for usrAppInit.c
#   -Wall				  Turn on all optional warnings which are desirable for normal code.
#   -Werror				Treat all warnings as errors
#   -fno-builtin		   No ISO C90 functions will be recognized as built-in functions
#   -fno-strict-aliasing   Do not allow the compiler to assume the strictest aliasing rules
#						  applicable to the language being compiled
#   -fno-crossjumping	  Do not use the cross-jumping transformation

CFLAGS_RELEASE := $(CFLAGS_COMMON) -O2
#   -O[0123]			   Control the optimization level

CFLAGS_DEBUG := $(CFLAGS_COMMON) -g
#   -g					 Generates debugging information

LINK_FLAGS = -lpthread -lrt  $(SSLINTERFACE_LIB_RELEASE) $(AUTOTEST_LIB_RELEASE) \
	$(MEMORY_WRAPPERS_C) \
	-Wl,-T $(PLATFORM_LDS)
# NORMAL FLAGS:
#   -lpthread			  Use threading library (required for SSL Interface Library)
#   -lrt				   Use realtime library (required for SSL Interface Library)
#   -Wl,-T $(PLATFORM_LDS) Platform linker script (required for persistent data backup)
#
# OPTIONAL FLAGS FOR ADDITIONAL CHECKS:
#   $(MEMORY_WRAPPERS_C)			Wrappers to make memory allocation/deallocation result in
#								   errors during NORMAL mode
#   $(call check_blackbox,BB_SIZE)  Instructs the linker to ensure the output program section
#								   sizes are less than or equal to those specified

SOURCE=src/Grader.c \
	   ../Common/src/Utilities.c \
	   ../Common/src/usrAppInit.c \
	   ../Common/src/LookupTable.c \
	   ../Common/src/Parameters.c \
	   $(MAIN_SOURCE)


.PHONY: all release debug clean

all: clean release

release:
	$(CC) $(CFLAGS_RELEASE) $(LINK_FLAGS) $(SOURCE) -o $(TARGET)

debug:
	$(CC) $(CFLAGS_DEBUG) $(LINK_FLAGS) $(SOURCE) -o $(TARGET)

clean:
	-$(RM) $(RMFLAGS) $(TARGET)
