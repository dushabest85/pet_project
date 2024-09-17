
//Если не xотим хранить в векторе копии, то можем хранить ссылки на исходный объект
/*
std::vector<std::reference_wrapper<const ColumnMetaData<WorkColumnTypes::Type>>> combineColumns;

for(auto it = template_->beginColumn(); it != template_->endColumn(); ++it)
    boost::range::copy(it->columns, std::back_inserter(combineColumns));
*/

//Динамическое создание статического константного контейнера
/*inline const QHash<QString, Type> enumsFromString = [](){
    QHash<QString, Type> result;

    // Итерация по парам (ключ, значение) из names
    std::for_each(names.keyValueBegin(), names.keyValueEnd(), [&result](const auto &pair)
                  {
                      // Вставка в result: строка как ключ, Type как значение
                      result.insert(pair.second, pair.first);
                  });

    // Возвращаем результат для инициализации const QHash
    return result;
}(); ///< Основные колонки левой таблицы календарного плана*/

/*
class MatWorksDbApi : public DbApiBase {
    using DbApiBase::DbApiBase; // Наследование конструкторов базового класса
};
Преимущества:
Меньше кода: Не нужно вручную писать конструкторы, если они просто передают аргументы в базовый класс.
Чистота и читаемость: Класс становится чище и легче для понимания.
Поддержка в C++11 и выше: Это стало возможным с введением этой функциональности в C++11, что упрощает наследование конструкторов.
Когда это применимо:
Когда конструктор производного класса не добавляет никакой дополнительной логики и все его аргументы используются только для инициализации базового класса.
В таких случаях собственного конструктора для производного класса может вообще не понадобиться.
*/
