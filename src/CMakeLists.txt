# Variable definition
## Execute file
set(
    OUTPUT
    # reboot
   keti-redfish
#   keti-redfish1024
)
## Source file
set(
    SOURCE_FILES 
    # reboot.cpp
    main.cpp 
    handler.cpp
    resource.cpp
    util.cpp
    record.cpp
    hwcontrol.cpp
    task.cpp
    chassis_controller.cpp
    TaskServer.cpp
    init.cpp
    lssdp.cpp
    multipart_parser.cpp
    logservice.cpp
    eventservice.cpp
    ethernetinterface.cpp
    ntp.cpp
    certificate.cpp
    smbus.cpp
    fan.cpp
)

# # Directory setting
# set(Boost_INCLUDE_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/boost-1.66.0)
# set(Boost_LIBRARY_DIR ${ROOT_DIR}/output/rockchip_rk3399/build/boost-1.66.0/stage/lib)

# # Linkinking
# ## Header
# include_directories(
#     ${Boost_INCLUDE_DIR} 
# )
# ## Source
# link_directories(
#     ${Boost_LIBRARY_DIR} 
# )

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(deps REQUIRED IMPORTED_TARGET glib-2.0)

# Build
add_executable(${OUTPUT} ${SOURCE_FILES})
target_link_libraries(
    ${OUTPUT} 
    crypto 
    ssl 
    pam
    pam_misc 
    pthread 
    boost_system 
    boost_log 
    boost_log_setup
    boost_thread
    stdc++fs
    cpprest
    sqlite3
    # PkgConfig::deps
)
