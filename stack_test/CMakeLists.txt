LINK_LIBRARIES(m)
LINK_LIBRARIES(dl)
LINK_LIBRARIES(lua)

add_executable  (test1 test1.c)
add_executable  (test2 test2.c script.c)
add_executable  (test_global test_global.c)

