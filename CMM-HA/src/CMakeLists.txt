# Variable definition
## Execute file
set(
    OUTPUT
    CMM-HA
)
## Source file
set(
    SOURCE_FILES 
    KETI_halmp.cpp 
    main.cpp
    KETIHA.cpp
    KETIHA_Sync.cpp
    KETI_Helper.cpp
    KETIhaNetService.cpp
    handler.cpp
    util.cpp
    KETI_Switch.cpp
    TaskServer.cpp
)


# Directory setting
set(Boost_INCLUDE_DIR ${ROOT_DIR}/output/build/boost-1.74.0)
set(Boost_LIBRARY_DIR ${ROOT_DIR}/CMM_SDK/output/build/boost-1.74.0/stage/lib)

# Linkinking
## Header
include_directories(
    ${Boost_INCLUDE_DIR} 
)
## Source
link_directories(
    ${Boost_LIBRARY_DIR} 
)

# Build
add_executable(${OUTPUT} ${SOURCE_FILES})
target_link_libraries(
    ${OUTPUT} 
    crypto 
    ssl
    pam
    pam_misc
    cpprest 
    pthread 
    boost_system 
    boost_log 
    boost_log_setup
	boost_system
    boost_thread
    ${PROJECT_SOURCE_DIR}/xml2json/libxml2json.a
)
