#ifndef KERNEL_MM_REGION_H_
#define KERNEL_MM_REGION_H_

#include <cstring>
#include "kernel/utils/array.h"
#include "kernel/mm/physical_allocator.h"
#include "kernel/mm/shared_ptr.h"

// From here is needed only table entry struct.
// Later will be replaced with arch undependent types
#include "arch/arm64/mm/translation_descriptor.h"

namespace kernel {
namespace mm {

class Region {
 public:
  using Sptr = SharedPointer<Region, SlabAllocator>;

  struct Attributes {
    arch::arm64::mm::MemoryAttr mem_attr;
    arch::arm64::mm::S2AP s2ap;
    arch::arm64::mm::SH sh;
    arch::arm64::mm::AF af;
    arch::arm64::mm::Contiguous contiguous;
    arch::arm64::mm::XN xn;
  };

  std::size_t Length() const { return length_; };

 protected:
  Region(std::size_t length) : length_(length) {}
  Region() = delete;

  std::size_t length_;
};

class DirectRegion : public Region {
 public:
  using Sptr = SharedPointer<DirectRegion, SlabAllocator>;

  DirectRegion(void* begin, std::size_t length)
    : Region(length),
      begin_(begin) {
  }

  ~DirectRegion() {
    LOG(DEBUG) << "~DirectRegion";
  }

  void* Begin() const { return begin_; };

 private:
  void* begin_;
};

class PagedRegion : public Region {
 public:
  using Page = kernel::mm::Page<KERNEL_PAGE_SIZE>;
  using PageContainer = utils::List<Page*, SlabAllocator>;
  using Sptr = SharedPointer<PagedRegion, SlabAllocator>;

  PagedRegion(std::size_t count)
    : Region(count * PageSizeInfo<KERNEL_PAGE_SIZE>::in_bytes), pages_() {
    for (size_t i = 0; i < count; i++) {
      auto page = SlabAllocator<Page>::Allocate();
      LOG(VERBOSE) << "Add page to region: " << page;
      pages_.Push(page);
    }
  }

  ~PagedRegion() {
    LOG(DEBUG) << "~PagedRegion";

    for (auto it = pages_.Begin(); it != pages_.End(); it++) {
      SlabAllocator<Page>::Deallocate(it.Value());
      LOG(VERBOSE) << "Remove page from region: " << it.Value();
    }
  }

  PageContainer& Pages() { return pages_; }

 private:
  PageContainer pages_;
};

}  // namespace mm
}  // namespace kernel

#endif  // KERNEL_MM_ADDRESS_SPACE_H_
