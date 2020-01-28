#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/translation_table.h"
#include "kernel/config.h"
#include "kernel/logger.h"
#include "kernel/mm/boot_allocator.h"
#include "kernel/mm/page_pool.h"

namespace kernel {
namespace mm {

template <template <class, size_t> class AllocatorBase>
class AddressSpace {
 public:
  AddressSpace() : translation_table() {}

  using TranslationTable =
      arch::arm64::mm::TranslationTable<KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH,
                                        AllocatorBase>;

  using Page = uint8_t[PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes];

  void MapNewPage(const void* address) {
    using namespace arch::arm64::mm;
    auto page = PhysicalPagePoolAllocator<Page>::Allocate();
    LOG(VERBOSE) << "map page by address: " << page;

    translation_table.Map(
        address, reinterpret_cast<void*>(page),
        {TranslationTable::BlockSize::_4KB, MemoryAttr::NORMAL, S2AP::NORMAL,
         SH::INNER_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});
  }

  TranslationTable translation_table;
};  // namespace mm

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
