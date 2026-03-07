
# nothingbagel_relay_driver

## Structure

| Path                                 | Purpose                                                                                                                             |
|--------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------|
| _build                               | The [CMake build tree](https://cmake.org/cmake/help/latest/manual/cmake.1.html#introduction-to-cmake-buildsystems), can be deleted. |
| cmake                                | Generated [CMake](https://cmake.org/) files. May be deleted if user.cmake has not been added                                        |
| .vscode                              | See [VSCode](https://code.visualstudio.com/docs/getstarted/settings)                                                                |
| .vscode/settings.json                | Workspace specific settings                                                                                                         |
| .vscode/nothingbagel_relay_driver.mplab.json | The MPLAB project file, should not be deleted                                                                                       |
| out                                  | Final build artifacts                                                                                                               |

## CPU Clock (`F_CPU`)

`F_CPU` is configured as a project-wide build flag in 
`cmake/nothingbagel_relay_driver/default/user.cmake`:

`add_compile_definitions(F_CPU=1000000UL)`

To change the clock speed, edit that value and then reconfigure/build:

`cmake --preset nothingbagel_relay_driver_default_conf -S cmake/nothingbagel_relay_driver/default`

`cmake --build _build/nothingbagel_relay_driver/default`
