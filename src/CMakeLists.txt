cmake_minimum_required(VERSION 3.25)
project(State)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Boost 1.85.0 REQUIRED COMPONENTS charconv system date_time regex url json fiber thread program_options random)
find_package(OpenSSL REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARIES})

add_executable(Serve
        main.cpp
        app.cpp
        app.hpp
        state.hpp
        network/protocol.hpp
        entities/worker.cpp
        entities/worker.hpp
        network/listener.hpp
        network/http_session.hpp
        network/websocket_session.hpp
        network/handle_request.hpp
        network/fail.hpp
        network/mime_type.hpp
        network/path_cat.hpp
        network/stamper.hpp
        cipher.hpp
        network/protocol.cpp
        network/websocket_session.cpp
)

target_link_libraries(Serve PUBLIC ${Boost_LIBRARIES} ${OPENSSL_LIBRARIES})