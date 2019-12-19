# CMake generated Testfile for 
# Source directory: E:/my-mini
# Build directory: E:/my-mini/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(all_test "miniplc0_test")
set_tests_properties(all_test PROPERTIES  _BACKTRACE_TRIPLES "E:/my-mini/CMakeLists.txt;72;add_test;E:/my-mini/CMakeLists.txt;0;")
subdirs("3rd_party/argparse")
subdirs("3rd_party/fmt")
subdirs("3rd_party/catch2")
