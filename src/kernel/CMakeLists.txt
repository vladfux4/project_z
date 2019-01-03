include_directories(../)

set(SOURCE ${SOURCE}
  ${CMAKE_CURRENT_SOURCE_DIR}/hal/mutex_base.h
  ${CMAKE_CURRENT_SOURCE_DIR}/hal/mutex.h

  ${CMAKE_CURRENT_SOURCE_DIR}/scheduler/routine.h
  ${CMAKE_CURRENT_SOURCE_DIR}/scheduler/routine.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/scheduler/routine_static_wrapper.h

  ${CMAKE_CURRENT_SOURCE_DIR}/mm/boot_allocator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/memory.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/mm/page_registry.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/kernel.h
  ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/logger.h
  ${CMAKE_CURRENT_SOURCE_DIR}/logger.cc

  CACHE INTERNAL "" FORCE
)
