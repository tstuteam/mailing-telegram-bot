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
  const std::set<MailingApp::Command> commands = {"start", "register",
                                                  "unregister"};

  TgBot::Bot bot(token);

  TgBot::InlineKeyboardMarkup::Ptr keyboard(new TgBot::InlineKeyboardMarkup);
  std::vector<TgBot::InlineKeyboardButton::Ptr> row0;
  TgBot::InlineKeyboardButton::Ptr registerButton(
      new TgBot::InlineKeyboardButton);
  registerButton->text = "register";
  registerButton->callbackData = "register";
  TgBot::InlineKeyboardButton::Ptr unregisterButton(
      new TgBot::InlineKeyboardButton);
  unregisterButton->text = "unregister";
  unregisterButton->callbackData = "unregister";
  row0.push_back(registerButton);
  row0.push_back(unregisterButton);
  keyboard->inlineKeyboard.push_back(row0);

  bot.getEvents().onCommand("start", [&bot, &keyboard](
                                         const TgBot::Message::Ptr &message) {
    const std::string welcome = "Hi! It's a mailing bot! "
                                "Try to register with `/register` command.";
    bot.getApi().sendMessage(message->chat->id, welcome, false, 0, keyboard);
  });

  const auto users_or_null = MailingApp::read_db(db_path);
  try {
    if (!users_or_null.has_value()) {
      throw std::invalid_argument("Check your corrupted DB");
    }
  } catch (const std::exception &e) {
    std::cerr << "DB EXCEPTION: " << e.what() << '\n';
    return -1;
  }
  auto users = users_or_null.value();
  bot.getEvents().onCommand(
      "register", [&bot, &users, &db_path, &admin_id,
                   &keyboard](const TgBot::Message::Ptr &message) {
        std::string response{"registered"};
        if (message->from->id == admin_id) {
          response = "admin can't register to the system.";
        } else if (!users.insert(message->from->id).second) {
          response = "you're already registered to the system.";
        } else {
          response = "you're successfully registered to the system.";
        }
        MailingApp::update_db(db_path, users);
        bot.getApi().sendMessage(message->chat->id, response, false, 0,
                                 keyboard, "Markdown");
      });
  bot.getEvents().onCommand(
      "unregister", [&bot, &users, &db_path, &admin_id,
                     &keyboard](const TgBot::Message::Ptr &message) {
        std::string response{"unregistered"};
        if (message->from->id == admin_id) {
          response = "admin can't unregister from the system.";
        } else if (users.erase(message->from->id) == 0) {
          response = "you're already unregistered from the system.";
        } else {
          response = "you're successfully unregistered from the system.";
        }
        MailingApp::update_db(db_path, users);
        bot.getApi().sendMessage(message->chat->id, response, false, 0,
                                 keyboard, "Markdown");
      });
  bot.getEvents().onCallbackQuery(
      [&bot, &users, &db_path, &admin_id,
       &keyboard](const TgBot::CallbackQuery::Ptr &query) {
        if (StringTools::startsWith(query->data, "register")) {
          std::string response{"registered"};
          if (query->from->id == admin_id) {
            response = "admin can't register to the system.";
          } else if (!users.insert(query->from->id).second) {
            response = "you're already registered to the system.";
          } else {
            response = "you're successfully registered to the system.";
          }
          MailingApp::update_db(db_path, users);
          bot.getApi().sendMessage(query->message->chat->id, response, false, 0,
                                   keyboard, "Markdown");
        }
      });
  bot.getEvents().onCallbackQuery(
      [&bot, &users, &db_path, &admin_id,
       &keyboard](const TgBot::CallbackQuery::Ptr &query) {
        if (StringTools::startsWith(query->data, "unregister")) {
          std::string response{"unregistered"};
          if (query->from->id == admin_id) {
            response = "admin can't unregister from the system.";
          } else if (users.erase(query->from->id) == 0) {
            response = "you're already unregistered from the system.";
          } else {
            response = "you're successfully unregistered from the system.";
          }
          MailingApp::update_db(db_path, users);
          bot.getApi().sendMessage(query->message->chat->id, response, false, 0,
                                   keyboard, "Markdown");
        }
      });
  bot.getEvents().onAnyMessage(
      [&bot, &users, &commands, &admin_id](const TgBot::Message::Ptr &message) {
        std::cout << "User with ID " << message->from->id << " send message\n";
        try {
          // if (message->photo.get() != nullptr) { sendPhoto(...) }
          for (auto &&command : commands) {
            if (StringTools::startsWith(message->text, "/" + command))
              return;
          }
          if (message->from->id == admin_id) {
            std::cout << "Admin wrote: " << message->text << '\n';
            for (auto &&user_id : users) {
              for (auto &&msg : MailingApp::split_message(message->text)) {
                bot.getApi().sendMessage(user_id, msg);
              }
            }
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
