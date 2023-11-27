/*=========================================================================

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkLight.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTransform.h>

namespace {
    class vtkMyCallback : public vtkCommand {
    public:
        static vtkMyCallback* New() {
            return new vtkMyCallback;
        }
        void Execute(vtkObject* caller, unsigned long, void*) override {
            vtkNew<vtkTransform> t;
            auto widget = reinterpret_cast<vtkBoxWidget*>(caller);
            widget->GetTransform(t);
            widget->GetProp3D()->SetUserTransform(t);
        }
    };
}

int main(int, char*[]) {
    vtkNew<vtkNamedColors> colors;

    vtkNew<vtkOBJReader> objReader;
    objReader->SetFileName("/home/aleksandr/CLionProjects/individual/mesh.obj");

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(objReader->GetOutputPort());

    vtkNew<vtkJPEGReader> jpegReader;
    jpegReader->SetFileName("/home/aleksandr/Документы/7sem/polevoy/rendering/chess.jpeg");

    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpegReader->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(actor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    vtkNew<vtkCamera> camera;
    camera->SetPosition(0, 0, 1);
    camera->SetFocalPoint(actor->GetPosition());
    camera->SetViewUp(0, 1, 0);
    ren1->SetActiveCamera(camera);

    vtkNew<vtkLight> light;
    light->SetLightTypeToSceneLight();
    light->SetPosition(1, 1, 1);
    light->SetPositional(true);
    light->SetConeAngle(30);
    light->SetFocalPoint(actor->GetPosition());
    light->SetDiffuseColor(colors->GetColor3d("White").GetData());
    light->SetAmbientColor(colors->GetColor3d("Gray").GetData());
    light->SetSpecularColor(colors->GetColor3d("White").GetData());
    ren1->AddLight(light);

    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("Tutorial_Step6");

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    vtkNew<vtkBoxWidget> boxWidget;
    boxWidget->SetInteractor(iren);
    boxWidget->SetPlaceFactor(1.25);
    boxWidget->GetOutlineProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    boxWidget->SetProp3D(actor);
    boxWidget->PlaceWidget();

    vtkNew<vtkMyCallback> callback;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    boxWidget->On();

    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}
