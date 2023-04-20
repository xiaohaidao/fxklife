
include(FetchContent)
FetchContent_Declare(
    libusb1_0
    URL ${PROJECT_SOURCE_DIR}/../libusb-1.0.26-binaries.7z
    # GIT_TAG dev
)
FetchContent_MakeAvailable(libusb1_0)


add_library(libusb INTERFACE)

target_include_directories(libusb
    INTERFACE
    ${libusb1_0_SOURCE_DIR}/libusb-MinGW-x64/include/
)

target_link_directories(libusb
    INTERFACE
    # ${libusb1_0_SOURCE_DIR}/VS2015-x64/lib/
    ${libusb1_0_SOURCE_DIR}/libusb-MinGW-x64/lib/
)

target_link_libraries(libusb
    INTERFACE
    libusb-1.0
)