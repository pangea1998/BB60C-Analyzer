cmake_minimum_required(VERSION 3.16)
project(BB60C_Analyzer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Qt paketlerini bul
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Gui
    Widgets
    PrintSupport  # QCustomPlot için gerekli
)

# Kaynak dosyaları
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/waterfallplot.cpp
    src/demodulator.cpp
    src/analyzer.cpp
    src/datamanager.cpp
    include/qcustomplot/qcustomplot.cpp
    include/bb_api/bb_api.cpp
)

# Header dosyaları
set(HEADERS
    src/mainwindow.h
    src/waterfallplot.h
    src/demodulator.h
    src/analyzer.h
    src/datamanager.h
    include/qcustomplot/qcustomplot.h
    include/bb_api/bb_api.h
)

# Kaynaklar (ikonlar vb.)
set(RESOURCES
    resources.qrc
)

# Executable oluştur
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Include dizinleri
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/qcustomplot
    ${CMAKE_CURRENT_SOURCE_DIR}/include/bb_api
)

# Qt kütüphanelerini bağla
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)

# Windows için özel ayarlar
if(WIN32)
    # Windows subsystem
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # DLL'leri kopyala
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE:Qt6::PrintSupport>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    # Platform plugin'lerini kopyala
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
    )
endif()

# Kurulum hedefleri
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/${PROJECT_NAME}
) 