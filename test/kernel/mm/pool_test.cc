#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "kernel/mm/pool.h"

class PoolTest : public ::testing::Test {
 protected:
  PoolTest() {
  }

  virtual ~PoolTest() {
  }

  void New() {
    uint32_t* ptr = pool.Allocate();
    ASSERT_NE(ptr, nullptr);
    *ptr = index;
    data.push_back({ptr, index});
    index++;
  }

  void DeleteByIndex(size_t index) {
    pool.Deallocate(data[index].first);
    data.erase(data.begin() + index);
  }

  void CheckContent() {
    for (size_t i = 0; i < data.size(); ++i) {
      EXPECT_EQ(data[i].second, *(data[i].first));
    }
  }

  void Print() {
    for (size_t i = 0; i < data.size(); ++i) {
      std::cout << "value:" << data[i].second << ", ptr:" << (data[i].first) << std::endl;
    }
  }

  kernel::mm::Pool<uint32_t, size_t, 32> pool;
  std::vector<std::pair<uint32_t*, uint32_t>> data;
  uint32_t index = 0;
};

TEST_F(PoolTest, Allocation) {
  for (size_t i = 0; i < pool.Size(); ++i) {
    New();
  }

  CheckContent();

  EXPECT_EQ(pool.Allocate(), nullptr);
  EXPECT_EQ(pool.Allocate(), nullptr);
}

TEST_F(PoolTest, Updates) {
  for (size_t i = 0; i < pool.Size(); ++i) {
    New();
  }

  CheckContent();

  for (size_t i = 0; i < 8; ++i) {
    DeleteByIndex(0);
  }

  for (size_t i = 0; i < 16; ++i) {
    DeleteByIndex(7);
  }

  for (size_t i = 0; i < 4; ++i) {
    New();
  }

  CheckContent();

  for (size_t i = 0; i < 8; ++i) {
    DeleteByIndex(3);
  }

  for (size_t i = 0; i < 6; ++i) {
    New();
  }

  CheckContent();

  for (size_t i = 0; i < pool.FreeItems(); ++i) {
    New();
  }

  CheckContent();
}