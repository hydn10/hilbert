set(target hilbert-cli)

add_executable(${target}
  "src/main.cpp"
  "src/collectors.cpp"
)

target_include_directories(${target} PRIVATE include)
target_compile_features(${target} PRIVATE cxx_std_23)

target_link_libraries(${target} PRIVATE hilbert)

install(TARGETS ${target})
