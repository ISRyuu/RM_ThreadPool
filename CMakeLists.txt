add_executable(main main.c is_threadpool.c is_priority_queue.c)
target_link_libraries(main pthread)
