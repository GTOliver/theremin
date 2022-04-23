function(copy_leap_library target_name)
    # Copy the Leap shared library to the target's output directory
    # Useful for being able to run executables in the build folder

    get_target_property(
            LEAPC_IMPORTED_CONFIG
            LeapSDK::LeapC
            IMPORTED_CONFIGURATIONS
    )

    get_target_property(
            LEAPC_SHARED_LIB_PATH
            LeapSDK::LeapC
            IMPORTED_LOCATION_${LEAPC_IMPORTED_CONFIG}
    )

    add_custom_command(
            TARGET
            ${target_name}
            POST_BUILD
            COMMAND
            ${CMAKE_COMMAND} -E copy
            ${LEAPC_SHARED_LIB_PATH}
            $<TARGET_FILE_DIR:${target_name}>
    )

endfunction()
