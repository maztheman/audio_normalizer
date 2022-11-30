find_package(Drogon)
find_package(fmt)

add_executable(anserver main.cpp)

target_link_libraries(anserver PRIVATE Drogon::Drogon fmt::fmt anworker)
