# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/or/Desktop/clion-2017.2.3/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/or/Desktop/clion-2017.2.3/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/or/CLionProjects/EX3_b

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/or/CLionProjects/EX3_b/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/EX3_b.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/EX3_b.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/EX3_b.dir/flags.make

CMakeFiles/EX3_b.dir/main.c.o: CMakeFiles/EX3_b.dir/flags.make
CMakeFiles/EX3_b.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/or/CLionProjects/EX3_b/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/EX3_b.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/EX3_b.dir/main.c.o   -c /home/or/CLionProjects/EX3_b/main.c

CMakeFiles/EX3_b.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/EX3_b.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/or/CLionProjects/EX3_b/main.c > CMakeFiles/EX3_b.dir/main.c.i

CMakeFiles/EX3_b.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/EX3_b.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/or/CLionProjects/EX3_b/main.c -o CMakeFiles/EX3_b.dir/main.c.s

CMakeFiles/EX3_b.dir/main.c.o.requires:

.PHONY : CMakeFiles/EX3_b.dir/main.c.o.requires

CMakeFiles/EX3_b.dir/main.c.o.provides: CMakeFiles/EX3_b.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/EX3_b.dir/build.make CMakeFiles/EX3_b.dir/main.c.o.provides.build
.PHONY : CMakeFiles/EX3_b.dir/main.c.o.provides

CMakeFiles/EX3_b.dir/main.c.o.provides.build: CMakeFiles/EX3_b.dir/main.c.o


# Object files for target EX3_b
EX3_b_OBJECTS = \
"CMakeFiles/EX3_b.dir/main.c.o"

# External object files for target EX3_b
EX3_b_EXTERNAL_OBJECTS =

EX3_b: CMakeFiles/EX3_b.dir/main.c.o
EX3_b: CMakeFiles/EX3_b.dir/build.make
EX3_b: CMakeFiles/EX3_b.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/or/CLionProjects/EX3_b/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable EX3_b"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/EX3_b.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/EX3_b.dir/build: EX3_b

.PHONY : CMakeFiles/EX3_b.dir/build

CMakeFiles/EX3_b.dir/requires: CMakeFiles/EX3_b.dir/main.c.o.requires

.PHONY : CMakeFiles/EX3_b.dir/requires

CMakeFiles/EX3_b.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/EX3_b.dir/cmake_clean.cmake
.PHONY : CMakeFiles/EX3_b.dir/clean

CMakeFiles/EX3_b.dir/depend:
	cd /home/or/CLionProjects/EX3_b/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/or/CLionProjects/EX3_b /home/or/CLionProjects/EX3_b /home/or/CLionProjects/EX3_b/cmake-build-debug /home/or/CLionProjects/EX3_b/cmake-build-debug /home/or/CLionProjects/EX3_b/cmake-build-debug/CMakeFiles/EX3_b.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/EX3_b.dir/depend

