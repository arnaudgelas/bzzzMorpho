#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"
#include "vtkPolyDataWriter.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkMetaImageWriter.h"
#include "vtkImageData.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"

#include "vtkPoissonReconstruction.h"

#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 5 )
    {
    std::cerr << "PointCloudReader (.exe) takes 3 arguments: " <<std::endl;
    std::cerr << "1- Input file name" <<std::endl;
    std::cerr << "2- Depth" <<std::endl;
    std::cerr << "3- Output file name (.vtk)" <<std::endl;
    std::cerr << "4- Output file name (.mhd)" <<std::endl;
    return EXIT_FAILURE;
    }
  std::ifstream ifs( argv[1], std::ifstream::in );

  if( !ifs.is_open() )
    {
    return EXIT_FAILURE;
    }
  else
    {
    vtkIdType NumberOfPoints;
    ifs >> NumberOfPoints;

    vtkSmartPointer< vtkPolyData > output = vtkSmartPointer< vtkPolyData >::New();

    vtkSmartPointer< vtkPoints > points = vtkSmartPointer< vtkPoints >::New();
    points->SetNumberOfPoints( NumberOfPoints );

    vtkSmartPointer< vtkPointData > pointdata = output->GetPointData();

    vtkSmartPointer< vtkFloatArray > normals =
      vtkSmartPointer< vtkFloatArray >::New();
    normals->SetNumberOfComponents( 3 );
    normals->SetNumberOfTuples( NumberOfPoints );
    normals->SetName( "Normals" );

    float x[3], N[3];
    for( vtkIdType i = 0; i < NumberOfPoints; i++ )
      {
      ifs >> x[0] >> x[1] >> x[2] >> N[0] >> N[1] >> N[2];
      points->SetPoint( i, x );
      normals->SetTuple( i, N );
      }

    pointdata->SetNormals( normals );
    output->SetPoints( points );

    vtkSmartPointer<vtkPoissonReconstruction> poissonFilter = 
      vtkSmartPointer<vtkPoissonReconstruction>::New();
    poissonFilter->SetDepth( atoi( argv[2] ) );
    poissonFilter->SetInput( output );
    poissonFilter->Update();

    vtkSmartPointer< vtkPolyDataWriter > writer =
      vtkSmartPointer< vtkPolyDataWriter >::New();
    writer->SetFileName( argv[3] );
    writer->SetInputConnection( poissonFilter->GetOutputPort() );
    writer->Update();

    vtkSmartPointer<vtkImageData> whiteImage =
      vtkSmartPointer<vtkImageData>::New();

    double bounds[6];
    bounds[0] = -500.;
    bounds[1] = 500.;
    bounds[2] = -200.;
    bounds[3] = 200.;
    bounds[4] = -200.;
    bounds[5] = 200.;

    double spacing[3]; // desired volume spacing
    spacing[0] = 10.;
    spacing[1] = 10.;
    spacing[2] = 10.;
    whiteImage->SetSpacing(spacing);
  
    // compute dimensions
    int dim[3];
    for (int i = 0; i < 3; i++)
    {
      dim[i] = static_cast<int>(ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
    whiteImage->SetDimensions(dim);
    whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);
  
    double origin[3];
    // NOTE: I am not sure whether or not we had to add some offset!
    origin[0] = bounds[0];// + spacing[0] / 2;
    origin[1] = bounds[2];// + spacing[1] / 2;
    origin[2] = bounds[4];// + spacing[2] / 2;
    whiteImage->SetOrigin(origin);
  
    whiteImage->SetScalarTypeToUnsignedChar();
    whiteImage->AllocateScalars();
  
    // fill the image with foreground voxels:
    unsigned char inval = 255;
    unsigned char outval = 0;
    vtkIdType count = whiteImage->GetNumberOfPoints();
    for (vtkIdType i = 0; i < count; ++i)
    {
      whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }
 

    vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc =
      vtkSmartPointer<vtkPolyDataToImageStencil>::New(); //<---- This is the 'problem'???
    pol2stenc->SetInput( poissonFilter->GetOutput() );
    pol2stenc->SetOutputOrigin(origin);
    pol2stenc->SetOutputSpacing(spacing);
    pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
    pol2stenc->Update();
 
    // cut the corresponding white image and set the background:
    vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
    imgstenc->SetInput( whiteImage );
    imgstenc->SetStencil( pol2stenc->GetOutput() );
    imgstenc->ReverseStencilOff();
    imgstenc->SetBackgroundValue( outval );
    imgstenc->Update();

    vtkSmartPointer<vtkMetaImageWriter> imagewriter =
      vtkSmartPointer<vtkMetaImageWriter>::New();
    imagewriter->SetInput( imgstenc->GetOutput() );
    imagewriter->SetFileName( argv[4] );
    imagewriter->Write();
    }

  return EXIT_SUCCESS;
}

