cmake_minimum_required(VERSION 3.14)
project(TearDrop VERSION 0.0.1 LANGUAGES CXX)
set(PRO_DIR ${PROJECT_SRC_DIR})
set(PRO_DEP_INC_DIR /usr/local/include)
set(PRO_DEP_LIB_DIR /usr/local/libs)

if(LINUX)
	
endif()

find_package(opengl VERSION 3.1)
find_package(Boost COMPONENTS boost::system boost::program_options)

find_library(FMT PATHS ${PRO_DEP_LIB_DIR} NAMES fmt fmtd)
find_library(SPDLOG PATHS  ${PRO_DEP_LIB_DIR} NAMES  spdlog spdlogd)

if(Boost_FOUND AND FMT AND SPDLOG)
	message("Found dependencies")

else (Boost_FOUND AND FMT AND SPDLOG)
		
endif()

if(opengl_FOUND)

endif()


file(SRCS GLOB ${PRO_DIR}/src/*.cpp)
file(HEADERS GLOB ${PRO_DIR}/incldue/*.h)

add_executable(tear ${SRCS} ${HEADERS})

