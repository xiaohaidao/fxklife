
include(libusb)

set(library_name "fxklife")
add_executable(${library_name}
    main.cpp
)

target_link_libraries(${library_name} PUBLIC
    # proactor
    # sockets
    # utils
    libusb
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
