include_directories(../../)

set(SOURCE ${SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/arch_start.S
  ${CMAKE_CURRENT_SOURCE_DIR}/exceptions.S
  ${CMAKE_CURRENT_SOURCE_DIR}/mutex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mutex.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/mm/tcr.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/translation_descriptor.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/translation_table.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/mmu.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/mmu.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/address_space.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/timer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/timer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/exceptions.h
  ${CMAKE_CURRENT_SOURCE_DIR}/exceptions.cc

  CACHE INTERNAL "" FORCE
)
