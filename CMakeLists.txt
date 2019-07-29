project(TestContain)
set(SOURCE TestContain.cpp HttpUtility.cpp adler32.cpp md5.cpp)
set(CMAKE_CXX_STANDARD 14)
add_executable(${PROJECT_NAME} ${SOURCE})

project(sserver)
set(SOURCE sserver.cpp HttpUtility.cpp)
set(CMAKE_CXX_STANDARD 17)
add_executable(${PROJECT_NAME} ${SOURCE})