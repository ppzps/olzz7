# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ggz/code/olzz7

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ggz/code/olzz7/build

# Include any dependencies generated for this target.
include CMakeFiles/test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test.dir/flags.make

CMakeFiles/test.dir/test.cpp.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/test.cpp.o: ../test.cpp
CMakeFiles/test.dir/test.cpp.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ggz/code/olzz7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test.dir/test.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/test.cpp.o -MF CMakeFiles/test.dir/test.cpp.o.d -o CMakeFiles/test.dir/test.cpp.o -c /home/ggz/code/olzz7/test.cpp

CMakeFiles/test.dir/test.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test.dir/test.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ggz/code/olzz7/test.cpp > CMakeFiles/test.dir/test.cpp.i

CMakeFiles/test.dir/test.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test.dir/test.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ggz/code/olzz7/test.cpp -o CMakeFiles/test.dir/test.cpp.s

CMakeFiles/test.dir/damage.cpp.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/damage.cpp.o: ../damage.cpp
CMakeFiles/test.dir/damage.cpp.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ggz/code/olzz7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/test.dir/damage.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/damage.cpp.o -MF CMakeFiles/test.dir/damage.cpp.o.d -o CMakeFiles/test.dir/damage.cpp.o -c /home/ggz/code/olzz7/damage.cpp

CMakeFiles/test.dir/damage.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test.dir/damage.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ggz/code/olzz7/damage.cpp > CMakeFiles/test.dir/damage.cpp.i

CMakeFiles/test.dir/damage.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test.dir/damage.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ggz/code/olzz7/damage.cpp -o CMakeFiles/test.dir/damage.cpp.s

CMakeFiles/test.dir/unit.cpp.o: CMakeFiles/test.dir/flags.make
CMakeFiles/test.dir/unit.cpp.o: ../unit.cpp
CMakeFiles/test.dir/unit.cpp.o: CMakeFiles/test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ggz/code/olzz7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/test.dir/unit.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test.dir/unit.cpp.o -MF CMakeFiles/test.dir/unit.cpp.o.d -o CMakeFiles/test.dir/unit.cpp.o -c /home/ggz/code/olzz7/unit.cpp

CMakeFiles/test.dir/unit.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test.dir/unit.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ggz/code/olzz7/unit.cpp > CMakeFiles/test.dir/unit.cpp.i

CMakeFiles/test.dir/unit.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test.dir/unit.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ggz/code/olzz7/unit.cpp -o CMakeFiles/test.dir/unit.cpp.s

# Object files for target test
test_OBJECTS = \
"CMakeFiles/test.dir/test.cpp.o" \
"CMakeFiles/test.dir/damage.cpp.o" \
"CMakeFiles/test.dir/unit.cpp.o"

# External object files for target test
test_EXTERNAL_OBJECTS =

test: CMakeFiles/test.dir/test.cpp.o
test: CMakeFiles/test.dir/damage.cpp.o
test: CMakeFiles/test.dir/unit.cpp.o
test: CMakeFiles/test.dir/build.make
test: CMakeFiles/test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ggz/code/olzz7/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test.dir/build: test
.PHONY : CMakeFiles/test.dir/build

CMakeFiles/test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test.dir/clean

CMakeFiles/test.dir/depend:
	cd /home/ggz/code/olzz7/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ggz/code/olzz7 /home/ggz/code/olzz7 /home/ggz/code/olzz7/build /home/ggz/code/olzz7/build /home/ggz/code/olzz7/build/CMakeFiles/test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test.dir/depend

