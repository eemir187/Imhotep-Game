#include "network/TestsystemCommunicationBridge.hpp"
#include <cassert>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <format>

TestsystemCommunicationBridge::TestsystemCommunicationBridge(const std::string &id, bool is_owner, bool verbose) 
  : cfg_id_(id.c_str()), cfg_is_owner_(is_owner), cfg_verbose_(verbose), state_(InternalState::NETWORK_SETUP)
{
  assert(std::getenv("TESTING") != nullptr && "TestsystemCommunicationBridge: Usage in non-testing context forbidden!");
  assert(id.starts_with("/imhotep") && "Invalid identifier for server-network!");

  // Owner: Create shm object
  // Non-Owner: Open shm object

  const int posix_shm_flag = cfg_is_owner_ ? O_CREAT | O_RDWR : O_RDWR;
  shm_fd_ = shm_open(cfg_id_, posix_shm_flag, 0666);
  if (shm_fd_ == -1) [[unlikely]]
  {
    throw std::runtime_error(
      std::format(
        "TestsystemCommunicationBridge: Failed to open SHM object {} for {}!",
        cfg_id_, 
        cfg_is_owner_ ? "(owner)" : "(non-owner)"
      )
    );
  }
  else if (cfg_verbose_)
  {
    std::cout << (cfg_is_owner_ ? ". (owner) " : ". (non-owner) ") << " opened " << cfg_id_ << std::endl;
  }

  // Truncate shared memory object

  if (ftruncate(shm_fd_, sizeof(sem_t)) == -1) [[unlikely]]
  {
    close(shm_fd_); 
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to ftruncate SHM object!");
  }

  // Map semaphore process' address space

  shared_semaphore_ = static_cast<sem_t*>(
    mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_, 0)
  );
  if (shared_semaphore_ == MAP_FAILED) [[unlikely]]
  {
    close(shm_fd_);
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to mmap SHM object!");
  }

  // Owner: Initialize semaphore

  if (cfg_is_owner_)
  {
    if (sem_init(shared_semaphore_, 1, 0) == -1) [[unlikely]]
    {
      munmap(shared_semaphore_, sizeof(sem_t));
      close(shm_fd_);
      throw std::runtime_error("TestsystemCommunicationBridge: Failed to initialize semaphore!");
    }
  }
}

TestsystemCommunicationBridge::~TestsystemCommunicationBridge() noexcept
{
  munmap(shared_semaphore_, sizeof(sem_t));
  close(shm_fd_);

  if (cfg_is_owner_)
  {
    sem_destroy(shared_semaphore_);
    shm_unlink(cfg_id_);
  }
}

bool TestsystemCommunicationBridge::wait(std::optional<std::size_t> timeout_s)
{
  if (cfg_verbose_)
    std::cout << (cfg_is_owner_ ? "(owner)" : "(non-owner)") << " waits for input signal!" << std::endl;

  if (!timeout_s.has_value())
  {
    assert(false && "Currently not used - Timeout values should always be provided!");
    if (sem_wait(shared_semaphore_) == -1) [[unlikely]]
        throw std::runtime_error("TestsystemCommunicationBridge:: sem_wait failed!");
    return true;
  }
  else
  {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout_s.value();

    if (sem_timedwait(shared_semaphore_, &ts) == -1)
    {
      if (errno == ETIMEDOUT) [[likely]]
        return false; /* indicate signal was not received */
      else
        throw std::runtime_error("TestsystemCommunicationBridge:: sem_timedwait failed!");
    }
    else
    {
      return true; /* indicate signal was received */
    }
  }
}

void TestsystemCommunicationBridge::signal()
{
  if (cfg_verbose_)
    std::cout << (cfg_is_owner_ ? "(owner)" : "(non-owner)") << " sends signal!" << std::endl;

  // Sanity Check: Logically, we have a binary semaphore (no 2 consecutive signals should appear without wait)

  int curr_sem_val;
  if (sem_getvalue(shared_semaphore_, &curr_sem_val) == -1) [[unlikely]]
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to retrieve semaphore value!");

  assert(curr_sem_val == 0 &&
         "TestsystemCommunicationBridge: LogicError - Consecutive signal without wait on binary semaphore!");

  // Actual signal

  if (sem_post(shared_semaphore_) == -1) [[unlikely]]
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to signal semaphore!");
}

bool TestsystemCommunicationBridge::waitForServerAwaitsInputSignal(std::optional<std::size_t> timeout_s)
{
  assert(state_ == InternalState::MESSAGE_COMMUNICATION);
  return this->wait(timeout_s);
}

bool TestsystemCommunicationBridge::waitForNetworkReadySignal(std::optional<std::size_t> timeout_s)
{
  assert(state_ == InternalState::NETWORK_SETUP);
  return this->wait(timeout_s);
}

void TestsystemCommunicationBridge::signalServerAwaitsInput()
{
  assert(state_ == InternalState::MESSAGE_COMMUNICATION);
  this->signal();
}

void TestsystemCommunicationBridge::signalNetworkReady()
{
  assert(state_ == InternalState::NETWORK_SETUP);
  this->signal();
}

void TestsystemCommunicationBridge::updateState(InternalState new_state)
{
  // Current STM: (init) Network_Setup --> Message_communication

  assert(state_ == InternalState::NETWORK_SETUP);
  assert(new_state == InternalState::MESSAGE_COMMUNICATION);

  // Owner: Might re-initialize semaphore

  if (cfg_is_owner_)
  {
    int curr_val;
    if (sem_getvalue(shared_semaphore_, &curr_val) == -1) [[unlikely]]
      throw std::runtime_error("TestsystemCommunicationBridge: Failed to retrieve semaphore value on updateState!");

    assert(curr_val == 0 && "TestsystemCommunicationBridge - Logic Error: Tried to update with unconsumed signal!");

    /* no semaphore update necessary (is already 0) */
  }

  // Set state indicator
  
  state_ = new_state;
}

void TestsystemCommunicationBridge::reInitialize()
{
  assert(cfg_is_owner_ && "Only owner is allowed to reset semaphore!");

  // Reset state

  this->state_ = InternalState::NETWORK_SETUP;

  // Reset semaphore (by destroying and reinitializing with value 0)
    
  if (cfg_is_owner_)
  {
    if (sem_destroy(shared_semaphore_) == -1) [[unlikely]]
      throw std::runtime_error("TestsystemCommunicationBridge: Failed to initialize semaphore!");
  
    if (sem_init(shared_semaphore_, 1, 0) == -1) [[unlikely]]
      throw std::runtime_error("TestsystemCommunicationBridge: Failed to initialize semaphore!");
  }
}

bool TestsystemCommunicationBridge::existsUnconsumedSignal()
{
  int curr_sem_val;
  if (sem_getvalue(shared_semaphore_, &curr_sem_val) == -1) [[unlikely]]
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to retrieve semaphore value!");

  assert(curr_sem_val == 0 || curr_sem_val == 1); /* binary semaphore! */
  
  return curr_sem_val > 0;
}

void TestsystemCommunicationBridge::assertSignalValue(int value)
{
  int curr_sem_val;
  if (sem_getvalue(shared_semaphore_, &curr_sem_val) == -1) [[unlikely]]
    throw std::runtime_error("TestsystemCommunicationBridge: Failed to retrieve semaphore value!");

  assert(curr_sem_val == value);
}