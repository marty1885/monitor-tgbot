cmake_minimum_required(VERSION 3.0)
project(monitor-tgbot CXX)
set(CMAKE_CXX_STANDARD 17)

find_package(Lua REQUIRED)

add_executable(monitor-tgbot main.cpp BotConfig.cpp)
target_link_libraries(monitor-tgbot PRIVATE ssl crypto pthread boost_system TgBot SQLiteCpp sqlite3 ${LUA_LIBRARY} stdc++fs)
