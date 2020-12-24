# Add GLFW to build
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
add_subdirectory(glfw)

# Clean project up a little
set_target_properties(glfw uninstall PROPERTIES FOLDER Vendor)
