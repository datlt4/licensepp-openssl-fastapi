# CMakeLists.txt
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(license_manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

find_package(Boost REQUIRED COMPONENTS system thread)
set(LIB ${LIB} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

# OpenSSL
find_package(OpenSSL REQUIRED)
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(LIB ${LIB} OpenSSL::Crypto)

# Licensepp
set(LIB ${LIB} OpenSSL::Crypto liblicensepp.so.1.0.6)

# add Source
file(GLOB LICENSE_MANAGER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(SRC ${SRC} ${LICENSE_MANAGER_SRC})
set(INC ${INC} ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "SRC: ${SRC}")
message(STATUS "INC: ${INC}")
message(STATUS "LIB: ${LIB}")

# Target
add_library(${PROJECT_NAME} SHARED ${SRC})
target_include_directories(${PROJECT_NAME} PUBLIC ${INC})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB})
target_compile_definitions(${PROJECT_NAME} PUBLIC LICENSE_MANAGER_VERSION=1.0)

# Install
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
