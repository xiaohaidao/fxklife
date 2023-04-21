
include(libusb)
include(common_cpp)

# protobuf
find_package(Protobuf REQUIRED)
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/carlife/proto/*.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

set(library_name "Fxklife")
add_executable(${library_name}
    ${PROTO_SRCS}
    ${PROTO_HDRS}
    carlife/channel/ChannelBase.cpp
    carlife/channel/CmdChannel.cpp
    carlife/channel/CtrlChannel.cpp
    carlife/channel/MediaChannel.cpp
    carlife/channel/TTSChannel.cpp
    carlife/channel/VideoChannel.cpp
    carlife/channel/VRChannel.cpp
    carlife/CarlifeMobileDevice.cpp
    MobileDevice.cpp
    main.cpp
)

target_link_libraries(${library_name} PUBLIC
    proactor
    sockets
    utils
    # libusb
    ${Protobuf_LIBRARIES}
)

target_include_directories(${library_name} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    PUBLIC $<INSTALL_INTERFACE:include/>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
)

target_compile_features(${library_name} PRIVATE
    cxx_std_11
)

# install and export configure
include(install_export)
