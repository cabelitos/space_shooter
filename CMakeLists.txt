cmake_minimum_required(VERSION 2.8)
project(space_shooter C)

include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=c99" COMPILER_C99)
CHECK_C_COMPILER_FLAG("-Wall" COMPILER_WALL)
CHECK_C_COMPILER_FLAG("-Wextra" COMPILER_WALL_EXTRA)
if (COMPILER_C99)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")
else()
	message(FATAL_ERROR "No c99 support. bye." )
endif()

if (COMPILER_WALL)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
endif()

if (COMPILER_WALL_EXTRA)
   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra")
endif()

find_package(PkgConfig)
pkg_check_modules(ALLEGRO REQUIRED allegro-5)
pkg_check_modules(ALLEGRO_PRIMITIVES REQUIRED allegro_primitives-5)
pkg_check_modules(ALLEGRO_MAIN REQUIRED allegro_main-5)

include_directories(${ALLEGRO_INCLUDE_DIRS} ${ALLEGRO_PRIMITIVES_INCLUDE_DIRS} ${ALLEGRO_MAIN_INCLUDE_DIRS})
link_directories(${ALLEGRO_LIBRARY_DIRS}  ${ALLEGRO_PRIMITIVES_LIBRARY_DIRS} ${ALLEGRO_MAIN_LIBRARY_DIRS})

add_executable(space_shooter src/main.c src/game.c src/space_ship.c src/float_compare.c src/cannon.c src/utils.c)

target_link_libraries(space_shooter ${ALLEGRO_LIBRARIES} ${ALLEGRO_PRIMITIVES_LIBRARIES} ${ALLEGRO_MAIN_LIBRARIES})
