
if(WIN32)
    set(_local_install ${CPP_PACKAGE_DIR}/libusb)

    if(EXISTS ${_local_install}/README)
        set(_libusb_exist ON)
    endif()
    if(NOT _libusb_exist)
        include(FetchContent)
        FetchContent_Declare(
            libusb1_0
            URL https://github.com/libusb/libusb/releases/download/v1.0.26/libusb-1.0.26-binaries.7z
        )
        FetchContent_MakeAvailable(libusb1_0)

        file(COPY ${libusb1_0_SOURCE_DIR}/ DESTINATION ${_local_install})
    endif()


    if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(_LIBUSB_PREFIX ${_local_install}/libusb-MinGW-x64/)
    else()
        set(_LIBUSB_PREFIX ${_local_install}/libusb-MinGW-Win32/)
    endif()

    if(MSVC)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_LIBUSB_PREFIX ${_local_install}/VS2015-x64/)
        else()
            set(_LIBUSB_PREFIX ${_local_install}/VS2015-Win32/)
        endif()
    endif()
    message("_LIBUSB_PREFIX ${_LIBUSB_PREFIX}")

    add_library(libusb INTERFACE)

    target_include_directories(libusb INTERFACE
        ${_LIBUSB_PREFIX}/../libusb-MinGW-x64/include/
    )

    target_link_directories(libusb INTERFACE
        ${_LIBUSB_PREFIX}/lib/
    )

    target_link_libraries(libusb INTERFACE
        libusb-1.0
    )

else(WIN32)

    find_package(PkgConfig QUIET)
    pkg_check_modules(libusb IMPORTED_TARGET libusb-1.0)

    if (TARGET PkgConfig::libusb)
        add_library(libusb INTERFACE)

        target_include_directories(libusb INTERFACE
            ${libusb_INCLUDE_DIRS}
        )

        target_link_directories(libusb INTERFACE
            ${libusb_LIBRARY_DIRS}
        )

        target_link_libraries(libusb INTERFACE
            ${libusb_LIBRARIES}
        )
    endif()

endif(WIN32)
