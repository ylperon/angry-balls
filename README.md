# Angry Balls

## Code conventions
Мы используем [Google C++ Styleguide](http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml
) за несколькими исключениями:

- Длина строки не должна превышать 100 символов 
- Для отступа мы используем 4 пробела, табуляция запрещена
- Мы используем `#pragma once` вместо define guard
- Разрешено использовать исключения

## JsonCpp
Для сборки [JsonCpp](http://jsoncpp.sourceforge.net/) потребуется установить [scons](http://www.scons.org/):

- `apt-get install scons` под Linux
- `brew install scons` под Mac OS

Вопрос на стековерфлоу про то, как собрать JsonCpp под линукс [link](http://stackoverflow.com/questions/4628922/building-jsoncpp-linux-an-instruction-for-us-mere-mortals).

Вкратце это делается так для Linux и Mac OS X:

`$ cd \path\to\jsoncpp\`

`$ scons platform=linux-gcc`

## Сетевой протокол

Сообщения, передаваемые по сети будут иметь следующий формат: `|message_length|message|`.

Здесь `message_length` имеет длину 4 байта и указывает длину в байтах `message`.

Сам `message` будет представлять из себя [JSON](http://www.json.org/).

В каждом `message` должно быть поле `type`, которое определяет тип сообщения.

Сообщения могут быть следующих типов:

* `CLI_SUB_REQUEST`
* `CLI_SUB_RESULT`
* `VIEW_SUB_REQUEST`
* `VIEW_SUB_RESULT`
* `STATE`
* `TURN`

### `CLI_SUB_REQUEST`

Это сообщение от клиента к серверу с заявкой на участие.

    {
        "type" : "CLI_SUB_REQUEST"
    }

### `CLI_SUB_RESULT`

Это сообщение от сервера к клиенту в ответ `CLI_SUB_REQUEST`.

Если сервер не смог подключить клиента, то придет такое сообщение:

    {
        "type" : "CLI_SUB_RESULT",
        "result" : "fail"
    }

Если сервер подключил клиента, то придет такое сообщение:

    {
        "type" : "CLI_SUB_RESULT",
        "result" : "ok",
        "id" : "<uint32>"
    }

Где в поле `id` будет указан идентификационный номер клиента.

### `VIEW_SUB_REQUEST`

Это сообщение от вьюера к серверу с заявкой на подписку.

    {
        "type" : "VIEW_SUB_REQUEST"
    }

### `VIEW_SUB_RESULT`

Это сообщение от сервера ко вьюеру в ответ на `VIEW_SUB_REQUEST`.

Если сервер не смог подключить вьюер, то придет такое сообщение:

    {
        "type" : "VIEW_SUB_RESULT",
        "result" : "fail"
    }

Если подключение было удачным, то придет такое сообщение:

    {
        "type" : "VIEW_SUB_RESULT",
        "result" : "ok",
        "id" : "<uint32>"
    }

Где в поле `id` будет указан идентицикационный номер вьюера.

### `STATE`

Это сообщение приходит от сервера к клиентам и вьюерам:

    {
        "type" : "STATE",
        "state_id" : "<uint64>",
        "field_radius" : "<float>",
        "player_radius" : "<float>",
        "coin_radius" : "<float>",
        "time_delta" : "<float>",
        "velocity_max" : "<float>",
        "players" : [
            { "id" : "<uint32>", "x" : "<float>", "y" : "<float>", "v_x" : "<float>", "v_y" : "<float>", "score" : "<float>" },
            ...
            { "id" : "<uint32>", "x" : "<float>", "y" : "<float>", "v_x" : "<float>", "v_y" : "<float>", "score" : "<float>" }
        ],
        "coins" : [
            { "x" : "<float>", "y" : "<float>", "value" : "float" },
            ...
            { "x" : "<float>", "y" : "<float>", "value" : "float" }
        ]
    }

### `TURN`

Это сообщение приходит от клиента к серверу в ответ на сообщение `STATE`.

    {
        "type" : "TURN",
        "state_id" : "<uint32>",
        "id" : "<uint32>",
        "a_x" : "<float>",
        "a_y" : "<float>"
    }
