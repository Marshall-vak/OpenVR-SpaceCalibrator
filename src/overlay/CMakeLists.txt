cmake_minimum_required (VERSION 3.8)

project(SpaceCalibrator2Overlay)
message("SpaceCalibrator2 - Overlay")

function(GroupSourcesByFolder target)
    set(SOURCE_GROUP_DELIMITER "/")
    set(last_dir "")
    set(files "")

    get_target_property(sources ${target} SOURCES)

    foreach(file ${sources})
        file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}" ${file})
        get_filename_component(dir "${relative_file}" PATH)
        if(NOT "${dir}" STREQUAL "${last_dir}")
            if(files)
                source_group("${last_dir}" FILES ${files})
            endif()
            set(files "")
        endif()
        set(files ${files} ${file})
        set(last_dir "${dir}")
    endforeach()

    if(files)
        source_group("${last_dir}" FILES ${files})
    endif()
endfunction()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_SOURCE_DIR}/src/overlay/SpaceCalibrator2.rc)
endif()

# Add source files
file(GLOB_RECURSE SOURCES_API ${CMAKE_SOURCE_DIR}/src/overlay "*.c" "*.h" "*.hpp" "*.cpp")
file(GLOB_RECURSE SOURCES_HEADERS ${CMAKE_SOURCE_DIR}/src/common "*.h" "*.hpp")

foreach(SOURCE IN ITEMS ${SOURCES_API})
    get_filename_component(SOURCE_PATH "${SOURCE}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP_PATH "${SOURCE_PATH}")
    string(REPLACE "/" "\\" GROUP_PATH "${GROUP_PATH}")
    source_group("${GROUP_PATH}" FILES "${SOURCE}")
endforeach()

set(IMGUI_FILES
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/lib/imgui/backends/imgui_impl_glfw.cpp)

add_executable(SpaceCalibrator2Overlay ${SOURCES_API} ${SOURCES_HEADERS} ${IMGUI_FILES} ${WIN32_RESOURCES})
GroupSourcesByFolder(SpaceCalibrator2Overlay)
# Disable console window
set_target_properties(SpaceCalibrator2Overlay PROPERTIES
    LINK_FLAGS_DEBUG            "/SUBSYSTEM:CONSOLE /ENTRY:wWinMainCRTStartup /NODEFAULTLIB:library"
    LINK_FLAGS_MINSIZEREL       "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /NODEFAULTLIB:library"
    LINK_FLAGS_RELEASE          "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup /NODEFAULTLIB:library"
    LINK_FLAGS_RELWITHDEBINFO   "/SUBSYSTEM:CONSOLE /ENTRY:wWinMainCRTStartup /NODEFAULTLIB:library")

target_include_directories(SpaceCalibrator2Overlay
    PUBLIC ${CMAKE_SOURCE_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/common
    PUBLIC ${CMAKE_SOURCE_DIR}/src/overlay
    PUBLIC ${CMAKE_SOURCE_DIR}/lib
    PUBLIC ${CMAKE_SOURCE_DIR}/lib/openvr/headers
    PUBLIC ${CMAKE_SOURCE_DIR}/lib/imgui/backends
    PUBLIC ${CMAKE_SOURCE_DIR}/lib/Eigen
)

target_link_libraries(SpaceCalibrator2Overlay
    PRIVATE opengl32.lib
    PRIVATE dwmapi.lib
    PRIVATE glfw
    PRIVATE gl3w
    PRIVATE imgui
    PRIVATE implot
    PRIVATE ${CMAKE_SOURCE_DIR}/lib/openvr/lib/win64/openvr_api.lib)

target_compile_definitions(SpaceCalibrator2Overlay
    PRIVATE NOMINMAX
    PRIVATE UNICODE
    PRIVATE IMGUI_DEFINE_MATH_OPERATORS
)

# Object files are too big with spacecal sooooo
if (MSVC)
    target_compile_options(SpaceCalibrator2Overlay PRIVATE /bigobj)
else ()
    target_compile_options(SpaceCalibrator2Overlay PRIVATE -Wa,-mbig-obj)
endif ()

# Copy vrmanifest
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/overlay/manifest.vrmanifest
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/manifest.vrmanifest
)

# Copy dashboard icon
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/overlay/icon.png
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/icon.png
)

# Copy taskbar icon
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/overlay/taskbar_icon.png
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/taskbar_icon.png
)

# Copy LICENSE
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/LICENSE
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/LICENSE
)

# Copy NOTICE
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/NOTICE
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/README
)

# Copy openvr_api.dll
add_custom_command(
    TARGET SpaceCalibrator2Overlay POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/lib/openvr/bin/win64/openvr_api.dll
    ${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>/openvr_api.dll
)

# Set as startup project
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT SpaceCalibrator2Overlay)
set_property(TARGET SpaceCalibrator2Overlay PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/artifacts/$<CONFIG>")
set_target_properties(SpaceCalibrator2Overlay PROPERTIES OUTPUT_NAME "SpaceCalibrator2")