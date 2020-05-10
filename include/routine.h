namespace z {
namespace kernel {
namespace virtualization {

/**
 * \brief The routine class
 *
 * Represents the part of code runnable in loop.
 * Should contain storage for processor specific context
 */
class Routine {
 public:
  /**
   * \brief Routine state
   */
  enum class State : uint8_t {
    kCreated,
    kRunning,
    kPending,
    kException,
    kExited,
  };

  /**
   * \brief Constructor
   *
   * \param address_space Reference on address space object, that would be used by routine.
   */
  Routine(AddressSpace& address_space);
};

} // namespace virtualization
} // namespace kernel
} // namespace z
