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

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors; // Создается объект для управления цветами, который предоставляет доступ к предопределенным цветам.

    vtkNew<vtkOBJReader> objReader; // Создается объект для чтения 3D-модели из файла
    objReader->SetFileName("/home/aleksandr/CLionProjects/individual/mesh.obj");

    // Создание источника плоскости
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetOrigin(0.0, 0.0, 0.0);
    planeSource->SetPoint1(0.913, 0.0, 0.0);
    planeSource->SetPoint2(0.0, 1.291, 0.0);
    planeSource->SetCenter(0.0, 0.0, 0.0);
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
    jpegReader->SetFileName("../chess.jpeg");

    // Создается текстура и ей устанавливается источник данных - выходной поток jpegReader.
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpegReader->GetOutputPort());

    // Создается актор (actor), который представляет собой элемент визуализации в сцене.
    // Актору назначается маппер для геометрии и текстура для внешнего вида.
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);
    actor->SetUserTransform(transform);

    // Создается рендерер (renderer), который управляет тем, как сцена отображается.
    // В рендерер добавляется актор, и устанавливается цвет фона сцены.
    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(actor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    // Создается новый объект камеры, который определяет точку зрения, с которой будет производиться рендеринг сцены.
    vtkNew<vtkCamera> camera;
    camera->SetPosition(0, -1.5, 2);
    camera->SetFocalPoint(actor->GetPosition()); // камера направлена на положение актора в сцене, то есть то место, где расположен объект.
    camera->SetViewUp(0, 1, 0);
    ren1->SetActiveCamera(camera); // созданная камера назначается активной для рендерера ren1,
    // который затем будет использовать ее для определения, как должна выглядеть сцена.

    vtkNew<vtkLight> light; // Создается новый объект света, который будет освещать сцену.

    light->SetLightTypeToSceneLight(); // Устанавливается тип света как "сценический свет", что означает,
    // что свет будет рассматриваться как часть сцены и будет влиять на объекты внутри нее.

    light->SetPosition(0.1, -1.2, 2.1); // Задается положение источника света в пространстве сцены.

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

    // Инициализация интерактора и начало цикла рендеринга
    iren->Initialize();

    // делает скриншот
    vtkNew<vtkWindowToImageFilter> windowToImageFilter;
    windowToImageFilter->SetInput(renWin);
    windowToImageFilter->SetScale(1); // устанавливает масштаб
    windowToImageFilter->SetInputBufferTypeToRGBA(); // захват альфа-канала (прозрачность)
    windowToImageFilter->ReadFrontBufferOff(); // Чтение из заднего буфера, так как окно не отображается
    windowToImageFilter->Update();

    vtkNew<vtkPNGWriter> pngWriter;
    pngWriter->SetFileName("../screenshot.png");
    pngWriter->SetInputConnection(windowToImageFilter->GetOutputPort());
    pngWriter->Write();

    // Экспорт данных геометрии
    vtkNew<vtkXMLPolyDataWriter> polyDataWriter;
    polyDataWriter->SetFileName("../mesh.vtp");
    polyDataWriter->SetInputConnection(objReader->GetOutputPort());
    polyDataWriter->Write();

    // Начать взаимодействие
    iren->Start();


    return EXIT_SUCCESS;
}