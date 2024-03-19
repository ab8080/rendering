#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkFeatureEdges.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkLight.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPNGWriter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTransform.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkCylinderSource.h>
#include <vtkPlaneSource.h>
#include <sstream>
#include <iostream>
#include <random>

namespace {
    class vtkMyCallback : public vtkCommand { // vtkCommand — это базовый класс для всех callback'ов в VTK,
        // который предоставляет интерфейс для выполнения действий в ответ на определенные события.
    public:
        static vtkMyCallback* New() { //  VTK использует свою систему управления памятью и часто требует определения
            // такого метода для правильного создания объектов.
            return new vtkMyCallback;
        }
        void Execute(vtkObject* caller, unsigned long, void*) override { // Перегруженный метод Execute, который будет
            // вызван, когда срабатывает событие, к которому vtkMyCallback подписан

            vtkNew<vtkTransform> t; // используется для хранения информации о трансформации (позиции, повороте, масштабе)

            auto widget = reinterpret_cast<vtkBoxWidget*>(caller); // vtkBoxWidget позволяет
            // интерактивно манипулировать объектом (перемещать, вращать, масштабировать) в 3D пространстве.

            widget->GetTransform(t); // извлекается текущая трансформация - процесс изменения позиции, ориентации или размера объекта

            widget->GetProp3D()->SetUserTransform(t); // трансформация применяется к связанному с ним 3D объекту,
            // то есть изменения, заданные в vtkTransform фактически применяются к объекту, изменяя его положение, ориентацию или размер в сцене.

        }
    };
}

std::string generateRandomString(size_t length) {
    const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::string randomString;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    for(size_t i = 0; i < length; ++i) {
        randomString += characters[distribution(generator)];
    }

    return randomString;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

double calculateFOVHorizontal(double fovVertical, double aspectRatio) {
    double fovVerticalRadians = vtkMath::RadiansFromDegrees(fovVertical);
    double fovHorizontalRadians = 2 * atan(tan(fovVerticalRadians / 2) * aspectRatio);
    return fovHorizontalRadians;
}

void setBackgroundPlaneSize(vtkCamera* camera, vtkPlaneSource* backgroundPlane, double windowWidth, double windowHeight) {
    // Аспектное соотношение окна рендеринга
    double aspectRatio = windowWidth / windowHeight;

    // Расстояние от камеры до фоновой плоскости - предположим, что плоскость должна быть
    // на 0.1 меньше по Z, чем самый дальний объект, чтобы избежать перекрытия
    double distanceToBackground = 1.5; // Вы можете установить это значение экспериментально

    // Вертикальный угол обзора камеры
    double verticalFOV = camera->GetViewAngle();

    // Рассчитываем горизонтальный угол обзора, исходя из вертикального угла обзора и аспектного соотношения
    double horizontalFOV = 2.0 * atan(tan(vtkMath::RadiansFromDegrees(verticalFOV) / 2.0) * aspectRatio);

    // Рассчитываем ширину и высоту фоновой плоскости
    double backgroundHeight = 20.0 * distanceToBackground * tan(vtkMath::RadiansFromDegrees(verticalFOV) / 2.0);
    double backgroundWidth = 20.0 * distanceToBackground * tan(horizontalFOV / 2.0);


    // Теперь устанавливаем размеры фоновой плоскости
    backgroundPlane->SetOrigin(-backgroundWidth / 2.0, -backgroundHeight / 2.0, -distanceToBackground);
    backgroundPlane->SetPoint1(backgroundWidth / 2.0, -backgroundHeight / 2.0, -distanceToBackground);
    backgroundPlane->SetPoint2(-backgroundWidth / 2.0, backgroundHeight / 2.0, -distanceToBackground);
    backgroundPlane->Update();
}

std::array<double, 2> Get2DProjection(vtkCamera* camera, vtkRenderer* renderer, double x, double y, double z) {
    double worldCoordinate[4] = {x, y, z, 1.0};
    double displayCoordinate[3];

    renderer->SetWorldPoint(worldCoordinate);
    renderer->WorldToDisplay();
    renderer->GetDisplayPoint(displayCoordinate);

    return {displayCoordinate[0], displayCoordinate[1]};
}

// Функция для записи координат в JSON-формат
void SaveCoordinatesAsJSON(const std::vector<std::array<double, 2>>& coordinates, const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи." << std::endl;
        return;
    }

    outFile << "{" << std::endl;
    outFile << "  \"all_points_x\": [";
    for (size_t i = 0; i < coordinates.size(); ++i) {
        outFile << coordinates[i][0];
        if (i < coordinates.size() - 1)
            outFile << ", ";
    }
    outFile << "]," << std::endl;

    outFile << "  \"all_points_y\": [";
    for (size_t i = 0; i < coordinates.size(); ++i) {
        outFile << coordinates[i][1];
        if (i < coordinates.size() - 1)
            outFile << ", ";
    }
    outFile << "]" << std::endl;
    outFile << "}" << std::endl;

    outFile.close();
}

std::array<double, 2> ConvertToImageCoordinates(double x, double y, int imageWidth, int imageHeight) {
    // Преобразование координат из системы координат VTK в систему координат изображения.
    double newX = x + imageWidth / 2.0;
    double newY = imageHeight / 2.0 - y;
    return {newX, newY};
}

double xTranslation = -1; // Сдвиг влево [-5; 0.6]
double yTranslation = 0;  // Сдвиг вверх [0; 2.5]
double zTranslation = 0.5;  // Без изменения глубины [0; 1]

int main(int argc, char* argv[]) {
    const char* modelFilePath = argv[1]; // Путь к 3D модели
    const char* backgroundImageFilePath = argv[2]; // Путь к фоновому изображению

    vtkNew<vtkNamedColors> colors; // Создается объект для управления цветами, который предоставляет доступ к предопределенным цветам.

    vtkNew<vtkOBJReader> objReader; // Создается объект для чтения 3D-модели из файла
    objReader->SetFileName("/home/aleksandr/CLionProjects/individual/mesh.obj");

    // Создание источника плоскости
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetOrigin(-0.5, -0.5, 0); // Координаты для квадратной плоскости
    planeSource->SetPoint1(0.5, -0.5, 0);
    planeSource->SetPoint2(-0.5, 0.5, 0);
    planeSource->Update();

    // Создание трансформации для изгиба плоскости
    vtkNew<vtkTransform> transform;
    transform->PostMultiply(); // Для применения масштабирования после остальных трансформаций
    transform->RotateX(45); // Поворот плоскости, чтобы создать эффект изгиба

    // Создается маппер (mapper), который преобразует полигональные данные, полученные от objReader,
    // в графические примитивы, которые можно отрисовать на экране.
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(objReader->GetOutputPort());
    mapper->SetInputConnection(planeSource->GetOutputPort());

    // Создается объект для чтения изображения в формате JPEG, которое будет использоваться как текстура.
    vtkNew<vtkJPEGReader> jpegReader;
    jpegReader->SetFileName(modelFilePath);

    // Создается текстура и ей устанавливается источник данных - выходной поток jpegReader.
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpegReader->GetOutputPort());

    // Создается актор (actor), который представляет собой элемент визуализации в сцене.
    // Актору назначается маппер для геометрии и текстура для внешнего вида.
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);
    transform->Translate(xTranslation, yTranslation, zTranslation);
    actor->SetUserTransform(transform);

    // Создается рендерер (renderer), который управляет тем, как сцена отображается.
    // В рендерер добавляется актор, и устанавливается цвет фона сцены.
    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(actor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    // Создается новый объект камеры, который определяет точку зрения, с которой будет производиться рендеринг сцены.
    vtkNew<vtkCamera> camera;
    double cameraXPos = 2;
    double cameraYPos = -3.1;
    double cameraZPos = 2;

    camera->SetPosition(cameraXPos, cameraYPos, cameraZPos); // вот тут менять расположение камеры в цикле для генерации разных штрих-кодов
    camera->SetFocalPoint(actor->GetPosition()); // камера направлена на положение актора в сцене, то есть то место, где расположен объект.
    camera->SetViewUp(0, 1, 0);
    ren1->SetActiveCamera(camera); // созданная камера назначается активной для рендерера ren1,
    // который затем будет использовать ее для определения, как должна выглядеть сцена.


    // Шаг 1: Загрузка изображения фона
    vtkNew<vtkJPEGReader> backgroundReader;
    backgroundReader->SetFileName(backgroundImageFilePath); // Укажите правильный путь к файлу

    // Шаг 2: Создание плоскости для фона
    vtkNew<vtkPlaneSource> backgroundPlane;
    setBackgroundPlaneSize(camera, backgroundPlane, 1920.0, 1080.0);

    // Шаг 3: Применение текстуры фона
    vtkNew<vtkTexture> backgroundTexture;
    backgroundTexture->SetInputConnection(backgroundReader->GetOutputPort());

    // Шаг 4: Добавление плоскости с текстурой в рендерер
    vtkNew<vtkPolyDataMapper> backgroundMapper;
    backgroundMapper->SetInputConnection(backgroundPlane->GetOutputPort());

    vtkNew<vtkActor> backgroundActor;
    backgroundActor->SetMapper(backgroundMapper);
    backgroundActor->SetTexture(backgroundTexture);

    // Создаем трансформацию для фоновой плоскости
    vtkNew<vtkTransform> backgroundTransform;
    backgroundTransform->PostMultiply(); // Применять масштабирование после других трансформаций
    backgroundTransform->RotateX(60); // Тот же угол поворота, что и у объекта
    backgroundActor->SetUserTransform(backgroundTransform); // Применяем трансформацию к фоновому актору

    // Смещаем фон влево и вверх
    // Значения смещения подбираются экспериментально, чтобы соответствовать вашим требованиям
    double shiftLeft = -2; // Смещение влево; отрицательное значение перемещает влево
    double shiftUp = 1; // Смещение вверх; положительное значение перемещает вверх
    backgroundTransform->Translate(shiftLeft, shiftUp, 0.0);

    ren1->AddActor(backgroundActor); // Убедитесь, что это делается перед добавлением других акторов


    vtkNew<vtkLight> light; // Создается новый объект света, который будет освещать сцену.

    light->SetLightTypeToSceneLight(); // Устанавливается тип света как "сценический свет", что означает,
    // что свет будет рассматриваться как часть сцены и будет влиять на объекты внутри нее.

    light->SetPosition(5 * 0.1, 5 * -1.2, 5 * 2.1); // Задается положение источника света в пространстве сцены.

    light->SetPositional(true); // Указывает, что свет является позиционным (в отличие от направленного),
    // что означает, что он будет иметь определенное положение и характеристики, такие как затухание и конус света.

    light->SetConeAngle(30); // Устанавливается угол конуса для позиционного света, что влияет на распространение света от источника.

    light->SetFocalPoint(actor->GetPosition()); // Задается точка, на которую направлен свет.
    // Она совпадает с положением актора, так что свет будет направлен на объект.

    light->SetDiffuseColor(colors->GetColor3d("White").GetData()); // Определяется основной цвет света, который будет влиять на объекты в сцене.
    light->SetAmbientColor(colors->GetColor3d("Gray").GetData()); // Устанавливается цвет окружающего света, который определяет интенсивность света, рассеянного во всех направлениях.
    light->SetSpecularColor(colors->GetColor3d("White").GetData()); // Устанавливается цвет бликов света, которые появляются на более глянцевых поверхностях.

    light->SetAmbientColor(1.0, 1.0, 1.0); // теперь сцена равномерно освещается белым, но этого не видно(

    ren1->AddLight(light); // Добавляется источник света к рендереру, что позволяет ему влиять на внешний вид сцены.

    vtkNew<vtkRenderWindow> renWin; // окно для рендеринга 3D сцены.
    renWin->AddRenderer(ren1); // К окну рендеринга добавляется рендерер ren1. Рендерер управляет тем, как сцена отображается в окне.
    renWin->SetSize(1920, 1080); // размер окна рендеринга
    renWin->SetWindowName("Tutorial_Step6"); // название окна

    // Интерактор обрабатывает ввод пользователя (например, движения мыши, клики, нажатия клавиш) и переводит его в действия в окне рендеринга.
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin); // Интерактору назначается окно рендеринга, с которым он будет взаимодействовать.

    // Создается объект style класса vtkInteractorStyleTrackballCamera, который определяет стиль взаимодействия
    // пользователя с камерой. В данном случае это "trackball" стиль, который позволяет камере вращаться вокруг объекта,
    // как если бы объект находился внутри виртуального шара (trackball), и пользователь мог вращать этот шар мышью.
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    // Создается новый объект featureEdges, который будет использоваться для выделения границ на полигональной модели
    vtkNew<vtkFeatureEdges> featureEdges;

    // featureEdges настраивается на прием данных с выходного порта objReader, который читает 3D модель из файла.
    featureEdges->SetInputConnection(objReader->GetOutputPort());

    // Включает выделение граничных ребер модели. Это ребра, которые присутствуют только на одной грани полигональной сетки.
    featureEdges->BoundaryEdgesOn();

    // Отключает выделение особых ребер (feature edges), которые обычно отличаются по углу между смежными гранями.
    featureEdges->FeatureEdgesOff();

    // Отключает выделение ребер, которые не являются многообразными (non-manifold edges), то есть ребер, имеющих более двух смежных граней.
    featureEdges->NonManifoldEdgesOff();

    // Отключает выделение многообразных ребер (manifold edges), которые обычно имеют ровно две смежные грани.
    featureEdges->ManifoldEdgesOff();

    // Создается новый маппер edgeMapper, который будет использоваться для отображения данных о выделенных границах.
    vtkNew<vtkPolyDataMapper> edgeMapper;

    // edgeMapper настраивается на прием данных с выходного порта featureEdges, который содержит информацию о выделенных ребрах модели.
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());

    // Создается новый актер edgeActor, который будет использоваться для отображения выделенных ребер в сцене.
    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(colors->GetColor3d("Red").GetData());
    ren1->AddActor(edgeActor);

    // Создается новый экземпляр vtkBoxWidget. Этот виджет представляет собой манипулятор в виде трехмерного
    // прямоугольного ящика, который может быть использован для вращения, перемещения и масштабирования объекта.
    vtkNew<vtkBoxWidget> boxWidget;
    boxWidget->SetInteractor(iren);
    boxWidget->SetPlaceFactor(1.25); // виджет будет на 25% больше, чем ограничивающий прямоугольник объекта.
    boxWidget->GetOutlineProperty()->SetColor(colors->GetColor3d("Gold").GetData());
    boxWidget->GetOutlineProperty()->SetOpacity(0.0); // Сделать рамку полностью прозрачной
    // Виджет связывается с actor, что позволяет ему контролировать положение и ориентацию объекта.
    boxWidget->SetProp3D(actor);
    // Размещает виджет в сцене в соответствии с текущими размерами и позицией связанного объекта.
    boxWidget->PlaceWidget();

    // Виджет подписывается на события интеракции с помощью созданного callback.
    // Это значит, что каждый раз при взаимодействии с виджетом будет вызываться метод Execute класса vtkMyCallback.
    vtkNew<vtkMyCallback> callback;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    // Включает виджет, делая его активным и видимым в сцене для взаимодействия с пользователем
    boxWidget->On();
    boxWidget->Off();

    // Инициализация интерактора и начало цикла рендеринга
    iren->Initialize();

    // делает скриншот
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(renWin);
    windowToImageFilter->SetScale(1); // устанавливает масштаб
    windowToImageFilter->SetInputBufferTypeToRGBA(); // захват альфа-канала (прозрачность)
    windowToImageFilter->ReadFrontBufferOff(); // Чтение из заднего буфера, так как окно не отображается
    windowToImageFilter->Update();

    std::ostringstream filePath;
    auto imageName = split(backgroundImageFilePath, '/').back();
    std::string randomString = generateRandomString(4);
    filePath << "data/" << randomString << "_" << imageName; // надо добавить /.. перед data если запускать через IDE

    vtkNew<vtkPNGWriter> pngWriter;

    pngWriter->SetFileName(filePath.str().c_str());
    pngWriter->SetInputConnection(windowToImageFilter->GetOutputPort());
    pngWriter->Write();

    // Экспорт данных геометрии
    vtkNew<vtkXMLPolyDataWriter> polyDataWriter;
    polyDataWriter->SetFileName("../mesh.vtp");
    polyDataWriter->SetInputConnection(objReader->GetOutputPort());
    polyDataWriter->Write();

    std::vector<std::array<double, 2>> coordinates;

    // Где-то в вашем коде, когда вы получаете vtkActor* actor:
    vtkPolyData* polyData = vtkPolyData::SafeDownCast(actor->GetMapper()->GetInput());

    vtkPoints* points = polyData->GetPoints();

    for(vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
        double* pt = points->GetPoint(i);
        coordinates.push_back(Get2DProjection(camera, ren1, pt[0], pt[1], pt[2]));
    }

    auto imageNameWithoutExtension = split(imageName, '.').front();

    std::ostringstream jsonFilePath;
    jsonFilePath << "data/" << randomString << "_"  << imageNameWithoutExtension << ".json"; // надо добавить /.. перед data если запускать через IDE

    // Сохранение координат в файл JSON
    SaveCoordinatesAsJSON(coordinates, jsonFilePath.str());

    // Начать взаимодействие
    // iren->Start();

    return EXIT_SUCCESS;
}