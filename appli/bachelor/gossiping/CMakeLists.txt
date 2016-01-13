include_directories(${PROJECT_SOURCE_DIR}/appli/iotlab/lib)

if(${PLATFORM_HAS_CSMA})
add_executable(gossiping main network gossiping cache)
target_link_libraries(gossiping platform mac_csma iotlab_uid_num_table)
endif(${PLATFORM_HAS_CSMA})