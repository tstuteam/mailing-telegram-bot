#include "lib.h"

std::vector<std::string> MailingApp::split_message(const std::string message,
                                                   const int max) {
  if (message.length() == 0)
    return std::vector<std::string>();
  return std::vector<std::string>{
      message.substr(0, max), split_message(message.substr(max), max).back()};
}

std::optional<std::set<MailingApp::UserId>>
MailingApp::read_db(const std::string path) {
  std::set<MailingApp::UserId> result;
  std::string line;
  std::ifstream file(path);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      if (line.size() == 0)
        continue;
      std::size_t pos{};
      try {
        const MailingApp::UserId buffer = std::stoll(line, &pos);
        result.insert(buffer);
      } catch (const std::invalid_argument &e) {
        file.close();
        std::cerr << "no conversion could be performed: " << e.what() << '\n';
        return std::nullopt;
      } catch (const std::out_of_range &e) {
        file.close();
        std::cerr << "converted value would fall out of the range of the "
                     "result type or if the underlying function: "
                  << e.what() << '\n';
        return std::nullopt;
      }
    }
  } else {
    file.close();
    return std::nullopt;
  }
  file.close();
  return std::optional<std::set<MailingApp::UserId>>(result);
}

void MailingApp::update_db(const std::string path,
                           const std::set<UserId> users) {
  std::ofstream file(path);
  if (file.is_open()) {
    for (auto &&user : users) {
      file << user << '\n';
    }
  }
  file.close();
}