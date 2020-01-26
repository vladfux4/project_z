#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/translation_table.h"
#include "kernel/config.h"
#include "kernel/mm/boot_allocator.h"

namespace kernel {
namespace mm {

template <template <class, size_t> class AllocatorBase>
class AddressSpace {
 public:
  AddressSpace() : translation_table() {}

  using TranslationTable =
      arch::arm64::mm::TranslationTable<KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH,
                                        AllocatorBase>;
  TranslationTable translation_table;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
