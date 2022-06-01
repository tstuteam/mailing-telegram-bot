#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <tgbot/tgbot.h>
#include <vector>

const int64_t admin_id = 379420471;

int main() {
  TgBot::Bot bot("5246372133:AAH3XvfTmCJIywDsL4acJZcGu7F8suep4DY");
  std::vector<int64_t> users;

  bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
    bot.getApi().sendMessage(message->chat->id, "Hi!");
  });
  bot.getEvents().onCommand("register",
                            [&bot, &users](TgBot::Message::Ptr message) {
                              if (std::find(users.begin(), users.end(),
                                            message->from->id) != users.end()) {
                                return;
                              } else {
                                if (message->from->id == admin_id)
                                  return;
                                else {
                                  users.emplace_back(message->from->id);
                                }
                              }
                            });
  bot.getEvents().onAnyMessage([&bot, &users](TgBot::Message::Ptr message) {
    if (StringTools::startsWith(message->text, "/start")) {
      return;
    }
    if (StringTools::startsWith(message->text, "/register")) {
      return;
    }
    if (message->from->id == admin_id) {
      printf("Admin wrote: %s\n", message->text.c_str());
      for (auto user_id : users) {
        bot.getApi().sendMessage(user_id, message->text);
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
