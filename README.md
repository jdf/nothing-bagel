
# nothingbagel_driver

`main.c` is an ATtiny85-based driver for a CD74HC4053 analog switch.

## CPU Clock (`F_CPU`)

`F_CPU` is configured as a project-wide build flag in 
`cmake/nothingbagel_switch_driver/default/user.cmake`:

`add_compile_definitions(F_CPU=1000000UL)`

To change the clock speed, edit that value and then reconfigure/build:

`cmake --preset nothingbagel_switch_driver_default_conf -S cmake/nothingbagel_switch_driver/default`

`cmake --build _build/nothingbagel_switch_driver/default`
