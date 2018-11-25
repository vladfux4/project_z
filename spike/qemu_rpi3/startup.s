
.global stack_top

.globl _start
_start:
  ldr x0, =stack_top
  mov sp, x0
  b c_entry
