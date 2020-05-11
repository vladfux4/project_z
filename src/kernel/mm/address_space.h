#ifndef KERNEL_MM_ADDRESS_SPACE_H_
#define KERNEL_MM_ADDRESS_SPACE_H_

#include "arch/arm64/mm/address_space.h"
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

  ~AddressSpace() {
    LOG(DEBUG) << "~AddressSpace";
  }

  template<class RegionType>
  struct RegionJoint {
    void* begin;
    typename RegionType::Sptr region;
    Region::Attributes attr;
  };

  void MapRegion(void* begin, PagedRegion::Sptr& region, const Region::Attributes& attr) {
    arch::arm64::mm::AddressSpace::MapRegion(begin, region, attr);
    paged_regions_.Push({begin, region, attr});
  }

  void MapRegion(void* begin, DirectRegion::Sptr& region, const Region::Attributes& attr) {
    arch::arm64::mm::AddressSpace::MapRegion(begin, region, attr);
    direct_regions_.Push({begin, region, attr});
  }

 private:
  utils::List<RegionJoint<DirectRegion>, SlabAllocator> direct_regions_;
  utils::List<RegionJoint<PagedRegion>, SlabAllocator> paged_regions_;

};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
