cmake_minimum_required(VERSION 3.0)
project(monitor-tgbot CXX)
set(CMAKE_CXX_STANDARD 17)

set(LUA_VERSION "5.3" CACHE STRING "5.3")
set(LUA_LIBRARY lua${LUA_VERSION})

add_executable(monitor-tgbot main.cpp BotConfig.cpp)
target_link_libraries(monitor-tgbot PRIVATE ssl crypto pthread boost_system TgBot SQLiteCpp sqlite3 ${LUA_LIBRARY} stdc++fs)
