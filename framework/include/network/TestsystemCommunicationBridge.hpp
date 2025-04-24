/**
 * Dear students,
 * DON'T TOUCH THIS FILE AND DON'T INCLUDE IT IN ANY OF YOUR CODE.
 */
#pragma once
#include <semaphore.h>
#include <string>
#include <optional>

/**
 * Bridge for meta data about current server state (i.e. if waiting, ...)
 * CURRENTLY: Uses POSIX (plattform-dependent).
 */
class TestsystemCommunicationBridge final
{
public:
  /**
   * Internal State of communication Bridge.
   *  - NETWORK_SETUP: Semaphore indicates whether network setup is ready.
   *  - MESSAGE_COMMUNICATION: Semaphore indicates message waiting state.
   */
  enum class InternalState
  {
    NETWORK_SETUP, /* initial; signal indicates: port file is available */
    MESSAGE_COMMUNICATION /* signal indicates: server awaits input */
  };

private:
  /**
   * Identifier for shared memory object.
   * Usually: /imhotep
   */
  const char *const cfg_id_;

  /**
   * Information whether the sahred network memory object is owned.
   * Context: Owner should be testsystem (create and destory shared memory object).
   */
  const bool cfg_is_owner_;

  /**
   * Flag whether print is enabled.
   */
  const bool cfg_verbose_;

  /**
   * Shared memory file descriptor for shm object on filesystem.
   */
  int shm_fd_;

  /**
   * Pointer to shared memory sempahore (that is mapped within address space). 
   * - Signal sender: Server application to communicate readyness for [context-dependent, specified by `state`].
   * - Signal consumer: Testsystem that waits for [context-dependent, specified by `state`].
   *
   * NOTE: This is a POSIX semaphore, C++ std::binary_semaphore is not guaranteed to fulfill process-safety.
   */
  sem_t *shared_semaphore_;

  /**
   * Internal state representation (determines contextual usage of semaphore).
   */
  InternalState state_;

public:
  TestsystemCommunicationBridge(const TestsystemCommunicationBridge&) = delete;
  TestsystemCommunicationBridge(TestsystemCommunicationBridge&&) = delete;

  /**
   * Initialized shared memory part used for server network state communication between testsystem and server app.
   *
   * @throws std::runtime_error If any shared memory operation fails.
   * @param id Unique identifier for server network (imhotep_server_[PORT])
   * @param is_owner Information about whether semaphore should be allocated and deallocated.
   */
  explicit TestsystemCommunicationBridge(const std::string &id, bool is_owner, bool verbose = false);

  /**
   * Cleanup: unmaps and closes fd, owner also deletes and removes shared memory object.
   */
  ~TestsystemCommunicationBridge() noexcept;

private:

  /**
   * Waits for the signal (optionally: for a given time frame specified by the parameter).
   * 
   * @throws std::runtime_error If semaphore operation fails for some reason (si.e. due to ystem issues, logic error).
   * @param timeout_s (optional) The time frame to wait for the signal in seconds before function returns.
   * @return true if signal was received, false if timeout occured
   */
  [[nodiscard]] bool wait(std::optional<std::size_t> timeout_s = std::nullopt);

  /**
   * Sends signal to indicate that server awaits now input.
   * 
   * @throws std::runtime_error If semaphore operation fails for some reason (si.e. due to ystem issues, logic error).
   */
  void signal();

public:

  /**
   * Waits for signal from server up to a specified amount of time. 
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   * @param timeout_s: Maximum time to wait for signal in seconds.
   * @return True if signal was received, else false.
   */
  [[nodiscard]] bool waitForServerAwaitsInputSignal(std::optional<std::size_t> timeout_s = std::nullopt);

  /**
   * Waits for signal from server up to a specified amount of time.
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   * @param timeout_s: Maximum time to wait for signal in seconds.
   * @return True if signal was received, else false.
   */
  [[nodiscard]] bool waitForNetworkReadySignal(std::optional<std::size_t> timeout_s = std::nullopt);

  /**
   * Sends signal to indicate that the server awaits input.
   * Sender: Target server application.
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   */
  void signalServerAwaitsInput();

  /**
   * Sends signal to indicate that the network setup is ready (= port file is available!).
   * Sender: Target server application.
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   */
  void signalNetworkReady();

  /**
   * Updates internal contextual state that determines semantics for semaphore.
   * See documentation of `InternalState`.
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   * @param new_state: New state to set.
   */
  void updateState(InternalState new_state);

  /**
   * Method that resets all internals of TCB (but not shared memory object creation, as it is resource expensive).
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   */
  void reInitialize();

  /**
   * Returns whether an unconsumed signal of the current state context exists.
   * 
   * @throws std::runtime_error If semaphore operation fail for some reason (should not happen, except system issues).
   */
  [[nodiscard]] bool existsUnconsumedSignal();

  /**
   * For Debugging only!
   */
  void assertSignalValue(int value);
};