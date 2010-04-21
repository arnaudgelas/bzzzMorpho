#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSignedDanielssonDistanceMapImageFilter.h"
#include "itkImage.h"


int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cerr << "SignedDistance (.exe) requires 2 arguments" << std::endl;
    std::cerr << "1- Input Image" <<std::endl;
    std::cerr << "2- Output Image" <<std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > InputReaderType;
  typedef itk::Image< float, Dimension > OutputImageType;
  typedef itk::SignedDanielssonDistanceMapImageFilter< ImageType, OutputImageType >
    FilterType;
  typedef itk::ImageFileWriter< OutputImageType > OutputImageWriterType;

  InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetUseImageSpacing( true );
  filter->SetInsideIsPositive( true );
  filter->Update();

  OutputImageWriterType::Pointer writer = OutputImageWriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( argv[2] );
  writer->Update();
  
  return EXIT_SUCCESS;
}
