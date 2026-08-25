set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Match the standard library used by the consuming Clang project.
set(VCPKG_C_FLAGS "")
set(VCPKG_CXX_FLAGS "-stdlib=libc++")
