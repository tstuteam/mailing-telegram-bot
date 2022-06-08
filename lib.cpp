#include "lib.h"

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