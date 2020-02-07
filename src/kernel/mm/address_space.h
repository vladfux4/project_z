#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/translation_table.h"
#include "kernel/config.h"
#include "kernel/logger.h"
#include "kernel/mm/physical_allocator.h"

namespace kernel {
namespace mm {

template <template <class, size_t> class AllocatorBase>
class PhysicalAddressSpaceBase
    : public arch::arm64::mm::PhysicalAddressSpace<
          KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH, AllocatorBase> {};

template <template <class, size_t> class AllocatorBase>
class VirtualAddressSpaceBase
    : public arch::arm64::mm::VirtualAddressSpace<
          KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH, AllocatorBase> {
 public:
  using PageType = kernel::mm::Page<KERNEL_PAGE_SIZE>;
  using TranslationTable = typename arch::arm64::mm::VirtualAddressSpace<
      KERNEL_PAGE_SIZE, KERNEL_ADDRESS_LENGTH, AllocatorBase>::TranslationTable;

  void* MapNewPage(const void* address) {
    using namespace arch::arm64::mm;
    auto page = PhysicalAllocator<PageType>::Allocate();
    LOG(DEBUG) << "map new page v: " << address << " -> p: " << page;

    this->translation_table.Map(
        address, reinterpret_cast<void*>(page),
        {TranslationTable::BlockSize::_4KB, MemoryAttr::NORMAL, S2AP::NORMAL,
         SH::INNER_SHAREABLE, AF::IGNORE, Contiguous::OFF, XN::EXECUTE});
    return page;
  }
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
