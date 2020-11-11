install(TARGETS ${APP} 
    RUNTIME DESTINATION bin
)

install(TARGETS ${SO} ${L}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(FILES ${LIBINC_T}
    DESTINATION include/cry
)
