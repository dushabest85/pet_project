cmake_minimum_required(VERSION 3.10)
project(pet_project VERSION 1.0 LANGUAGES CXX)

# Устанавливаем стандарт C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Указываем директорию для сборки
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Указываем исходные файлы
set(SOURCE_FILES
    src/main.cpp
)

# Указываем заголовочные файлы
set(HEADER_FILES
    include/pet_project/binary_search.h
    include/pet_project/boost_timer.h
    include/pet_project/coroutine_example.h
    include/pet_project/example_code.h
    include/pet_project/factorial.h
    include/pet_project/redis_cache.h
    include/pet_project/redis_stream.h
    include/pet_project/selection_sorting.h
)

# Создаем исполняемый файл
add_executable(pet_project ${SOURCE_FILES} ${HEADER_FILES})
