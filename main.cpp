#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <tgbot/tgbot.h>
#include <vector>

int main() {
  TgBot::Bot bot("5246372133:AAH3XvfTmCJIywDsL4acJZcGu7F8suep4DY");
  using UserId = int64_t;
  std::vector<UserId> users;
  const UserId admin_id = 379420471;
  const short max_message_size = 4096;

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    bot.getApi().sendMessage(
        message->chat->id,
        "Hi! It's a mailing bot! Try to register with `/register` command.");
  });
  bot.getEvents().onCommand("register", [&bot,
                                         &users](TgBot::Message::Ptr message) {
    if (std::find(users.begin(), users.end(), message->from->id) != users.end())
      return;
    if (message->from->id == admin_id)
      return;
    users.emplace_back(message->from->id);
  });
  bot.getEvents().onAnyMessage([&bot, &users](TgBot::Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/start"))
      return;
    if (StringTools::startsWith(message->text, "/register"))
      return;
    if (message->from->id == admin_id) {
      auto message_length = message->text.length();
      printf("Admin wrote: %s\n", message->text.c_str());
      for (auto user_id : users) {
        if (message_length > max_message_size) {
          std::basic_string<char>::size_type i = 0;
          for (; i < message_length; i += max_message_size) {
            bot.getApi().sendMessage(
                user_id, message->text.substr(i, i + max_message_size));
          }
          if (i != message_length) {
            bot.getApi().sendMessage(user_id,
                                     message->text.substr(message_length - i));
          }
        } else {
          bot.getApi().sendMessage(user_id, message->text);
        }
      }
    }
  });

  signal(SIGINT, [](int s) {
    printf("SIGINT got\n");
    exit(EXIT_SUCCESS);
  });

  try {
    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
      printf("Long poll started\n");
      longPoll.start();
    }
  } catch (TgBot::TgException &e) {
    printf("error: %s\n", e.what());
  }

  return EXIT_SUCCESS;
}
