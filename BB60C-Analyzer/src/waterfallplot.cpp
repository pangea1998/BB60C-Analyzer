cmake_minimum_required(VERSION 3.15)
    project(BB60C_Analyzer VERSION 1.0)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)

# Qt yolunu belirt
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.2/msvc2019_64")

    find_package(Qt6 REQUIRED COMPONENTS
                     Core
                         Gui
                             Widgets
                                 PrintSupport
                 )

# Kaynak dosyaları
    set(SOURCES
            src/main.cpp
              src/mainwindow.cpp
              src/waterfallplot.cpp
              src/demodulator.cpp
              src/analyzer.cpp
              src/datamanager.cpp
              resources.qrc
        )

    set(HEADERS
            src/mainwindow.h
              src/waterfallplot.h
              src/demodulator.h
              src/analyzer.h
              src/datamanager.h
        )

# QCustomPlot'u ekle
    add_library(qcustomplot STATIC
                    include/qcustomplot/qcustomplot.cpp
                )

    target_include_directories(qcustomplot PUBLIC
                                   include/qcustomplot
                                   ${Qt6Core_INCLUDE_DIRS}
                               ${Qt6Widgets_INCLUDE_DIRS}
                               ${Qt6Gui_INCLUDE_DIRS}
                               ${Qt6PrintSupport_INCLUDE_DIRS}
                               )

    target_link_libraries(qcustomplot PUBLIC
                              Qt6::Core
                                  Qt6::Gui
                                      Qt6::Widgets
                                          Qt6::PrintSupport
                          )

# Executable oluştur
    add_executable(BB60C_Analyzer
                       ${SOURCES}
                   ${HEADERS}
                   )

# Include dizinleri
    target_include_directories(BB60C_Analyzer PRIVATE
                                   ${CMAKE_SOURCE_DIR}/src
                                   ${CMAKE_SOURCE_DIR}/include
                                   ${CMAKE_SOURCE_DIR}/include/qcustomplot
                                   ${CMAKE_SOURCE_DIR}/include/bb_api
                               )

# Kütüphaneleri bağla
    target_link_libraries(BB60C_Analyzer PRIVATE
                              Qt6::Core
                                  Qt6::Gui
                                      Qt6::Widgets
                                          Qt6::PrintSupport
                                              qcustomplot
                          )

# DLL'leri kopyala
    add_custom_command(TARGET BB60C_Analyzer POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy_if_different
                           "${CMAKE_SOURCE_DIR}/include/bb_api/bb_api.dll"
                           $<TARGET_FILE_DIR:BB60C_Analyzer>
                       )

# Qt DLL'lerini kopyala
    if(WIN32)
    add_custom_command(TARGET BB60C_Analyzer POST_BUILD
                               COMMAND ${CMAKE_COMMAND} -E copy_directory
                           "${CMAKE_PREFIX_PATH}/bin"
                           $<TARGET_FILE_DIR:BB60C_Analyzer>
                       )
    endif()
