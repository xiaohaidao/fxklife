
if(WIN32)
    set(_local_install ${CPP_PACKAGE_DIR}/ffmpeg)

    if(EXISTS ${_local_install}/bin/ffmpeg.exe)
        set(_ffmpeg_exist ON)
    endif()
    if(NOT _ffmpeg_exist)
        include(FetchContent)
        FetchContent_Declare(
            ffmpeg
            URL https://www.gyan.dev/ffmpeg/builds/packages/ffmpeg-6.0-full_build-shared.7z
        )
        FetchContent_MakeAvailable(ffmpeg)
        file(COPY ${ffmpeg_SOURCE_DIR}/ DESTINATION ${_local_install})

    endif()

    macro(COMPONENT_ADD_LIBRARY _target _library _include)
        if(NOT TARGET FFmpeg::${_target})
            add_library(lib${_target} INTERFACE)
            target_include_directories(lib${_target} INTERFACE ${_include})
            target_link_directories(lib${_target} INTERFACE ${_library})
            target_link_libraries(lib${_target} INTERFACE ${_target})

            add_library(FFmpeg::${_target} ALIAS lib${_target})
        endif()
    endmacro()

    COMPONENT_ADD_LIBRARY(avcodec ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(avformat ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(avdevice ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(avutil ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(avfilter ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(swscale ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(postproc ${_local_install}/lib/ ${_local_install}/include)
    COMPONENT_ADD_LIBRARY(swresample ${_local_install}/lib/ ${_local_install}/include)

    message("ffmpeg_exist ${_local_install}")
endif(WIN32)
