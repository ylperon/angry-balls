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
Вкратце это делается так для Linux и Mac OS X.
`$ cd \path\to\jsoncpp\
$ scons platform=linux-gcc`
