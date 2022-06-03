# mailing-telegram-bot

## Описание

Телеграмм-бот, который рассылает сообщение всем зарегистрированным пользователям.

## Используемые библиотеки и зависимости

- Компилятор - `g++` или `clang++`
- Система сборки - `Make`
- Telegram Bot API - [tgbot-cpp](https://github.com/reo7sp/tgbot-cpp)

## Как собрать и запустить проект

Установить все зависимости, компилятор.

Запустить терминал и написать следующее:

```console
make all
./bot <token> <admin id> <path to existing empty file>
```

## Алгоритм работы

1. Пока нет нового сообщения:
   1. Бот регистрирует пользователя в системе.
2. Администратор подготавливает сообщение и отправляет его боту.
3. Бот отправляет сообщение каждому пользователю в системе.
4. Вернуться к пункту 1.
