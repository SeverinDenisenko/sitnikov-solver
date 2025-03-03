add_executable(poincare_app main.cpp)

target_link_libraries(poincare_app PRIVATE sitnikov yaml-cpp::yaml-cpp)
