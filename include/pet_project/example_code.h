
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
// Наследование конструкторов базового класса
                        class MatWorksDbApi : public DbApiBase {
                            using DbApiBase::DbApiBase;
                        };
                        Преимущества:
                        Меньше кода: Не нужно вручную писать конструкторы, если они просто передают аргументы в базовый класс.
                        Чистота и читаемость: Класс становится чище и легче для понимания.
                        Поддержка в C++11 и выше: Это стало возможным с введением этой функциональности в C++11, что упрощает наследование конструкторов.
                        Когда это применимо:
                        Когда конструктор производного класса не добавляет никакой дополнительной логики и все его аргументы используются только для инициализации базового класса.
                        В таких случаях собственного конструктора для производного класса может вообще не понадобиться.
*/

/*
// std::apply — это функция из библиотеки C++17, которая позволяет "развернуть" кортеж и передать его элементы в качестве аргументов функции.
                     * Это полезно, когда у вас есть кортеж значений, и вы хотите передать эти значения функции в виде отдельных аргументов.
                     *         const auto defaultArguments = std::tuple{rec.value("id"),
                                                                     rec.value("parent_id"),
                                                                     rec.value("item_sub_type"),
                                                                     rec.value("column_status"),
                                                                     rec.value("column_is_show")};

                            const auto isInvalid = std::apply([](const auto &...arg){return ((arg.isNull()) || ... );}, defaultArguments);

                            Упрощение кода: Использование std::apply может сделать ваш код более читаемым и понятным, особенно когда вы работаете с
                            функциями высшего порядка или функциональным стилем программирования.

                            Пример использования:
                            #include <iostream>
                            #include <tuple>
                            #include <functional>

                            void printSum(int a, int b) {
                                std::cout << "Sum: " << a + b << std::endl;
                            }

                            int main() {
                                auto args = std::make_tuple(5, 3);

                                // Используем std::apply для вызова printSum с аргументами из кортежа
                                std::apply(printSum, args);

                                return 0;
                            }
*/
/*
 * std::visit — это функция из C++17, которая позволяет применить лямбда-выражение или другой вызываемый объект ко всем возможным вариантам,
 * хранящимся в std::variant. Она удобно используется для обработки значений разных типов, хранящихся в одном std::variant.
 *
                           std::variant<int, float, std::string> var;

                            var = 10;  // Присваиваем значение типа int

                            // Используем std::visit для обработки значения в зависимости от его типа
                            std::visit([](auto&& value) {
                                // auto&& позволяет принять любой тип, хранящийся в variant
                                using T = std::decay_t<decltype(value)>; // Определяем реальный тип
                                if constexpr (std::is_same_v<T, int>) {
                                    std::cout << "Это int со значением: " << value << '\n';
                                } else if constexpr (std::is_same_v<T, float>) {
                                    std::cout << "Это float со значением: " << value << '\n';
                                } else if constexpr (std::is_same_v<T, std::string>) {
                                    std::cout << "Это string со значением: " << value << '\n';
                                }
                            }, var);
 */
