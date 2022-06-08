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

#include <tgbot/tgbot.h>

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

#include "lib.h"

int main(const int argc, char const *const *argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <text file>\n";
    return -1;
  }

  const std::string db_path = argv[1];
  const std::string token(std::getenv("TELEGRAM_BOT_TOKEN"));
  const MailingApp::UserId admin_id =
      std::stoll(std::getenv("TELEGRAM_BOT_ADMIN_ID"), nullptr);

  TgBot::Bot bot(token);

  bot.getEvents().onCommand(
      "start", [&bot](const TgBot::Message::Ptr &message) {
        const std::string welcome = "Hi! It's a mailing bot! "
                                    "Try to register with `/register` command. "
                                    "To unregister type `/unregister`.";
        bot.getApi().sendMessage(message->chat->id, welcome);
      });

  auto users = MailingApp::read_db(db_path);

  auto handle_registering = [&bot, &users, &db_path,
                             &admin_id](const TgBot::Message::Ptr &message,
                                        const std::string &response,
                                        const bool &callback_check) {
    std::string msg;
    if (message->from->id == admin_id) {
      msg = "admin can't be " + response + ".";
    } else if (callback_check) {
      msg = "you're already " + response + ".";
    } else {
      msg = "you're successfully " + response + ".";
      MailingApp::update_db(db_path, users);
    }
    bot.getApi().sendMessage(message->chat->id, msg);
  };

  bot.getEvents().onCommand(
      "register",
      [&bot, &users, &handle_registering](const TgBot::Message::Ptr &message) {
        handle_registering(message, "registered",
                           !users.insert(message->from->id).second);
      });
  bot.getEvents().onCommand(
      "unregister",
      [&bot, &users, &handle_registering](const TgBot::Message::Ptr &message) {
        handle_registering(message, "unregistered",
                           users.erase(message->from->id) == 0);
      });
  bot.getEvents().onAnyMessage(
      [&bot, &users, &admin_id](const TgBot::Message::Ptr &message) {
        std::cout << "User with ID " << message->from->id << " send message\n";
        try {
          // if (message->photo.get() != nullptr) { sendPhoto(...) }
          auto commands = bot.getApi().getMyCommands();
          for (auto &&command : commands) {
            if (StringTools::startsWith(message->text, "/" + command->command))
              return;
          }
          if (message->from->id == admin_id) {
            std::cout << "Admin wrote: " << message->text << '\n';
            auto chunks = MailingApp::split_message(message->text);
            for (auto &&user_id : users)
              for (auto &&msg : chunks)
                bot.getApi().sendMessage(user_id, msg);
          }
        } catch (const TgBot::TgException &e) {
          std::cerr << "TgBot EXCEPTION: " << e.what() << '\n';
        }
      });

  try {
    std::cout << "Bot username: " << bot.getApi().getMe()->username << '\n';
    bot.getApi().deleteWebhook();
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
