# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /nfshome0/jsledgei/brildaq-nivisa

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /nfshome0/jsledgei/brildaq-nivisa/build

# Utility rule file for Nightly.

# Include the progress variables for this target.
include CMakeFiles/Nightly.dir/progress.make

CMakeFiles/Nightly:
	/usr/bin/ctest -D Nightly

Nightly: CMakeFiles/Nightly
Nightly: CMakeFiles/Nightly.dir/build.make
.PHONY : Nightly

# Rule to build all files generated by this target.
CMakeFiles/Nightly.dir/build: Nightly
.PHONY : CMakeFiles/Nightly.dir/build

CMakeFiles/Nightly.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Nightly.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Nightly.dir/clean

CMakeFiles/Nightly.dir/depend:
	cd /nfshome0/jsledgei/brildaq-nivisa/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /nfshome0/jsledgei/brildaq-nivisa /nfshome0/jsledgei/brildaq-nivisa /nfshome0/jsledgei/brildaq-nivisa/build /nfshome0/jsledgei/brildaq-nivisa/build /nfshome0/jsledgei/brildaq-nivisa/build/CMakeFiles/Nightly.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Nightly.dir/depend

