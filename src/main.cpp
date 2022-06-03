/**
 * @file main.cpp
 * @author Andrey Malov (ndrmlv@ya.ru)
 * @brief This is simple mailing program for telegram.
 * @version 0.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "lib.h"

#include <tgbot/tgbot.h>

#include <algorithm>
#include <stdexcept>

int main(const int argc, char const *const *argv) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0]
              << " <bot token> <admin id> <text file>\n";
    return -1;
  }
  const std::string token = argv[1];
  const MailingApp::UserId admin_id = std::stoll(argv[2], nullptr);
  const std::string db_path = argv[3];
  const std::set<MailingApp::Command> commands = {"start", "register",
                                                  "unregister"};

  TgBot::Bot bot(token);
  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    const std::string welcome = "Hi! It's a mailing bot!"
                                "Try to register with `/register` command.";
    bot.getApi().sendMessage(message->chat->id, welcome);
  });

  const auto users_or_null = MailingApp::read_db(db_path);
  try {
    if (!users_or_null.has_value()) {
      throw std::invalid_argument("Check your corrupted DB");
    }
  } catch (const std::exception &e) {
    std::cerr << "DB ERROR: " << e.what() << '\n';
    return -1;
  }
  auto users = users_or_null.value();
  bot.getEvents().onCommand(
      "register", [&users, &db_path, &admin_id](TgBot::Message::Ptr message) {
        if (message->from->id == admin_id)
          return;
        if (users.insert(message->from->id).second)
          MailingApp::update_db(db_path, users);
      });
  bot.getEvents().onCommand(
      "unregister", [&users, &db_path, &admin_id](TgBot::Message::Ptr message) {
        if (message->from->id == admin_id)
          return;
        if (users.erase(message->from->id) != 0)
          MailingApp::update_db(db_path, users);
      });

  bot.getEvents().onAnyMessage([&bot, &users, &commands,
                                &admin_id](TgBot::Message::Ptr message) {
    std::cout << "User with ID " << message->from->id << " wrote message\n";
    try {
      // if (message->photo.get() != nullptr) { sendPhoto(...) }
      for (auto &&command : commands) {
        if (StringTools::startsWith(message->text, "/" + command))
          return;
      }
      if (message->from->id == admin_id) {
        const short max_message_length = 4096;
        auto message_length = message->text.length();
        std::cout << "Admin wrote: " << message->text << '\n';
        for (auto &&user_id : users) {
          // It's ugly.
          if (message_length > max_message_length) {
            std::basic_string<char>::size_type sended_chars = 0;
            for (; sended_chars < message_length;
                 sended_chars += max_message_length) {
              bot.getApi().sendMessage(
                  user_id,
                  message->text.substr(sended_chars,
                                       sended_chars + max_message_length));
            }
            if (sended_chars != message_length) {
              bot.getApi().sendMessage(
                  user_id, message->text.substr(message_length - sended_chars));
            }
          } else {
            bot.getApi().sendMessage(user_id, message->text);
          }
        }
      }
    } catch (const TgBot::TgException &e) {
      std::cerr << "TgBot EXCEPTION: " << e.what() << '\n';
    }
  });

  try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username << '\n';
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      std::cout << "Long poll started\n";
      longPoll.start();
    }
  } catch (const TgBot::TgException &e) {
    std::cerr << "error: " << e.what() << '\n';
  }

  return 0;
}
