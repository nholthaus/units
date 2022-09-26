include(FetchContent)
FetchContent_Declare(
        googletest
        URL ${CMAKE_CURRENT_SOURCE_DIR}/googletest-1.12.1.zip
        URL_HASH MD5=2648d4138129812611cf6b6b4b497a3b
)
FetchContent_MakeAvailable(googletest)