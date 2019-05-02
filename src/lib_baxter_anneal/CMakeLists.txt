#--------------------------------------------------------
# The CMakeLists.txt for:                 lib_baxter_anneal
# Author(s):                               David Baxter modified from Henrik Schmidt
#--------------------------------------------------------

FILE(GLOB SRC *.cpp *.c)

ADD_LIBRARY(baxter_anneal SHARED ${SRC})

TARGET_LINK_LIBRARIES(baxter_anneal ${MOOS_LIBRARIES} mbutil)
