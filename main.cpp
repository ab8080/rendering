/*=========================================================================

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Include the necessary header files for the VTK classes we are using.
#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>       // Include vtkJPEGReader for reading JPEG files
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>        // Include vtkOBJReader for reading OBJ files
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>          // Include vtkTexture for applying texture
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

    // Create a new vtkOBJReader
    vtkNew<vtkOBJReader> objReader;
    objReader->SetFileName("/home/aleksandr/CLionProjects/individual/mesh.obj");

    // Create a new vtkPolyDataMapper and set the objReader as its input
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(objReader->GetOutputPort());

    // Create a new vtkJPEGReader to read the texture
    vtkNew<vtkJPEGReader> jpegReader;
    jpegReader->SetFileName("/home/aleksandr/Документы/7sem/polevoy/rendering/chess.jpeg");

    // Create a new vtkTexture and set the jpegReader as its input
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jpegReader->GetOutputPort());

    // Create an actor to represent the loaded 3D mesh and set the texture
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    // Create the Renderer and assign actors to it.
    vtkNew<vtkRenderer> ren1;
    ren1->AddActor(actor);
    ren1->SetBackground(colors->GetColor3d("MidnightBlue").GetData());

    // Create the render window
    vtkNew<vtkRenderWindow> renWin;
    renWin->AddRenderer(ren1);
    renWin->SetSize(300, 300);
    renWin->SetWindowName("Tutorial_Step6");

    // The vtkRenderWindowInteractor class watches for events in the vtkRenderWindow.
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renWin);

    // Specify a particular interactor style.
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    iren->SetInteractorStyle(style);

    // Use a vtkBoxWidget to transform the underlying actor.
    vtkNew<vtkBoxWidget> boxWidget;
    boxWidget->SetInteractor(iren);
    boxWidget->SetPlaceFactor(1.25);
    boxWidget->GetOutlineProperty()->SetColor(colors->GetColor3d("Gold").GetData());

    // Place the interactor initially.
    boxWidget->SetProp3D(actor);
    boxWidget->PlaceWidget();
    vtkNew<vtkMyCallback> callback;
    boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

    // Enable the box widget.
    boxWidget->On();

    // Start the event loop.
    iren->Initialize();
    iren->Start();

    return EXIT_SUCCESS;
}
