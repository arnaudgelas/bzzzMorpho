cmake_minimum_required( VERSION 2.6 )

PROJECT( PoissonReconstruction )

FIND_PACKAGE( VTK REQUIRED )
INCLUDE( ${VTK_USE_FILE} )

ADD_LIBRARY( vtkPoissonReconstruction
  vtkPoissonReconstruction.cxx
  Factor.cpp
  Geometry.cpp
  MarchingCubes.cpp
  MultiGridOctest.cpp
  MultiGridOctreeData.cpp
)

TARGET_LINK_LIBRARIES( vtkPoissonReconstruction
  vtkHybrid
)

ADD_EXECUTABLE( PoissonReconstructionExample Example.cxx )
TARGET_LINK_LIBRARIES( PoissonReconstructionExample
  vtkPoissonReconstruction
)

