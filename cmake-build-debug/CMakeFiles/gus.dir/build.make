# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

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
CMAKE_COMMAND = /home/andrey/DevKits/c++/clion-2021.1.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/andrey/DevKits/c++/clion-2021.1.1/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/andrey/Projects/c++/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andrey/Projects/c++/test/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/gus.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/gus.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/gus.dir/flags.make

CMakeFiles/gus.dir/client.cpp.o: CMakeFiles/gus.dir/flags.make
CMakeFiles/gus.dir/client.cpp.o: ../client.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/andrey/Projects/c++/test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/gus.dir/client.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gus.dir/client.cpp.o -c /home/andrey/Projects/c++/test/client.cpp

CMakeFiles/gus.dir/client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gus.dir/client.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/andrey/Projects/c++/test/client.cpp > CMakeFiles/gus.dir/client.cpp.i

CMakeFiles/gus.dir/client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gus.dir/client.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/andrey/Projects/c++/test/client.cpp -o CMakeFiles/gus.dir/client.cpp.s

CMakeFiles/gus.dir/structure_bd.cpp.o: CMakeFiles/gus.dir/flags.make
CMakeFiles/gus.dir/structure_bd.cpp.o: ../structure_bd.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/andrey/Projects/c++/test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/gus.dir/structure_bd.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/gus.dir/structure_bd.cpp.o -c /home/andrey/Projects/c++/test/structure_bd.cpp

CMakeFiles/gus.dir/structure_bd.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gus.dir/structure_bd.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/andrey/Projects/c++/test/structure_bd.cpp > CMakeFiles/gus.dir/structure_bd.cpp.i

CMakeFiles/gus.dir/structure_bd.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gus.dir/structure_bd.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/andrey/Projects/c++/test/structure_bd.cpp -o CMakeFiles/gus.dir/structure_bd.cpp.s

# Object files for target gus
gus_OBJECTS = \
"CMakeFiles/gus.dir/client.cpp.o" \
"CMakeFiles/gus.dir/structure_bd.cpp.o"

# External object files for target gus
gus_EXTERNAL_OBJECTS =

gus: CMakeFiles/gus.dir/client.cpp.o
gus: CMakeFiles/gus.dir/structure_bd.cpp.o
gus: CMakeFiles/gus.dir/build.make
gus: /usr/lib64/libcurl.so
gus: CMakeFiles/gus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/andrey/Projects/c++/test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable gus"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/gus.dir/build: gus

.PHONY : CMakeFiles/gus.dir/build

CMakeFiles/gus.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/gus.dir/cmake_clean.cmake
.PHONY : CMakeFiles/gus.dir/clean

CMakeFiles/gus.dir/depend:
	cd /home/andrey/Projects/c++/test/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/andrey/Projects/c++/test /home/andrey/Projects/c++/test /home/andrey/Projects/c++/test/cmake-build-debug /home/andrey/Projects/c++/test/cmake-build-debug /home/andrey/Projects/c++/test/cmake-build-debug/CMakeFiles/gus.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/gus.dir/depend

