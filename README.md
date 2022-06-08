# mailing-telegram-bot

## Описание

Телеграмм-бот, который рассылает сообщение всем зарегистрированным пользователям.

## Используемые библиотеки и зависимости

- Компилятор - `g++` или `clang++`
- Система сборки - `Make`
- Telegram Bot API - [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp) (MIT LICENSE)

## Как собрать и запустить проект

Клонировать проект и все его модули:

```console
git clone --recursive git@github.com:tstuteam/mailing-telegram-bot.git
```

Установить [Docker](https://docs.docker.com/engine/install/).

Изменить в файле `docker-compose.debug.yml` данные [токена бота](https://t.me/botfather) и [id админа](https://t.me/userinfobot).

Запустить терминал и написать следующее:

```console
docker-compose -f docker-compose.debug.yml up
```

## Алгоритм работы

1. Пока нет нового сообщения:
   1. Бот регистрирует пользователя в системе.
2. Администратор подготавливает сообщение и отправляет его боту.
3. Бот отправляет сообщение каждому пользователю в системе.
4. Вернуться к пункту 1.
