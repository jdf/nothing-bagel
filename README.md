
# nothingbagel_relay_driver

`main.c` is an ATtiny85-based driver for a latching relay.

## CPU Clock (`F_CPU`)

`F_CPU` is configured as a project-wide build flag in 
`cmake/nothingbagel_relay_driver/default/user.cmake`:

`add_compile_definitions(F_CPU=1000000UL)`

To change the clock speed, edit that value and then reconfigure/build:

`cmake --preset nothingbagel_relay_driver_default_conf -S cmake/nothingbagel_relay_driver/default`

`cmake --build _build/nothingbagel_relay_driver/default`
