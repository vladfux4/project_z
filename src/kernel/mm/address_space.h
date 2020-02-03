#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/translation_table.h"
#include "kernel/config.h"
#include "kernel/logger.h"
#include "kernel/mm/physical_allocator.h"

namespace kernel {
namespace mm {

template <template <class, size_t> class AllocatorBase>
class AddressSpace {
 public:
  using PageType = kernel::mm::Page<KERNEL_PAGE_SIZE>;
  using TranslationTable =
      arch::arm64::mm::TranslationTable<KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH,
                                        AllocatorBase>;

  void* MapNewPage(const void* address) {
    using namespace arch::arm64::mm;
    auto page = PhysicalAllocator<PageType, sizeof (PageType)>::Allocate();
//		for (size_t i = 0;i < sizeof (PageType); i++) {
//			page->data[i] = 0;
//		}

    LOG(DEBUG) << "map page v: " << address << " -> p: " << page;

    translation_table.Map(
        address, reinterpret_cast<void*>(page),
        {TranslationTable::BlockSize::_4KB, MemoryAttr::NORMAL, S2AP::NORMAL,
         SH::INNER_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});
		return page;
  }

  TranslationTable translation_table;
};  // namespace mm

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
