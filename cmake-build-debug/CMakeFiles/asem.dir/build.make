# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2021.2.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2021.2.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\Aya\Documents\GitHub\NOS-Emulator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/asem.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/asem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/asem.dir/flags.make

CMakeFiles/asem.dir/asem.c.obj: CMakeFiles/asem.dir/flags.make
CMakeFiles/asem.dir/asem.c.obj: ../asem.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/asem.dir/asem.c.obj"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\asem.dir\asem.c.obj -c C:\Users\Aya\Documents\GitHub\NOS-Emulator\asem.c

CMakeFiles/asem.dir/asem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/asem.dir/asem.c.i"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Users\Aya\Documents\GitHub\NOS-Emulator\asem.c > CMakeFiles\asem.dir\asem.c.i

CMakeFiles/asem.dir/asem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/asem.dir/asem.c.s"
	C:\MinGW\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Users\Aya\Documents\GitHub\NOS-Emulator\asem.c -o CMakeFiles\asem.dir\asem.c.s

# Object files for target asem
asem_OBJECTS = \
"CMakeFiles/asem.dir/asem.c.obj"

# External object files for target asem
asem_EXTERNAL_OBJECTS =

asem.exe: CMakeFiles/asem.dir/asem.c.obj
asem.exe: CMakeFiles/asem.dir/build.make
asem.exe: CMakeFiles/asem.dir/linklibs.rsp
asem.exe: CMakeFiles/asem.dir/objects1.rsp
asem.exe: CMakeFiles/asem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable asem.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\asem.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/asem.dir/build: asem.exe
.PHONY : CMakeFiles/asem.dir/build

CMakeFiles/asem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\asem.dir\cmake_clean.cmake
.PHONY : CMakeFiles/asem.dir/clean

CMakeFiles/asem.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\Aya\Documents\GitHub\NOS-Emulator C:\Users\Aya\Documents\GitHub\NOS-Emulator C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug C:\Users\Aya\Documents\GitHub\NOS-Emulator\cmake-build-debug\CMakeFiles\asem.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/asem.dir/depend
