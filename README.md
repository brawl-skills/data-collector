# data-collector
>Data_collector for players in Brawl Stars

used libraries: [libpqxx](http://pqxx.org/development/libpqxx/), [jsoncpp](https://github.com/open-source-parsers/jsoncpp), [curl](https://github.com/curl/curl)

## Содержание

- [Сборка](#сборка)
- [Установка](#установка)
  - [Пример](#пример)
- [Запуск](#запуск)
 
  

## Сборка

Перед установкой проверьте, что у вас установлены библиотеки **Jsoncpp \***, **curl \*** и **pqxx \***, а так же  **glibc.\*** и **cmake.\*** !

1. Установить локально репозиторий с проектом 

2.собрать проект с помощью cmake 

## Пример

```bash
git clone https://github.com/brawl-skills/data-collector.git
cd data-collector
cmake --build ./cmake-build-debug --target data_collector -- -j 9
cmake --build ./cmake-build-debug --target players_collector -- -j 9
```

## Запуск
написать в консоли (./data_collector "api-token") или (./players_collector "api-token").
