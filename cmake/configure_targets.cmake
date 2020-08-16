
# message("-- CMAKE_SYSTEM_INFO_FILE: ${CMAKE_SYSTEM_INFO_FILE}")
# message("-- CMAKE_SYSTEM_NAME:      ${CMAKE_SYSTEM_NAME}")
# message("-- CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}")
# message("-- CMAKE_SYSTEM:           ${CMAKE_SYSTEM}")
# message(${CMAKE_SIZEOF_VOID_P}) # We'll keep this for later.

# Configure Compiler Flags for each Platform
IF (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(WINVERSION_CODE 0x0A00) # Selected Windows 10 based on https://docs.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt
    
    # Add all the special definitions that need to be added for the program to properly compile on windows.
    set(SPECIAL_DEFINITIONS "/D_WIN32_WINNT=${WINVERSION_CODE} /D_USE_MATH_DEFINES")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /W2 /wd4251 ${SPECIAL_DEFINITIONS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c11 /W2 /wd4251 ${SPECIAL_DEFINITIONS}")
    add_definitions(-DOS_WINDOWS)

ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++17 -Wfatal-errors")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c11 -Wfatal-errors")
    add_definitions(-DOS_DARWIN)

ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -std=c++17 -Wfatal-errors -Wno-ignored-attributes -Wpedantic")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -std=c11 -Wfatal-errors -Wno-ignored-attributes -Wpedantic")
    add_definitions(-DOS_LINUX)

ENDIF()

if(DEFINE_DEBUG)
    message(STATUS "DEBUG MODE ENABLED")
    add_definitions(-DDEBUG)

    IF (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

    ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")  

    ELSEIF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")  

    ENDIF()

endif()

# ----------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------

