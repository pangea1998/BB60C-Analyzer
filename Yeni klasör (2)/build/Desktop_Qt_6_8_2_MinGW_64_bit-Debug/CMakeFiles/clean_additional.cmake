# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "BB60C_Analyzer_autogen"
  "CMakeFiles\\BB60C_Analyzer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\BB60C_Analyzer_autogen.dir\\ParseCache.txt"
  )
endif()
