# Трехмерное Отображение Электронного Документа
<<<<<<< HEAD

## Описание Проекта
Цель проекта - создание визуализации электронного документа, чтобы он выглядел как настоящий лист бумаги в трехмерном пространстве. Это включает в себя визуализацию документа, его освещение и настройку виртуальной камеры для просмотра с разных сторон.

### Исходная Точка
Исходный код взят с [VTK Examples](https://examples.vtk.org/site/Cxx/Tutorial/Tutorial_Step6/).

## Шаги Реализации

### Шаг 1: Загрузка и Визуализация 3D Mesh
- **Замена vtkConeSource на Mesh Loader**: Загрузка 3D mesh (vtkOBJReader или vtkSTLReader).
- **Настройка vtkPolyDataMapper**: Отображение загруженной 3D mesh.

### Шаг 2: Натяжение Текстуры
- **Загрузка Текстуры**: Использование vtkTexture для загрузки изображения документа.
- **Применение Текстуры**: Настройка vtkActor для использования текстуры.

### Шаг 3: Настройка Сцены
- **Настройка Камеры**: Установка позиции и параметров камеры через vtkCamera.
- **Настройка Освещения**: Добавление и настройка источников света с помощью vtkLight.

### Шаг 4: Рендеринг и Определение Границ
- **Рендеринг Сцены**: Использование vtkRenderWindow и vtkRenderer.
- **Определение Границ**: Применение vtkBoundaryEdges для визуализации границ mesh.

### Шаг 5: Экспорт Результата
- **Снимок Сцены**: Создание снимка с помощью vtkWindowToImageFilter.
- **Экспорт Данных**: Экспорт данных в требуемом формате.


