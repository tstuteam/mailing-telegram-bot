# mailing-telegram-bot

## Описание

Телеграмм-бот, который рассылает сообщение всем зарегистрированным пользователям.

## Используемые библиотеки и зависимости

- Компилятор - `g++` или `clang++`
- Система сборки - `Make`
- Telegram Bot API - [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp) (MIT LICENSE)
- Fast C++ logging library - [spdlog](https://github.com/gabime/spdlog) (MIT LICENSE)

## Как собрать и запустить проект

Клонировать проект:

```console
git clone git@github.com:tstuteam/mailing-telegram-bot.git
```

Установить зависимости.

Запустить терминал и написать следующее:

```console
make all
touch users.db
TELEGRAM_BOT_TOKEN=<токен бота> TELEGRAM_BOT_ADMIN_ID=<user id админа> ./bot users.db
```

## Алгоритм работы

1. Пока нет нового сообщения:
   1. Бот регистрирует пользователя в системе.
2. Администратор подготавливает сообщение и отправляет его боту.
3. Бот отправляет сообщение каждому пользователю в системе.
4. Вернуться к пункту 1.
