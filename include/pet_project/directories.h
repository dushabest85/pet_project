#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    // Путь к директории и файлу
    fs::path dirPath = std::filesystem::current_path() / "downloads";
    fs::path filePath = dirPath / "file.txt";

    std::cout<<dirPath<<"\n";

    // Проверяем, существует ли директория
    if (!fs::exists(dirPath)) {
        std::cout << "Директория не существует. Создаём..." << std::endl;
        // Создаём директорию
        if (fs::create_directories(dirPath)) {
            std::cout << "Директория создана." << std::endl;
        } else {
            std::cerr << "Ошибка создания директории!" << std::endl;
            return 1; // Выход при ошибке
        }
    } else {
        std::cout << "Директория существует." << std::endl;
        // Проверяем, существует ли файл
        if (fs::exists(filePath)) {
            std::cout << "Файл существует. Удаляем..." << std::endl;
            // Удаляем файл
            if (fs::remove(filePath)) {
                std::cout << "Файл удалён." << std::endl;
            } else {
                std::cerr << "Ошибка при удалении файла!" << std::endl;
                return 1; // Выход при ошибке
            }
        } else {
            std::cout << "Файл не найден." << std::endl;
        }
    }

    return 0;
}
