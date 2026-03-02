# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/Users/jdf/MPLABProjects/GeminiRelayDriver/out/GeminiRelayDriver/default.eep"
  "/Users/jdf/MPLABProjects/GeminiRelayDriver/out/GeminiRelayDriver/default.hex"
  "/Users/jdf/MPLABProjects/GeminiRelayDriver/out/GeminiRelayDriver/default.lss"
  "/Users/jdf/MPLABProjects/GeminiRelayDriver/out/GeminiRelayDriver/default.srec"
  "/Users/jdf/MPLABProjects/GeminiRelayDriver/out/GeminiRelayDriver/default.usersignatures"
  )
endif()
