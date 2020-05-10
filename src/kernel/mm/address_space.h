#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/translation_table.h"
#include "kernel/config.h"
#include "kernel/logger.h"
#include "kernel/mm/physical_allocator.h"
#include "kernel/mm/region.h"
#include "kernel/mm/unique_ptr.h"

namespace kernel {
namespace mm {

class AddressSpace : public arch::arm64::mm::AddressSpace {
 public:
  using Uptr = kernel::mm::UniquePointer<AddressSpace, SlabAllocator>;

  void MapRegion(void* begin, PagedRegion::Sptr region, const Region::Attributes& attr) {
    auto& pages = region->Pages();
    auto address = reinterpret_cast<PagedRegion::Page*>(begin);
    for (auto it = pages.Begin(); it != pages.End(); it++, address++) {
      LOG(DEBUG) << "map page v: " << address << " -> p: " << it.Value();

      TranslationTable::EntryParameters params = {
        AddressSpace::TranslationTable::BlockSize::_4KB,
        attr.mem_attr,
        attr.s2ap,
        attr.sh,
        attr.af,
        attr.contiguous,
        attr.xn
      };
      translation_table.Map(address, reinterpret_cast<void*>(it.Value()), params);
    }
  }
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
