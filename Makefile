# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/szp/Documents/Utility-master

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/szp/Documents/Utility-master

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/usr/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/szp/Documents/Utility-master/CMakeFiles /home/szp/Documents/Utility-master/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/szp/Documents/Utility-master/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named TestContain

# Build rule for target.
TestContain: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 TestContain
.PHONY : TestContain

# fast build rule for target.
TestContain/fast:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/build
.PHONY : TestContain/fast

#=============================================================================
<<<<<<< HEAD
# Target rules for targets named adler32

# Build rule for target.
adler32: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 adler32
.PHONY : adler32

# fast build rule for target.
adler32/fast:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/build
.PHONY : adler32/fast
=======
# Target rules for targets named sserver

# Build rule for target.
sserver: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 sserver
.PHONY : sserver

# fast build rule for target.
sserver/fast:
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/build
.PHONY : sserver/fast
>>>>>>> 8d2ca4b9abd980599f27bfa56ed7e0e70c481eee

HttpUtility.o: HttpUtility.cpp.o

.PHONY : HttpUtility.o

# target to build an object file
HttpUtility.cpp.o:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/HttpUtility.cpp.o
<<<<<<< HEAD
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/HttpUtility.cpp.o
=======
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/HttpUtility.cpp.o
>>>>>>> 8d2ca4b9abd980599f27bfa56ed7e0e70c481eee
.PHONY : HttpUtility.cpp.o

HttpUtility.i: HttpUtility.cpp.i

.PHONY : HttpUtility.i

# target to preprocess a source file
HttpUtility.cpp.i:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/HttpUtility.cpp.i
<<<<<<< HEAD
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/HttpUtility.cpp.i
=======
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/HttpUtility.cpp.i
>>>>>>> 8d2ca4b9abd980599f27bfa56ed7e0e70c481eee
.PHONY : HttpUtility.cpp.i

HttpUtility.s: HttpUtility.cpp.s

.PHONY : HttpUtility.s

# target to generate assembly for a file
HttpUtility.cpp.s:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/HttpUtility.cpp.s
<<<<<<< HEAD
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/HttpUtility.cpp.s
=======
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/HttpUtility.cpp.s
>>>>>>> 8d2ca4b9abd980599f27bfa56ed7e0e70c481eee
.PHONY : HttpUtility.cpp.s

TestContain.o: TestContain.cpp.o

.PHONY : TestContain.o

# target to build an object file
TestContain.cpp.o:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/TestContain.cpp.o
.PHONY : TestContain.cpp.o

TestContain.i: TestContain.cpp.i

.PHONY : TestContain.i

# target to preprocess a source file
TestContain.cpp.i:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/TestContain.cpp.i
.PHONY : TestContain.cpp.i

TestContain.s: TestContain.cpp.s

.PHONY : TestContain.s

# target to generate assembly for a file
TestContain.cpp.s:
	$(MAKE) -f CMakeFiles/TestContain.dir/build.make CMakeFiles/TestContain.dir/TestContain.cpp.s
.PHONY : TestContain.cpp.s

adler32.o: adler32.cpp.o

.PHONY : adler32.o

# target to build an object file
adler32.cpp.o:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/adler32.cpp.o
.PHONY : adler32.cpp.o

adler32.i: adler32.cpp.i

.PHONY : adler32.i

# target to preprocess a source file
adler32.cpp.i:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/adler32.cpp.i
.PHONY : adler32.cpp.i

adler32.s: adler32.cpp.s

.PHONY : adler32.s

# target to generate assembly for a file
adler32.cpp.s:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/adler32.cpp.s
.PHONY : adler32.cpp.s

md5.o: md5.cpp.o

.PHONY : md5.o

# target to build an object file
md5.cpp.o:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/md5.cpp.o
.PHONY : md5.cpp.o

md5.i: md5.cpp.i

.PHONY : md5.i

# target to preprocess a source file
md5.cpp.i:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/md5.cpp.i
.PHONY : md5.cpp.i

md5.s: md5.cpp.s

.PHONY : md5.s

# target to generate assembly for a file
md5.cpp.s:
	$(MAKE) -f CMakeFiles/adler32.dir/build.make CMakeFiles/adler32.dir/md5.cpp.s
.PHONY : md5.cpp.s

sserver.o: sserver.cpp.o

.PHONY : sserver.o

# target to build an object file
sserver.cpp.o:
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/sserver.cpp.o
.PHONY : sserver.cpp.o

sserver.i: sserver.cpp.i

.PHONY : sserver.i

# target to preprocess a source file
sserver.cpp.i:
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/sserver.cpp.i
.PHONY : sserver.cpp.i

sserver.s: sserver.cpp.s

.PHONY : sserver.s

# target to generate assembly for a file
sserver.cpp.s:
	$(MAKE) -f CMakeFiles/sserver.dir/build.make CMakeFiles/sserver.dir/sserver.cpp.s
.PHONY : sserver.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
<<<<<<< HEAD
	@echo "... sserver"
	@echo "... TestContain"
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... adler32"
=======
	@echo "... rebuild_cache"
	@echo "... edit_cache"
	@echo "... TestContain"
	@echo "... sserver"
>>>>>>> 8d2ca4b9abd980599f27bfa56ed7e0e70c481eee
	@echo "... HttpUtility.o"
	@echo "... HttpUtility.i"
	@echo "... HttpUtility.s"
	@echo "... TestContain.o"
	@echo "... TestContain.i"
	@echo "... TestContain.s"
	@echo "... adler32.o"
	@echo "... adler32.i"
	@echo "... adler32.s"
	@echo "... md5.o"
	@echo "... md5.i"
	@echo "... md5.s"
	@echo "... sserver.o"
	@echo "... sserver.i"
	@echo "... sserver.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

