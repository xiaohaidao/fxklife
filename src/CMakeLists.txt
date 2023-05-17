
file(GLOB PROTO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/carlife/proto/*.proto)
GENERATE_PROTOBUF(
    TARGET proto-obj
    IMPORT_DIR carlife/proto
    PROTO_SRC
    ${PROTO_FILES}
)

set(library_name "Fxklife")
add_executable(${library_name}
    av/CaptureAudioWasapi.cpp
    av/CaptureDesktopDxgi.cpp
    av/Decoder.cpp
    av/Encoder.cpp
    carlife/channel/ChannelBase.cpp
    carlife/channel/CmdChannel.cpp
    carlife/channel/CtrlChannel.cpp
    carlife/channel/MediaChannel.cpp
    carlife/channel/TTSChannel.cpp
    carlife/channel/VideoChannel.cpp
    carlife/channel/VRChannel.cpp
    carlife/AOA.cpp
    carlife/CarlifeHeadUnit.cpp
    carlife/CarlifeMobileDevice.cpp
    AOAService.cpp
    HeadUnit.cpp
    MobileDevice.cpp
    ServiceDiscovery.cpp
    TransferPort.cpp
    VideoService_win.cpp
    main.cpp
    # "main copy 2.cpp"
)

target_link_libraries(${library_name} PUBLIC
    common_cpp::proactor
    common_cpp::utils
    FFmpeg::avcodec
    FFmpeg::avutil
    FFmpeg::swscale
    libusb
    proto-obj
)

if(WIN32)
target_link_libraries(${library_name} PUBLIC
    d3d11
# winmm user32 Kernel32 Ole32
)
endif()

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
