if (NOT TARGET googletest)
    # Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24:
    if (POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
    endif ()

    if (POLICY CMP0077)
        cmake_policy(SET CMP0077 NEW)
        set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    endif ()

    set(gtest_force_shared_crt ON)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.17.0.zip
            URL_HASH MD5=3471f5011afc37b6555f6619c14169cf
    )
    FetchContent_MakeAvailable(googletest)
endif ()
