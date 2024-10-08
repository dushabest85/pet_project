/**
 * Полезные функции языка
 **/

/*
 * Этот цикл проходит по каждому элементу (начиная со второго) из контейнера records, используя std::span,
 * который предоставляет "вид" на элементы без их копирования.

 * std::vector<int> vec;
 * for (const auto &row : std::span{qAsConst(records)}.subspan(1))
 * {
 * }
 */

/*
 * "ASC" — это строка в обычной кодировке (обычно UTF-8 или ASCII).
 * u"ASC" — это строка, закодированная в формате UTF-16.

 * Если вы работаете только с ASCII-символами, это может не иметь большого значения, но для поддержки международных символов или для интеграции с
 * системами, использующими UTF-16, это важно.
 */


