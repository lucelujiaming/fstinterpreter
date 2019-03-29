cmake_minimum_required(VERSION 2.8.3)
project(prg_interpreter)


find_package(catkin REQUIRED COMPONENTS 
    log_manager
#    io_manager
#    motion_plan
    parameter_manager
#    mem_share
    motion_control
)
set(VERSION_MAJOR ?)
set(VERSION_MINOR ?)
set(VERSION_PATCH ?)
string(TIMESTAMP BUILD_TIME \"%Y-%m-%d|%H:%M:%S\")

set(${PROJECT_NAME}_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

configure_file(
    "${PROJECT_SOURCE_DIR}/include/version.h.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_version.h" 
)

file(WRITE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_version
    "${PROJECT_NAME}:${${PROJECT_NAME}_VERSION}\n"
)

if(catkin_CROSS)
	set(catkin_INCLUDE_DIRS ${catkin_INCLUDE_DIRS_CROSS})
    set(catkin_LIBRARIES ${catkin_LIBRARYS_CROSS})
    message(STATUS "ARM-Cross")
else()
	set(catkin_INCLUDE_DIRS ${catkin_INCLUDE_DIRS_x86})
    set(catkin_LIBRARIES ${catkin_LIBRARYS_x86})
    message(STATUS "x86-64")
endif()

set(catkin_INCLUDE_DIRS
    ${catkin_INCLUDE_DIRS}

    ${depend_PATH_INC}/parameter_manager/include
    ${depend_PATH_INC}/log_manager/include
    ${depend_PATH_INC}/ioboard/include
    ${depend_PATH_INC}/io_manager/include
    ${depend_PATH_INC}/mem_share/include
    ${depend_PATH_INC}/tp_interface/include
    ${depend_PATH_INC}/tp_interface/include/proto
    ${depend_PATH_INC}/prg_interpreter/include/reg_manager
#    ${depend_PATH_INC}/prg_interpreter/include/reg_shmi
    ${depend_PATH_INC}/thread_help/include 
    ${depend_PATH_INC}/common/
    ${depend_PATH_INC}/reg_manager/include 
    ${depend_PATH_INC}/fst_io_device/include 
    ${depend_PATH_INC}/base_device/include
    ${depend_PATH_INC}/process_comm/include 
    ${depend_PATH_INC}/modbus_manager/include
    ${depend_PATH_INC}/motion_control/include 
    ${depend_PATH_INC}/io_mapping/include 
    ${depend_PATH_INC}/basic_alg/include 
    ${depend_PATH_INC}/nvram/include
    ${depend_PATH_INC}/fst_spi/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wwrite-strings -fpermissive -O0")

catkin_package(
#    DEPENDS motion_plan 
    DEPENDS fst_io_device 
    DEPENDS motion_control 
    DEPENDS basic_alg 
    DEPENDS nvram 
    DEPENDS fst_spi 
)
 

include_directories(
    include
    ${catkin_INCLUDE_DIRS}
    ${PROJECT_BINARY_DIR}
)

###########
## Build ##
###########
add_executable(prg_interpreter
    src/reg_manager/reg_manager_interface_wrapper.cpp
    src/reg_manager/forsight_registers_manager.cpp
    src/forsight_innercmd.cpp
    src/forsight_xml_reader.cpp
    src/forsight_int_main.cpp
    src/forsight_basint.cpp
    src/forsight_innerfunc.cpp
    src/forsight_inter_control.cpp
    src/forsight_io_controller.cpp
    src/forsight_program_property.cpp
    src/forsight_cJSON.cpp
    src/launch_code_mgr.cpp
    src/forsight_home_pose.cpp
    src/forsight_external_resource.cpp
#   src/io_interface.cpp
)

target_link_libraries(prg_interpreter
    ${catkin_LIBRARIES}
#    io_manager
    parameter_manager
    xml2
#    motion_plan
    process_comm 
    thread_help
    pthread
    modbus_manager
    logger
    basic_alg
)

#############
## Install ##
#############
install(TARGETS prg_interpreter
    RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY config
    DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_version
    DESTINATION ${CATKIN_GLOBAL_SHARE_DESTINATION}/version
)

install(FILES config/programs_path.yaml
    DESTINATION ${CATKIN_GLOBAL_SHARE_DESTINATION}/configuration/machine
)

install(FILES config/user_defined_variable.xml
    DESTINATION ${CATKIN_GLOBAL_SHARE_DESTINATION}/configuration/machine
)

install(FILES config/prg_interpreter_config.yaml
    DESTINATION ${CATKIN_GLOBAL_SHARE_DESTINATION}/configuration/machine
)

install(FILES config/io_emulator.json
    DESTINATION ${CATKIN_GLOBAL_SHARE_DESTINATION}/configuration/configurable
)

