#pragma once

#include <stdexcept>
#include <string>

/**
 * DEAR STUDENTS,
 * 
 * THIS HEADER FILE IS MEANT TO BE INCLUDED FOR ALL ASSIGNMENTS.
 * NOTE: YOU DON'T HAVE TO THROW THIS EXCEPTION CLASSES, BUT ONLY CATCH THEM.
 * 
 * SINCERELY,
 * YOUR TUTORS
 */

namespace net
{
struct TimeoutException final : public std::runtime_error
{
  TimeoutException() : std::runtime_error("Timeout") {}
};

struct NetworkException final : public std::runtime_error
{
  NetworkException() : std::runtime_error("Network failure") {}

  explicit NetworkException(std::string message) : std::runtime_error(message) {}
};
} // namespace net
