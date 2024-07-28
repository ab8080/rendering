# Three-Dimensional Display of an Electronic Document

## Problem Statement
The input consists of a 3D mesh and a scanned image of a document. The task is to generate an image based on the description (rendering), i.e., map the image onto the 3D mesh and render it. The result should include the rendered image and reference information in some form.

### Starting Point
The source code is taken from [VTK Examples](https://examples.vtk.org/site/Cxx/Tutorial/Tutorial_Step6/).

## Implementation Steps

### Step 1: Loading and Visualizing the 3D Mesh
- **Replacing vtkConeSource with Mesh Loader**: Load the 3D mesh using vtkOBJReader or vtkSTLReader.
- **Configuring vtkPolyDataMapper**: Display the loaded 3D mesh.

### Step 2: Applying the Texture
- **Loading the Texture**: Use vtkTexture to load the document image.
- **Applying the Texture**: Configure vtkActor to use the texture.

### Step 3: Setting up the Scene
- **Configuring the Camera**: Set the camera position and parameters using vtkCamera.
- **Configuring the Lighting**: Add and configure light sources using vtkLight.

### Step 4: Rendering and Determining Boundaries
- **Rendering the Scene**: Use vtkRenderWindow and vtkRenderer.
- **Determining Boundaries**: Use vtkBoundaryEdges to visualize the mesh boundaries.

### Step 5: Exporting the Result
- **Capturing the Scene**: Create a snapshot using vtkWindowToImageFilter.
- **Exporting Data**: Export the data in the required format.

## Results
The document is rendered. It can be zoomed in/out and rotated.
![alt text](https://github.com/ab8080/rendering/blob/main/screenshot.png)

The original image can be viewed [here](https://github.com/ab8080/rendering/blob/main/chess.jpeg)

Reference information is saved in the file mesh.vtp.

# Трехмерное Отображение Электронного Документа

## Постановка задачи
На вход подается 3d сетка и отсканированное изображение документа. Нужно сгенерировать изображение по описанию (рендеринг), то есть натянуть изображение на 3d сетку и отрендерить это. В результате работы нужно отрендерить изображение и в каком-то виде сообщить эталонную информацию.

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

## Результаты
Отрендерен документ. Его можно приближать/отдалять и крутить. 
![alt text](https://github.com/ab8080/rendering/blob/main/screenshot.png)

Исходное изображение можно посмотреть [тут](https://github.com/ab8080/rendering/blob/main/chess.jpeg)

Эталонная информация сохраняется в файл mesh.vtp.

