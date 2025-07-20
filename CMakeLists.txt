cmake_minimum_required(VERSION 3.15)
project(scs_sdk_plugin)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-fPIC -Wall")

# Linux only: set soname
if(UNIX AND NOT APPLE)
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-soname,scs_sdk_plugin.so")
endif()

# Platform-specific library suffix
if(APPLE)
    set(LIB_SUFFIX ".dylib")
    set(CMAKE_OSX_ARCHITECTURES "x86_64")
elseif(UNIX)
    set(LIB_SUFFIX ".so")
endif()

include_directories(scs-telemetry/inc)
include_directories(scs_sdk_1_14/include)

add_library(scs_sdk_plugin SHARED
        scs-telemetry/inc/log.hpp
        scs-telemetry/inc/scs-telemetry-common.hpp
        scs-telemetry/inc/scs_config_handlers.hpp
        scs-telemetry/inc/scs_gameplay_event_handlers.hpp
        scs-telemetry/inc/sharedmemory.hpp
        scs-telemetry/src/log.cpp
        scs-telemetry/src/scs_config_handlers.cpp
        scs-telemetry/src/scs_gameplay_event_handlers.cpp
        scs-telemetry/src/scs_telemetry.cpp
        scs-telemetry/src/sharedmemory.cpp
        scs_sdk_1_14/include/amtrucks/scssdk_ats.h
        scs_sdk_1_14/include/amtrucks/scssdk_telemetry_ats.h
        scs_sdk_1_14/include/common/scssdk_telemetry_common_channels.h
        scs_sdk_1_14/include/common/scssdk_telemetry_common_configs.h
        scs_sdk_1_14/include/common/scssdk_telemetry_common_gameplay_events.h
        scs_sdk_1_14/include/common/scssdk_telemetry_job_common_channels.h
        scs_sdk_1_14/include/common/scssdk_telemetry_trailer_common_channels.h
        scs_sdk_1_14/include/common/scssdk_telemetry_truck_common_channels.h
        scs_sdk_1_14/include/eurotrucks2/scssdk_eut2.h
        scs_sdk_1_14/include/eurotrucks2/scssdk_telemetry_eut2.h
        scs_sdk_1_14/include/scssdk.h
        scs_sdk_1_14/include/scssdk_telemetry.h
        scs_sdk_1_14/include/scssdk_telemetry_channel.h
        scs_sdk_1_14/include/scssdk_telemetry_event.h)

# Apply correct suffix based on platform
set_target_properties(scs_sdk_plugin PROPERTIES SUFFIX "${LIB_SUFFIX}")

# Add test executable
add_executable(test
        scs_sdk_1_14/include/scssdk_value.h
        scs-telemetry/src/test.cpp)

target_link_libraries(test scs_sdk_plugin)
