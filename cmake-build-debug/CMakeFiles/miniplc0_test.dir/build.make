# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.14

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "E:\Program Files\JetBrains\CLion 2019.2.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "E:\Program Files\JetBrains\CLion 2019.2.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\my-mini

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\my-mini\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/miniplc0_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/miniplc0_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/miniplc0_test.dir/flags.make

CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj: CMakeFiles/miniplc0_test.dir/flags.make
CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj: CMakeFiles/miniplc0_test.dir/includes_CXX.rsp
CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj: ../tests/test_main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\my-mini\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj"
	D:\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\miniplc0_test.dir\tests\test_main.cpp.obj -c E:\my-mini\tests\test_main.cpp

CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\my-mini\tests\test_main.cpp > CMakeFiles\miniplc0_test.dir\tests\test_main.cpp.i

CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\my-mini\tests\test_main.cpp -o CMakeFiles\miniplc0_test.dir\tests\test_main.cpp.s

CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj: CMakeFiles/miniplc0_test.dir/flags.make
CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj: CMakeFiles/miniplc0_test.dir/includes_CXX.rsp
CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj: ../tests/test_tokenizer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\my-mini\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj"
	D:\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\miniplc0_test.dir\tests\test_tokenizer.cpp.obj -c E:\my-mini\tests\test_tokenizer.cpp

CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\my-mini\tests\test_tokenizer.cpp > CMakeFiles\miniplc0_test.dir\tests\test_tokenizer.cpp.i

CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\my-mini\tests\test_tokenizer.cpp -o CMakeFiles\miniplc0_test.dir\tests\test_tokenizer.cpp.s

CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj: CMakeFiles/miniplc0_test.dir/flags.make
CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj: CMakeFiles/miniplc0_test.dir/includes_CXX.rsp
CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj: ../tests/test_analyser.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\my-mini\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj"
	D:\mingw64\bin\g++.exe  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles\miniplc0_test.dir\tests\test_analyser.cpp.obj -c E:\my-mini\tests\test_analyser.cpp

CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.i"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\my-mini\tests\test_analyser.cpp > CMakeFiles\miniplc0_test.dir\tests\test_analyser.cpp.i

CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.s"
	D:\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\my-mini\tests\test_analyser.cpp -o CMakeFiles\miniplc0_test.dir\tests\test_analyser.cpp.s

# Object files for target miniplc0_test
miniplc0_test_OBJECTS = \
"CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj" \
"CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj" \
"CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj"

# External object files for target miniplc0_test
miniplc0_test_EXTERNAL_OBJECTS =

miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/tests/test_main.cpp.obj
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/tests/test_tokenizer.cpp.obj
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/tests/test_analyser.cpp.obj
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/build.make
miniplc0_test.exe: libminiplc0_lib.a
miniplc0_test.exe: 3rd_party/fmt/libfmtd.a
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/linklibs.rsp
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/objects1.rsp
miniplc0_test.exe: CMakeFiles/miniplc0_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\my-mini\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX executable miniplc0_test.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\miniplc0_test.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/miniplc0_test.dir/build: miniplc0_test.exe

.PHONY : CMakeFiles/miniplc0_test.dir/build

CMakeFiles/miniplc0_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\miniplc0_test.dir\cmake_clean.cmake
.PHONY : CMakeFiles/miniplc0_test.dir/clean

CMakeFiles/miniplc0_test.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\my-mini E:\my-mini E:\my-mini\cmake-build-debug E:\my-mini\cmake-build-debug E:\my-mini\cmake-build-debug\CMakeFiles\miniplc0_test.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/miniplc0_test.dir/depend

