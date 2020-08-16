
IF (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(FILESYSTEM_LIBS ${FILESYSTEM_LIBS})
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(FILESYSTEM_LIBS ${FILESYSTEM_LIBS})
    set(BLE_LIBS ${BLE_LIBS} "-framework Foundation" "-framework CoreBluetooth" objc)
ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(FILESYSTEM_LIBS ${FILESYSTEM_LIBS} stdc++fs)
    find_package(PkgConfig REQUIRED)
    pkg_search_module(DBUS REQUIRED dbus-1)
    if(DBUS_FOUND)
        include_directories(${DBUS_INCLUDE_DIRS})
        message(STATUS "Using DBUS from path: ${DBUS_INCLUDE_DIRS}")
    endif()

    set(BLE_LIBS ${BLE_LIBS} ${DBUS_LIBRARIES} pthread)
ENDIF()

# Combine with configure_outputs.cmake