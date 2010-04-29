#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <iostream>

#include <vtkstd/vector>
#include <vtkstd/map>
#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImagePCAShapeModelEstimator.h"

//**************************************************************************//
char ** GetAllFiles (const char *path)
  {

  DIR *dir = opendir (path);
  struct dirent *dp;          /* returned from readdir() */
  size_t filecount = 0;       /* number of entries in directory */
  size_t i = 0;
  char **files;

  if (dir == NULL) {
    return NULL;            /* opendir() failed */
  }
  /* Pass 1: Count number of files and subdirectories */
  while ((dp = readdir (dir)) != NULL) {
    filecount++;
  }
  /* Allocate space for the result */
  files = (char **) malloc (filecount * sizeof (*files));
  if (files == NULL) {
    return NULL;            /* malloc failed */
  }
  /* Pass 2: Collect all filenames */
  rewinddir (dir);
  while ((dp = readdir (dir)) != NULL) {
    files[i] = strdup (dp->d_name);
    if (files[i] == NULL) {
      /* memory allocation failure; free what's been allocated
       * so far and return NULL.
       */
      while (i > 0) {
        free (files[--i]);
      }
      free (files);
      return NULL;
    }
    //printf ("%d: %s\n", i, dp->d_name);
    i++;
  }

  closedir (dir);
  return files;
  }

//**************************************************************************//
int NumberOfFiles (const char *path)
  {

  DIR *dir = opendir (path);
  struct dirent *dp;          /* returned from readdir() */
  int numberOfFiles = 0;

  if (dir == NULL) {
    return NULL;            /* opendir() failed */
  }
  /* Pass 1: Count number of files and subdirectories */
  while ((dp = readdir (dir)) != NULL) {
    numberOfFiles++;
  }

  closedir (dir);
  return numberOfFiles;
  }

//**************************************************************************//
const char* getFileExtension(const std::string& file)
  {

  std::string str = file;
  std::string ext = "";
  const char* p;

  for(unsigned int i=0; i<str.length(); i++)
    {
    if(str[i] == '.')
      {
      for(unsigned int j = i; j<str.length(); j++)
        {
        ext += str[j];
        }
      return p = ext.c_str();
      }
    }

  return p = ext.c_str();
  }

//**************************************************************************//

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cerr << "PCAShapeModelEstimator(.exe) takes argument" <<std::endl;
    std::cerr << "1-Input Folder" <<std::endl;
    return EXIT_FAILURE;
    }

  const unsigned int Dimension = 3;
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType;

  // Get the files which are in the folder "argv[1]"
  static char **files = GetAllFiles (argv[1]);

  // Load all the polyDatas.vtk files and store it in a vtkstd::vector
  int numberOfFiles = NumberOfFiles(argv[1]);

  std::list< ImageType::Pointer > listofimages;

  for(int i=0; i<numberOfFiles; i++)
    {
    if(strcmp(getFileExtension(files[i]),".mhd") == 0)
      {
      std::string filename = argv[1];
      filename += files[i];
      
      ReaderType::Pointer reader = ReaderType::New();
      reader->SetFileName( filename.c_str() );
      reader->Update();

      listofimages.push_back( reader->GetOutput() );
      listofimages.back()->DisconnectPipeline();
      }
    }

  unsigned int NumberOfEigenValues = 3;

  typedef itk::ImagePCAShapeModelEstimator<ImageType, ImageType>
    ImagePCAShapeModelEstimatorType;

  ImagePCAShapeModelEstimatorType::Pointer 
    applyPCAShapeEstimator = ImagePCAShapeModelEstimatorType::New();
  applyPCAShapeEstimator->SetNumberOfTrainingImages( listofimages.size() );
  applyPCAShapeEstimator->SetNumberOfPrincipalComponentsRequired( NumberOfEigenValues );

  size_t j = 0;
  std::list< ImageType::Pointer >::iterator im_it = listofimages.begin();

  while( im_it != listofimages.end() )
    {
    applyPCAShapeEstimator->SetInput( j++, *im_it );
    ++im_it;
    }

  applyPCAShapeEstimator->Update();

  ImagePCAShapeModelEstimatorType::VectorOfDoubleType 
    eigen = applyPCAShapeEstimator->GetEigenValues();

  for( unsigned int k = 0; k < NumberOfEigenValues + 1; k++ )
    {
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( applyPCAShapeEstimator->GetOutput( k ) );
    if( k == 0 )
      {
      writer->SetFileName( "mean.mhd" );
      }
    else
      {
      std::stringstream ssfilename;
      ssfilename << "eigenvalue_" << k  << ".mhd";
      std::string filename;
      ssfilename >> filename;
      std::cout << k << "  *  " <<eigen[k-1] << "  *  " 
        <<filename.c_str() <<std::endl;
      writer->SetFileName( filename.c_str() );
      }
    writer->Update();
    }
  return EXIT_SUCCESS;
}

