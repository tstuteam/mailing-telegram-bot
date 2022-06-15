/**
 * @file main.cpp
 * @author Andrey Malov (ndrmlv@ya.ru)
 * @brief This is simple mailing program for telegram.
 * @version 0.1.1
 * @date 2022-06-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <spdlog/spdlog.h>
#include <tgbot/tgbot.h>

#include <algorithm>
#include <cstdlib>
#include <stdexcept>

#include "lib.h"

int main(const int argc, char const *const *argv) {
  if (argc != 2) {
    spdlog::error("Usage: {} <text file>", argv[0]);
    return -1;
  }
  const auto db_path = argv[1];
  const auto token = MailingApp::get_token("TELEGRAM_BOT_TOKEN");
  const auto admin_id = MailingApp::get_admin_id("TELEGRAM_BOT_ADMIN_ID");

  auto users = MailingApp::read_db(db_path);

  TgBot::Bot bot(token);

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
    spdlog::info("user with id - {}: {}", message->chat->id, msg);
    bot.getApi().sendMessage(message->chat->id, msg);
  };
  bot.getEvents().onCommand(
      "start", [&bot](const TgBot::Message::Ptr &message) {
        const std::string welcome = "Hi! It's a mailing bot! "
                                    "Try to register with `/register` command. "
                                    "To unregister type `/unregister`.";
        bot.getApi().sendMessage(message->chat->id, welcome);
      });
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
        try {
          if (message->photo.size() != 0) {
            const auto image_id = message->photo.back()->fileId;
            const auto image_info = bot.getApi().getFile(image_id);
            const auto image_path = image_info->filePath;
            const auto image_size = image_info->fileSize;
            spdlog::info("Downloading image {} from {}", image_path,
                         message->from->id);
            const auto image = bot.getApi().downloadFile(image_path);
            std::ofstream file("images/" + image_id + ".jpg",
                               std::ofstream::binary);
            file.write(image.c_str(), image_size);
            file.close();
          } else {
            spdlog::info("{} wrote: {}", message->from->id, message->text);
            const auto commands = {"/start", "/register", "/unregister"};
            for (auto &&command : commands) {
              if (StringTools::startsWith(message->text, command))
                return;
            }
            if (message->from->id == admin_id) {
              const auto chunks = MailingApp::split_message(message->text);
              for (auto &&user_id : users)
                for (auto &&msg : chunks)
                  bot.getApi().sendMessage(user_id, msg);
            }
          }
        } catch (const TgBot::TgException &e) {
          spdlog::warn("TgBot error: {}", e.what());
        }
      });

  try {
    spdlog::info("Bot username: {}.", bot.getApi().getMe()->username);
    bot.getApi().deleteWebhook();
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      spdlog::debug("Long poll started.");
      longPoll.start();
    }
  } catch (const TgBot::TgException &e) {
    spdlog::critical("TgBot error: {}", e.what());
  }

  return 0;
}
