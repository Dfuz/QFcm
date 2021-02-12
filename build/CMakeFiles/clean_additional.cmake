# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "src/CMakeFiles/QFcm_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/QFcm_autogen.dir/ParseCache.txt"
  "src/QFcm_autogen"
  "test/CMakeFiles/fcmanager_autogen.dir/AutogenUsed.txt"
  "test/CMakeFiles/fcmanager_autogen.dir/ParseCache.txt"
  "test/fcmanager_autogen"
  )
endif()
