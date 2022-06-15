#include "lib.h"

#include <spdlog/spdlog.h>

#include <iterator>

std::vector<std::string> MailingApp::split_message(const std::string &message,
                                                   size_t max) {
  if (message.length() < max)
    return std::vector<std::string>{message};
  return std::vector<std::string>{
      message.substr(0, max), split_message(message.substr(max), max).back()};
}

std::set<MailingApp::UserId> MailingApp::read_db(const std::string &path) {
  std::ifstream file(path);
  std::vector<double> content(std::istream_iterator<double>{file}, {});
  std::set<UserId> result;
  std::copy(content.begin(), content.end(),
            std::inserter(result, result.begin()));
  return result;
}

void MailingApp::update_db(const std::string &path,
                           const std::set<UserId> &users) {
  std::ofstream file(path);
  std::vector<UserId> content(users.begin(), users.end());
  std::ostream_iterator<UserId> output_iterator(file, "\n");
  std::copy(content.begin(), content.end(), output_iterator);
}

std::string MailingApp::get_token(const std::string &variable) {
  std::string result{};
  try {
    if (const char *env_p = std::getenv(variable.c_str()))
      result = env_p;
    else
      throw std::logic_error("can't find the environment variable");
  } catch (const std::logic_error &e) {
    spdlog::critical("{} is unset in OS: {}.", variable, e.what());
  }
  return result;
}

MailingApp::UserId MailingApp::get_admin_id(const std::string &variable) {
  UserId result{};
  try {
    result = std::stoll(get_token(variable), nullptr);
  } catch (const std::invalid_argument &e) {
    spdlog::critical("no conversion could be performed: {}.", e.what());
  } catch (const std::out_of_range &e) {
    spdlog::critical("the converted value would fall out of the range of the "
                     "result type or if the underlying function (std::strtol "
                     "or std::strtoll) sets errno to ERANGE.: {}.",
                     e.what());
  }
  return result;
}