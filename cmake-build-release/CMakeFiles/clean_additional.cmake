# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/cars_sim_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/cars_sim_autogen.dir/ParseCache.txt"
  "cars_sim_autogen"
  "libs/fmt/CMakeFiles/fmt_autogen.dir/AutogenUsed.txt"
  "libs/fmt/CMakeFiles/fmt_autogen.dir/ParseCache.txt"
  "libs/fmt/fmt_autogen"
  )
endif()
