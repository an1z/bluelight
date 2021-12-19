if (TARGET tinyusb_device)
    add_executable(bluelight
            bluelight.c
            )

    # pull in common dependencies
    target_link_libraries(bluelight pico_stdlib)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(bluelight 1)
    pico_enable_stdio_uart(bluelight 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(bluelight)

    # add url via pico_set_program_url
    example_auto_set_url(bluelight)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building bluelight because TinyUSB submodule is not initialized in the SDK")
endif()
