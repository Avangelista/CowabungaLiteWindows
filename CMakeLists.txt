cmake_minimum_required(VERSION 3.5)

project(CowabungaLite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES CowabungaLite_en_US.ts)
set(QRC_FILES resources.qrc)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        devicemanager.h
        devicemanager.cpp
        statusmanager/StatusManager.h
        statusmanager/StatusManager.cpp
        statusmanager/StatusSetter.h
        statusmanager/StatusSetter15.h
        statusmanager/StatusSetter15.cpp
        statusmanager/StatusSetter16.h
        statusmanager/StatusSetter16.cpp
        statusmanager/StatusSetter16_1.h
        statusmanager/StatusSetter16_1.cpp
        statusmanager/StatusSetter16_3.h
        statusmanager/StatusSetter16_3.cpp
        utils.h
        utils.cpp
        CreateBackup.h
        CreateBackup.cpp
        plistmanager.h
        plistmanager.cpp
        HomeScreenApps.h
        HomeScreenApps.cpp
        ${TS_FILES}
        ${QRC_FILES}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mno-ms-bitfields -malign-double")

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CowabungaLite
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET CowabungaLite APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(CowabungaLite SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(CowabungaLite
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(CowabungaLite PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(CowabungaLite PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Copy preset files to the output directory
set(FILES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/files")
file(COPY ${FILES_PATH} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy the DLL files to the output directory
set(DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/dll")
set(EXE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/exe")
target_include_directories(CowabungaLite PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
if(WIN32)
    file(GLOB EXE_FILES "${EXE_PATH}/*.exe")
    file(COPY ${EXE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(CowabungaLite PRIVATE "${DLL_PATH}/libimobiledevice-1.0.dll")
    target_link_libraries(CowabungaLite PRIVATE "${DLL_PATH}/libplist-2.0.dll")
    target_link_libraries(CowabungaLite PRIVATE "${DLL_PATH}/libplist++-2.0.dll")
    target_link_libraries(CowabungaLite PRIVATE "${DLL_PATH}/libssl-3-x64.dll")
    target_link_libraries(CowabungaLite PRIVATE "${DLL_PATH}/libcrypto-3-x64.dll")
elseif(APPLE)
    file(GLOB DYLIB_FILES "${DLL_PATH}/*.dylib")
    file(COPY ${DYLIB_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(UNIX)
    file(GLOB SO_FILES "${DLL_PATH}/*.so")
    file(COPY ${SO_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()

install(TARGETS CowabungaLite
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CowabungaLite)
endif()
