# Operating System
OS := windows

# Include Directories
IDIR := ./include ../Libraries/MinGW/Allegro/include
# Library Directories
LDIR := ./lib ../Libraries/MinGW/Allegro/lib
# Source Directory
SDIR := ./src
# Build Directory
BDIR := ./build
# Resulting Executable
TARGET := ./game

# Compilers
CC := gcc
CXX := g++
# Compiler Flags
CFLAGS := -pedantic -Wall -Wextra -Wconversion -g -Werror
CXXFLAGS := -pedantic -Wall -Wextra -Wconversion -g -Werror
# Include Flags
IFLAGS := $(addprefix -I,$(IDIR))
# Dependency Flags
DFLAGS = -MMD -MF $(@:.o=.d)
# Linker Flags
LFLAGS := $(addprefix -L,$(LDIR)) -lmingw32 -lallegro -lallegro_main -lallegro_font -lallegro_ttf
LFLAGS += -lallegro_primitives -lallegro_audio -lallegro_video -lallegro_color -lallegro_image
ifeq ($(OS), windows)
	LFLAGS += -mconsole -mwindows
endif

# Recursive Wildcard: searches through the given directory and all sub-directories.
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Source Files
SRCS_C := $(call rwildcard, $(SDIR), *.c)
SRCS_CXX := $(call rwildcard, $(SDIR), *.cpp *.cxx *.cc)
SRCS := $(SRCS_C) $(SRCS_CXX)

# Header Files
HDRS_C := $(call rwildcard, $(SDIR), *.h)
HDRS_CXX := $(call rwildcard, $(SDIR), *.hpp *.hxx *.hh)
HDRS := $(HDRS_C) $(HDRS_CXX)

# Object Files
OBJS_C := $(subst $(SDIR), $(BDIR), $(addsuffix .o, $(SRCS_C)))
OBJS_CXX := $(subst $(SDIR), $(BDIR), $(addsuffix .o, $(SRCS_CXX)))
OBJS := $(OBJS_C) $(OBJS_CXX)

BDIRS := $(sort $(dir $(OBJS)))

# Dependency Files
DEPS := $(OBJS:.o=.d)
-include $(DEPS)

# Start Command Sequence
all : $(BDIRS) $(TARGET)
	@echo Build Successful.

# $(call endsin, main.cc, $(SRCS))
# Creating Build Directories (if missing)
$(BDIRS) :
ifeq ($(OS), windows)
	@-md "$(@D)"
else 
	@-mkdir -p "$(@D)"
endif

# Linking Object Files
$(TARGET) : $(OBJS)
	@$(CXX) $^ $(CXXFLAGS) $(LFLAGS) -o $@

# Compiling C Source Files into Object Files
$(OBJS_C) : $(BDIR)/%.o : $(SDIR)/%
	@$(CC) -c $< $(CFLAGS) $(DFLAGS) $(IFLAGS) -o $@

# Compiling C++ Source Files into Object Files
$(OBJS_CXX) : $(BDIR)/%.o : $(SDIR)/%
	@$(CXX) -c $< $(CXXFLAGS) $(DFLAGS) $(IFLAGS) -o $@

# Remove Build Directory and Target Executable on Clean
.PHONY : clean
clean:
ifeq ($(OS), windows)
	@del "$(TARGET).exe" & rmdir /s /q "$(BDIR)"
else
	@rm "$(TARGET)" & rm -r "$(BDIR)"
endif