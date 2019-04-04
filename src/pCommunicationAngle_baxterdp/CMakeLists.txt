#--------------------------------------------------------
# The CMakeLists.txt for:                       pCommunicationAngle_baxterdp
# Author(s):                              David Baxter
#--------------------------------------------------------

SET(SRC
  CommunicationAngle_baxterdp.cpp
  CommunicationAngle_baxterdp_Info.cpp
  main.cpp
  Data.cpp
  Data.h
)

ADD_EXECUTABLE(pCommunicationAngle_baxterdp ${SRC})

TARGET_LINK_LIBRARIES(pCommunicationAngle_baxterdp
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread)

