#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  GenPath.cpp
  GenPath_Info.cpp
  main.cpp
  CompPath.cpp
)

ADD_EXECUTABLE(pGenPath ${SRC})

TARGET_LINK_LIBRARIES(pGenPath
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread
   geometry
   )

