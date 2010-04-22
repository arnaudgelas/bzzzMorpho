PROJECT( BZZZMORPHO )

CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

ADD_SUBDIRECTORY( PoissonReconstruction )

INCLUDE_DIRECTORIES( BEFORE
  PoissonReconstruction
)

ADD_EXECUTABLE( PointCloudReader PointCloudReader.cxx )
TARGET_LINK_LIBRARIES( PointCloudReader vtkPoissonReconstruction )

FIND_PACKAGE( ITK REQUIRED )
INCLUDE( ${ITK_USE_FILE} )

ADD_EXECUTABLE( SignedDistance SignedDistance.cxx )
TARGET_LINK_LIBRARIES( SignedDistance ITKIO ITKCommon )

ADD_EXECUTABLE( PCAShapeModelEstimator PCAShapeModelEstimator.cxx )
TARGET_LINK_LIBRARIES( PCAShapeModelEstimator ITKIO ITKCommon )