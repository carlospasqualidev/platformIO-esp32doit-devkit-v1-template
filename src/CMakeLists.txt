# # This file was automatically generated for projects
# # without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources})


# set(component_srcs "ssd1306.c" "ssd1306_i2c.c" "ssd1306_spi.c")



#idf_component_register(SRCS "bmp180.c" INCLUDE_DIRS .)
