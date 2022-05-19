# data-collector
Data_collector for players in Brawl Stars

used libraries: [libpqxx](http://pqxx.org/development/libpqxx/), [jsoncpp](https://github.com/open-source-parsers/jsoncpp), [curl](https://github.com/curl/curl)

# Site Analytic Tool

> Маленький скрипт для получения информации о пользовательской сессии на вашем сайте.

## Содержание

- [Особенности](#особенности)
- [Сборка](#сборка)
- [Установка](#установка)
  - [Пример](#пример)
- [Примеры Backend решений](#примеры-backend-решений)
  - [Fastify](#fastify5-nodejs-веб-фреймворк)

## Сборка

Перед установкой проверьте, что у вас установлен **Node.JS версии 16.13.\*** или выше и пакетный менеджер **Yarn версии 1.22.\*** или выше!

1. Установить локально репозиторий с проектом ()
2. Выполните установку всех dev зависимостей проекта командой `yarn install -D`
3. Измените переменную `SERVER_ADDRESS` в файле `.env` на адрес своего сервера.
4. Соберите проект командой `yarn build`
5. В папке `build` будет создан файл `site-analytic-tool.min.js` с обновленной ссылкой на сервер

## Установка

Файл `site-analytic-tool.min.js` необходимо поместить в `<head>` тег вашего сайта/страницы.
