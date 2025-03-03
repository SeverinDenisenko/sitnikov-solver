add_executable(sitnikov_app main.cpp)

target_link_libraries(sitnikov_app PRIVATE sitnikov yaml-cpp::yaml-cpp)
