include_directories(../../)

set(SOURCE ${SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/arch_start.s
  ${CMAKE_CURRENT_SOURCE_DIR}/mutex.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mutex.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/mm/translation_table.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/translation_table.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/mmu.h
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/mmu.cc

  CACHE INTERNAL "" FORCE
)
