project(CCTools)
cmake_minimum_required(VERSION 2.8.11)

if(POLICY CMP0054)
    # Silence warning about string comparison to unquoted names
    cmake_policy(SET CMP0054 NEW)
endif()

include(TestBigEndian)
test_big_endian(BYTES_BIG_ENDIAN)
if(${BYTES_BIG_ENDIAN})
    add_definitions(-DBYTES_BIG_ENDIAN)
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
    set(CMAKE_COMPILER_IS_CLANG 1)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_C_FLAGS "-Wall -Wextra ${CMAKE_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++0x ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
    add_definitions("/D_CRT_SECURE_NO_WARNINGS")
endif()

set(CCTools_Tilesets
    ${CMAKE_CURRENT_SOURCE_DIR}/share/cctools/TW32.tis
    ${CMAKE_CURRENT_SOURCE_DIR}/share/cctools/WEP.tis
)

add_subdirectory(lib/libcc1)
add_subdirectory(src/CCEdit)
add_subdirectory(src/CCPlay)
add_subdirectory(src/CCHack)

if(WIN32)
    install(FILES ${CCTools_Tilesets}
            DESTINATION .
    )
else()
    install(FILES ${CCTools_Tilesets}
            DESTINATION share/cctools
    )
endif()
