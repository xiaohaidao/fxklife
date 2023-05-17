
#third party library

set(GIT_COMMON_CPP_REPOSITORY ${GITHUB_URL_PREFIX}https://github.com/xiaohaidao/common_cpp.git)
include(dep_common_cpp)

# set(GIT_AV_CPP_REPOSITORY ${GITHUB_URL_PREFIX}https://github.com/xiaohaidao/av_cpp.git)
# include(dep_av_cpp)

set(GIT_PROTOBUF_REPOSITORY ${GITHUB_URL_PREFIX}https://github.com/protocolbuffers/protobuf.git)
set(PROTOBUF_VERSION v21.12) # need c++11
# set(FETCHCONTENT_SOURCE_DIR_PROTOBUF ${CMAKE_SOURCE_DIR}/.third_party/protobuf-src)
include(dep_protobuf)

# include(dep_libusb)
include(libusb)

include(ffmpeg)
