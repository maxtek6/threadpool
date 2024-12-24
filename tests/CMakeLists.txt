add_executable(
    unit 
    unit.cpp)

target_link_libraries(
    unit 
    PRIVATE 
    threadpool)

if(THREADPOOL_COVER)
    if(WIN32)
        message("skipping code coverage for windows")
    else()
        SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -g -O0")
        add_custom_target(
            cover
            DEPENDS unit)
        add_custom_command(
            TARGET cover
            COMMAND gcovr -r ${CMAKE_SOURCE_DIR} -e ${CMAKE_SOURCE_DIR}/tests)
    endif()
endif()

add_test(NAME test_constructor COMMAND unit CONSTRUCTOR)
add_test(NAME test_submit COMMAND unit SUBMIT)
add_test(NAME test_shutdown COMMAND unit SHUTDOWN)