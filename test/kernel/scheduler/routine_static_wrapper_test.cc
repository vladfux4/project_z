#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "kernel/scheduler/routine_static_wrapper.h"

class RoutineMock {
 public:
  MOCK_METHOD0(Exec, void());
};

class RoutineStaticWrapperTest : public ::testing::Test {
 public:
  typedef kernel::scheduler::RoutineStaticWrapper<RoutineMock> StaticRoutine;

 protected:
  RoutineStaticWrapperTest() {
    mock_ = new (StaticRoutine::GetRoutineLocation()) RoutineMock();
  }

  virtual ~RoutineStaticWrapperTest() {
    mock_->~RoutineMock();
  }

  RoutineMock* mock_;
};

TEST_F(RoutineStaticWrapperTest, Exec) {
  EXPECT_CALL(*mock_, Exec()).Times(1);
  StaticRoutine::Exec();
}
