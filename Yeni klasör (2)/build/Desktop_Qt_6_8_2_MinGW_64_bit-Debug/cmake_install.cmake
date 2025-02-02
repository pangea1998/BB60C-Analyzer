# Install script for directory: C:/Users/PC_N_815/Desktop/Yeni klasör (2)

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/BB60C_Analyzer")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Qt/Tools/mingw1310_64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE EXECUTABLE FILES "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/BB60C_Analyzer.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/BB60C_Analyzer.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/BB60C_Analyzer.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "C:/Qt/Tools/mingw1310_64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/BB60C_Analyzer.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/main.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/mainwindow.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/analyzer.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/demodulator.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/datamanager.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/waterfallplot.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/bb_api.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/qcustomplot.cpp;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/mainwindow.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/analyzer.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/demodulator.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/datamanager.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/waterfallplot.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/bb_api.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/qcustomplot.h;C:/Users/PC_N_815/Desktop/Yeni klasör (2)/resources.qrc")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  file(INSTALL DESTINATION "C:/Users/PC_N_815/Desktop/Yeni klasör (2)" TYPE FILE FILES
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/main.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/mainwindow.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/analyzer.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/demodulator.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/datamanager.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/waterfallplot.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/include/bb_api/bb_api.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/include/qcustomplot/qcustomplot.cpp"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/mainwindow.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/analyzer.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/demodulator.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/datamanager.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/src/waterfallplot.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/include/bb_api/bb_api.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/include/qcustomplot/qcustomplot.h"
    "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/resources.qrc"
    )
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/PC_N_815/Desktop/Yeni klasör (2)/build/Desktop_Qt_6_8_2_MinGW_64_bit-Debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
