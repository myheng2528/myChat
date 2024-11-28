cmake_minimum_required(VERSION 3.8)

# 项目信息
project(server LANGUAGES CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 手动设置库路径
set(BOOST_ROOT "D:/cppsoft/boost_1_84_0")
set(JSONCPP_DIR "D:/cppsoft/jsoncpp")

# 查找 Boost
set(BOOST_LIBRARYDIR "${BOOST_ROOT}/stage/lib")
find_package(Boost REQUIRED COMPONENTS system filesystem)

if (NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found! Please check BOOST_ROOT or BOOST_LIBRARYDIR.")
endif()

message(STATUS "Boost Include Dir: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost Libraries: ${Boost_LIBRARIES}")

# 查找 JsonCpp 库
if (NOT EXISTS "${JSONCPP_DIR}/include/json/json.h")
  message(FATAL_ERROR "JsonCpp headers not found in ${JSONCPP_DIR}/include.")
endif()

# 手动设置 JsonCpp 库路径并查找
find_library(JSONCPP_LIBRARY NAMES jsoncpp_static libjsoncpp_static HINTS "${JSONCPP_DIR}/lib")

if (NOT JSONCPP_LIBRARY)
  message(FATAL_ERROR "JsonCpp library not found in ${JSONCPP_DIR}/lib.")
else()
  message(STATUS "JsonCpp Library: ${JSONCPP_LIBRARY}")
endif()

# 添加可执行文件
add_executable(server "server.cpp" "server.h")

# 添加头文件路径和库路径
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS} ${JSONCPP_DIR}/include)
link_directories(${JSONCPP_DIR}/lib)

# 链接库
target_link_libraries(server PRIVATE ${Boost_LIBRARIES} ${JSONCPP_LIBRARY})
set_target_properties(server PROPERTIES
    INSTALL_RPATH "${JSONCPP_DIR}/lib"
)
