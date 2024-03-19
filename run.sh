#!/bin/bash

# Создаем папку для штрих-кодов, если она еще не существует
mkdir -p barcodes

# Вызываем Python скрипт для генерации штрих-кодов
python3 generate.py

# Переходим к директории C++ проекта (измените на ваш путь)
cd .

mkdir build
cd build

# Собираем C++ проект
cmake ..
make

# Переходим обратно в директорию со скриптом
cd -

# Путь к исполняемому файлу C++ программы (измените на ваш путь)
executable="./build/Tutorial_Step6"

# Путь к папке с фонами
backgrounds_path="photos/"

# Цикл по всем файлам штрих-кодов в папке barcodes
for barcode in barcodes/*.jpg; do
    # Предполагаем, что фон для каждого штрих-кода один и тот же, замените на правильный путь
    background="${backgrounds_path}IMG_20230529_151304_487.jpg"

    echo "currently in"
    realpath .
    # Вызываем C++ программу с аргументами штрих-кода и фона
    $executable $barcode $background

    # Здесь можно добавить логику сохранения или обработки результатов
done

# Удаляем папку с штрих-кодами
rm -rf barcodes

rm -rf build

echo "Обработка завершена."

