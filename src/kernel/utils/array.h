#ifndef KERNEL_UTILS_ARRAY_H_
#define KERNEL_UTILS_ARRAY_H_

#include <cstdint>

namespace utils {

template <typename T, template <class, std::size_t> class AllocatorBase, std::size_t kAlignment = 0>
class List {
 public:
  struct Node {
    Node() : next(nullptr), buffer() {}

    Node* next;
    uint8_t buffer[sizeof(T)];
  };

  using Allocator = AllocatorBase<Node, kAlignment>;

  class Iterator {
   public:
    Iterator(Node* node) : node_(node) {}

    Iterator& operator++(int) {
      node_ = node_->next;
      return *this;
    }

    bool operator!=(const Iterator& obj) const {
      return (obj.node_ != node_);
    }

    T& Value() { return *reinterpret_cast<T*>(node_->buffer); }

   private:
    Node* node_;
  };

  List() : head_(nullptr), tail_(nullptr), size_(0) {}

  ~List() {
    auto node = head_;
    while (node != nullptr) {
      reinterpret_cast<T*>(node->buffer)->~T();
      Allocator::Deallocate(node);
      node = node->next;
    }
  }

  void Push(const T& item) {
    Node* node = new (Allocator::Allocate()) Node();
    new (node->buffer) T(item);

    if (head_ == nullptr) {
      head_ = node;
      tail_ = head_;
    } else {
      tail_->next = node;
      tail_ = node;
    }

    size_++;
  }

  Iterator Begin() {
    return Iterator(head_);
  }

  Iterator End() {
    return Iterator(nullptr);
  }

  List(const List&) = delete;
  List& operator=(const List& other) = delete;

  std::size_t size() const { return size_; }

 private:
  Node* head_;
  Node* tail_;
  std::size_t size_;
};

}  // namespace utils

#endif  // KERNEL_UTILS_ARRAY_H_
