include_directories(../)

add_library (kernel
  hal/mutex_base.h
  hal/mutex.h

  scheduler/routine.h
  scheduler/routine.cc
  scheduler/routine_static_wrapper.h
)
