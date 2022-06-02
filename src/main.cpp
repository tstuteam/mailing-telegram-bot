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
#include <csignal>
#include <cstdlib>
#include <iostream>
#include <vector>

int main() {
  TgBot::Bot bot(MailingApp::token);
  std::vector<MailingApp::UserId> users;
  const short max_message_size = 4096;

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    std::string welcome = "Hi! It's a mailing bot!"
                          "Try to register with `/register` command.";
    bot.getApi().sendMessage(message->chat->id, welcome);
  });
  bot.getEvents().onCommand("register", [&users](TgBot::Message::Ptr message) {
    // If user is already registered.
    // It's ugly. Can't be easily understood.
    if (std::find(users.begin(), users.end(), message->from->id) != users.end())
      return;
    // Don't need to register admin.
    if (message->from->id == MailingApp::admin_id)
      return;
    // Otherwise register.
    users.emplace_back(message->from->id);
  });
  bot.getEvents().onAnyMessage([&bot, &users](TgBot::Message::Ptr message) {
    try {
      // if message->photo.get() != nullptr -> {sendPhoto(...)}
      if (StringTools::startsWith(message->text, "/start"))
        return;
      if (StringTools::startsWith(message->text, "/register"))
        return;
      if (message->from->id == MailingApp::admin_id) {
        auto message_length = message->text.length();
        std::cout << "Admin wrote: " << message->text << '\n';
        for (auto user_id : users) {
          // It's ugly.
          if (message_length > max_message_size) {
            std::basic_string<char>::size_type sended_chars = 0;
            for (; sended_chars < message_length; sended_chars += max_message_size) {
              bot.getApi().sendMessage(
                  user_id, message->text.substr(sended_chars, sended_chars + max_message_size));
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
      std::cerr << "EXCEPTION: " << e.what() << '\n';
    }
  });

  // Kill proccess with Ctrl+C.
  signal(SIGINT, [](int s) {
    std::cout << "SIGINT got: " << s << '\n';
    exit(EXIT_SUCCESS);
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

  return EXIT_SUCCESS;
}
