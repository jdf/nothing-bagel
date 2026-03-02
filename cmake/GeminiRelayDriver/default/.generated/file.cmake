# The following variables contains the files used by the different stages of the build process.
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assemble)
set_source_files_properties(${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess)
set_source_files_properties(${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_assembleWithPreprocess})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_compile "${CMAKE_CURRENT_SOURCE_DIR}/../../../main.c")
set_source_files_properties(${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_compile_cpp)
set_source_files_properties(${GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_compile_cpp} PROPERTIES LANGUAGE CXX)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_link)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_objcopy_ihex)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_objcopy_eep)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_objcopy_lss)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_objcopy_srec)
set(GeminiRelayDriver_default_default_AVR_GCC_FILE_TYPE_objcopy_sig)
set(GeminiRelayDriver_default_image_name "default.elf")
set(GeminiRelayDriver_default_image_base_name "default")

# The output directory of the final image.
set(GeminiRelayDriver_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/GeminiRelayDriver")

# The full path to the final image.
set(GeminiRelayDriver_default_full_path_to_image ${GeminiRelayDriver_default_output_dir}/${GeminiRelayDriver_default_image_name})

# Potential output file extensions
set(output_extensions
    .hex
    .lss
    .eep
    .srec
    .usersignatures)
list(TRANSFORM output_extensions PREPEND "${GeminiRelayDriver_default_output_dir}/${GeminiRelayDriver_default_image_base_name}")
