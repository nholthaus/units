# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(gtest_force_shared_crt ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.12.1.zip
        URL_HASH MD5=2648d4138129812611cf6b6b4b497a3b
)
FetchContent_MakeAvailable(googletest)