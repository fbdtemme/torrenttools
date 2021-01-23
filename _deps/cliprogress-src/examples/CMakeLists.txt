#add_executable(progress-examples progress.cpp)
#add_executable(bar-and-spinner progress_example.cpp)
#add_executable(example example.cpp)
add_executable(signal_notifier signal_notifier.cpp)
add_executable(progress_example progress_example.cpp)
add_executable(layout_example layout_example.cpp)

target_link_libraries(signal_notifier PRIVATE cliprogress)
target_link_libraries(progress_example PRIVATE cliprogress)
target_link_libraries(layout_example PRIVATE cliprogress)

