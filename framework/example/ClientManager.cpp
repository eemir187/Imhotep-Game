#include "network/Client.hpp"
#include "network/Exceptions.hpp"
#include "toml++/toml.h"

#include <algorithm>
#include <cassert>
#include <format>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <span>
#include <sstream>
#include <string>
#include <thread>
#include <cstdlib>

struct ANSI_COLOR final
{
  static constexpr const char *RED_C = "\033[1;31m";
  static constexpr const char *GREEN_C = "\033[1;32m";
  static constexpr const char *NC = "\033[0m";
};

struct Command final
{
  unsigned player;
  std::string message;
};

class TOMLData final
{
private:
  /**
   * Parsed TOML file content [result of toml::parse_toml(std::string) call].
   * It will be used to fetch the 
   */
  const toml::table test_file_;

public:
  std::vector<Command> commands;
  std::vector<std::string> expected;
  int number_of_clients{};

  /**
   * Parse string for given entry.
   */
  const std::string& parseStringAttribute(const std::string& field_name) const
  {
    auto found = test_file_.find(field_name);
    if (found == test_file_.cend()) [[unlikely]]
      throw toml::parse_error(("req. attribute <" + field_name + "> not present!").c_str(), test_file_.source());
 
    const toml::value<std::string> *str_node = found->second.as_string();
    if (str_node == nullptr) [[unlikely]]
    {
      throw toml::parse_error(
        ("req. attribute <" + field_name + "> is not string!").c_str(), 
        found->second.source()
      );
    }

    return str_node->get();
  }

  /**
   * Parse array of [int, string] for entry <commands>.
   */
  const std::vector<Command> parseCommandsAttribute() const
  {
    auto found = this->test_file_.find("commands");
    if (found == test_file_.cend())
      throw toml::parse_error("req. attribute <commands> not present!", test_file_.source());

    const toml::array *command_array = found->second.as_array();
    if (command_array == nullptr)
      throw toml::parse_error("req. attribute <commands> is not array!", test_file_.source());

    std::vector<Command> output;
    for (std::size_t i = 0; i < command_array->size(); i++)
    {
      const toml::array *command = command_array->get(i)->as_array();
      if (command == nullptr)
      {
        throw toml::parse_error(
          ("req. attribute <commands> is not array of array (entry " + std::to_string(i) + " )!").c_str(),
          command_array->get(i)->source()
        );
      }

      if (command->size() != 2) [[unlikely]]
      {
        throw toml::parse_error(
          ("req. attribute <commands> does not fulfill format [ [player: int, command: str], ...] (entry " 
            + std::to_string(i) + " )!").c_str(),
          command_array->get(i)->source()
        );
      }
      
      auto player = command->get(0)->as_integer();
      if (!player) [[unlikely]]
      {
        throw toml::parse_error(
          ("req. attribute <commands> does not fulfill format [ [player: int, command: str], ...]" 
            "(entry: commands[" + std::to_string(i) +"][0] is not int)!").c_str(),
          command->get(0)->source()
        );
      }
      
      auto cmd_str = command->get(1)->as_string();
      if (!cmd_str) [[unlikely]]
      {
        throw toml::parse_error(
          ("req. attribute <commands> does not fulfill format [ [player: int, command: str], ...]" 
            "(entry: commands[" + std::to_string(i) +"][0] is not string)!").c_str(),
          command->get(1)->source()
        );
      }
   
      output.push_back(
        Command { 
          .player = static_cast<unsigned int>(player->get()), 
          .message = cmd_str->get() + '\n' 
        });
    }

    return output;
  }
  
  /**
   * Parse array of strings (entry <expected>).
   */
  std::vector<std::string> parseExpectedAttribute() const
  {
    auto found = test_file_.find("expected");
    if (found == test_file_.cend()) [[unlikely]]
      throw toml::parse_error("req. attribute <expected> not present!", test_file_.source());

    const toml::array *expected_array = found->second.as_array();
    if (expected_array == nullptr) [[unlikely]]
      throw toml::parse_error("req. attribute <expected> is not array!", found->second.source());
    
    std::vector<std::string> output;
    for (std::size_t i = 0; i < expected_array->size(); i++)
    {
      const toml::value<std::string> *line = expected_array->get(i)->as_string();
      if (line == nullptr) [[unlikely]]
      {
        throw toml::parse_error(
          ("req. attribute <expected> is not array of strings (entry " + std::to_string(i) + ")!").c_str(), 
          expected_array->get(i)->source());
      }

      output.push_back(line->get());
    }

    return output;
  }

  int parseIntAttribute(const std::string& input) const
  {
    auto found = this->test_file_.find(input);
    if (found == this->test_file_.end()) [[unlikely]]
      throw toml::parse_error(("req. attribute" + input + " not present!").c_str(), test_file_.source());

    const toml::value<int64_t>* returnValue = found->second.as_integer();
    if (returnValue == nullptr) [[unlikely]]
      throw toml::parse_error(("req. attribute " + input + " is not an integer!").c_str(), found->second.source());

    if (returnValue->get() > std::numeric_limits<int>::max()) [[unlikely]]
      throw toml::parse_error(("req.attribute " + input + " exceeds size of integer!").c_str(), found->second.source());

    return static_cast<int>(returnValue->get());
  }

public:  
  /**
   * Regular Constructor.
   * 
   * @param local_id: Identifier for testcase (name of toml file, without path), i.e. `testcase09`.
   * @param test_file: Parsed TOML file (result of toml::parse_file).
   */
  explicit TOMLData(const toml::table&& test_file)
  : test_file_(std::move(test_file)),
    commands(parseCommandsAttribute()),
    expected(parseExpectedAttribute()),
    number_of_clients(parseIntAttribute("numberOfClients"))
  {}
  
  /**
   * Move Constructor.
   */
  explicit TOMLData(TOMLData&& other)
  : test_file_(std::move(other.test_file_)),
    commands(std::move(other.commands)),
    expected(std::move(other.expected)),
    number_of_clients(other.number_of_clients)
  {}
  
  /* Avoid unnecessary copying (only use move constructor) */

  TOMLData() = delete;
  TOMLData(const TOMLData&) = delete;
  TOMLData& operator=(const TOMLData&) = delete;
};

// ----------------------------------------------------------------------------------------------------------------- //

class ClientSender final
{
private:
  std::vector<std::unique_ptr<net::Client>> clients;
  std::size_t received_counter;
  std::size_t sent_counter;

public:
  ClientSender(std::size_t clients_count) 
   : received_counter(0), sent_counter(0)
  {
    try 
    {
      for (std::size_t i = 0; i < clients_count; i++)
        clients.emplace_back(std::make_unique<net::Client>("TestBot_" + std::to_string(i + 1))); // might throw
    }
    catch (const net::NetworkException &e)
    {
      std::cerr << (clients.size() == 0 ?
                        "Hmm, seems you did not start a (listening) server instance before calling this script." :
                        "Hmm, maybe your server's client connections and TOML file's numberOfClients"
                        "doesn't match (or your server already crashed)?")
                << std::endl;
      throw e;
    }
  }

  void send(int sender_id, const std::string& content) // might throw NetworkException (+ bad_alloc)
  {
    if (sender_id > static_cast<int>(clients.size())) [[unlikely]]
    {
      std::cerr << std::format(
                       "{}FATAL ERROR{}: TOML file seems semantically invalid (sender id exceeds client count)",
                       ANSI_COLOR::RED_C,
                       ANSI_COLOR::NC)
                << std::endl;
      exit(-1);
    }

    std::cout << std::format("[{}] >> {}", sender_id, content) << std::endl;
    clients.at(sender_id)->send(content);
    sent_counter++;
  }

  std::string receiveMessage() // might throw NetworkException (+ bad_alloc)
  {
    // All clients receive the same message anyway, so we only use the first client
    std::string msg = clients.at(0)->awaitServerMessage();
    received_counter++;
    return msg;
  }

  std::size_t getReceivedCounter() const noexcept
  {
    return received_counter;
  }

  std::size_t receivedSentCounter() const noexcept
  {
    return sent_counter;
  }
};

// ----------------------------------------------------------------------------------------------------------------- //

static void printReceived(const std::string& received, const std::string& expected)
{
  auto is_equal_f = [](const std::string& a, const std::string& b) -> bool 
  {
    if (a.size() != b.size())
      return false;

    for (std::size_t i = 0; i < a.size(); i++)
      if (::tolower(a.at(i)) != ::tolower(b.at(i)))
        return false;

    return true;
  };

  if (is_equal_f(received, expected))
  {
    std::cout << std::format("<< {}{}{}", ANSI_COLOR::GREEN_C, received, ANSI_COLOR::NC) << std::endl;
  }
  else
  {
    std::cout
        << std::format("<< {}{}{} (Expected <{}>)", ANSI_COLOR::RED_C, received, ANSI_COLOR::NC, expected)
        << std::endl;
  }
}

// ----------------------------------------------------------------------------------------------------------------- //

int main(int argc, char **argv)
{
  if (argc != 2) [[unlikely]]
  {
    std::cerr << "Usage: ./client_manager <PATH-TO-TOML-FILE>\n" 
              << "  -> i.e. `./client_manager ../testsystem/tests/ass0/public/test01.toml`"
              << std::endl;
    return 1;
  }

  std::fstream file_test(argv[1]);
  if (!file_test.is_open() || !file_test.good()) [[unlikely]]
  {
    std::cerr << std::format("{}FATAL ERROR{}: No accessable toml file in provided path found!", 
                             ANSI_COLOR::RED_C, ANSI_COLOR::NC)
              << std::endl;
    return 2;
  }
  file_test.close();

  try
  {
    const TOMLData data(toml::parse_file(argv[1]));

    ClientSender cs(data.number_of_clients);

    std::size_t curr_expected_idx = 0;
    std::mutex console_out_lock;

    auto send_message_f = [&]() -> void
    {
      for (std::size_t i = 0; i < data.commands.size(); i++)
      {
        std::cin.get();
        console_out_lock.lock();
        {
          cs.send(data.commands.at(i).player, data.commands.at(i).message); // might throw
        }
        console_out_lock.unlock();
        std::cin.clear();
      }
    };

    auto receive_message_f = [&]() -> void
    {
      while (true)
      {
        std::string received_msg = cs.receiveMessage(); // might throw
        while (received_msg.back() != '\n')  [[unlikely]]
          received_msg += cs.receiveMessage();

        received_msg.pop_back(); // Remove '\n' at end
        
        console_out_lock.lock();
        {
          if (received_msg == "Quit") [[unlikely]] // happens only 1x at end
          {
            for (std::size_t i = cs.getReceivedCounter(); i < data.expected.size(); i++)
              printReceived("[NOTHING]", data.expected.at(i));
            
            std::cout << "---------------------------------------------------------------" << std::endl
                      << "Testcase End Successfully => Terminate Client ..." << std::endl;

            console_out_lock.unlock();
            exit(0);
          }

          printReceived(
            std::move(received_msg),
            curr_expected_idx < data.expected.size() ? data.expected.at(curr_expected_idx++) : "[NOTHING]"
          );
        }
        console_out_lock.unlock();
      }
    };

    std::thread send_thread(send_message_f);
    receive_message_f();

    return 0;
  }
  catch (const toml::parse_error& e)
  {
    std::cerr << std::format("{}FATAL ERROR{}: Parsing of TOML file failed.\n", ANSI_COLOR::RED_C, ANSI_COLOR::NC);
    std::cerr << "  => " << e.what() << std::endl;
    return 1;
  }
  catch (const net::TimeoutException &e) // should not happen
  {
    std::cerr << std::format("{}FATAL ERROR{}: Something unexpected went wrong.\n", ANSI_COLOR::RED_C, ANSI_COLOR::NC);
    std::cerr << "  => " << e.what() << std::endl;
    return 2;
  }
  catch (const net::NetworkException& e)
  {
    std::cerr << std::format("{}ERROR{}: Server did not start or disconnected!\n", ANSI_COLOR::RED_C, ANSI_COLOR::NC);
    std::cerr << "  => " << e.what() << std::endl;
    return 3;
  }
  catch (const std::runtime_error &e)
  {
    std::cerr << std::format("{}FATAL ERROR{}: Something unexpected went wrong.\n", ANSI_COLOR::RED_C, ANSI_COLOR::NC);
    std::cerr << "  => " << e.what() << std::endl;
    return 4;
  }
}