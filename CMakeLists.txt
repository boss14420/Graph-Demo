cmake_minimum_required(VERSION 2.6)
PROJECT(GraphDemo)

#SET(CMAKE_CXX_COMPILER "clang++")
#SET(CMAKE_C_COMPILER "clang")

find_package(Qt4 REQUIRED)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(graphdemo_SOURCES main.cc graphwidget.cc graphscene.cc vertex.cc edge.cc weighttextitem.cc mainwindow.cc algchooser.cc vertexlabel.cc animation.cc algorithm.cc dijkstra.cc bellman_ford.cc bfs.cc astar.cc dag.cc)
SET(graphdemo_HEADERS graphwidget.h graphscene.h vertex.h edge.h weighttextitem.h mainwindow.h algchooser.h vertexlabel.h animation.h algorithm.h dijkstra.h dijkstra_visitor.hpp bellman_ford.h bfs.h astar.h dag.h bellman_ford_visitor.hpp bfs_visitor.hpp astar_visitor.hpp dag_visitor.hpp)
SET(graphdemo_QT_HEADERS graphwidget.h graphscene.h vertex.h edge.h weighttextitem.h mainwindow.h algchooser.h vertexlabel.h algorithm.h dijkstra.h bellman_ford.h bfs.h astar.h dag.h)
SET(graphdemo_RESOURCES graphdemo.qrc)

#SET(CMAKE_CXX_FLAGS "-Wall -O2 -march=native -ftree-vectorize -fomit-frame-pointer -DNDEBUG")
#SET(CMAKE_CXX_FLAGS "-Wall -O3 -march=native -std=gnu++0x -DNDEBUG -DQT_NO_DEBUG")
SET(CMAKE_CXX_FLAGS "-Wall -g -O0 -std=c++11 -pedantic-errors -DNDEBUG")

QT4_WRAP_CPP(graphdemo_HEADERS_MOC ${graphdemo_QT_HEADERS})
QT4_ADD_RESOURCES(graphdemo_RESOURCES_RCC ${graphdemo_RESOURCES})

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

ADD_EXECUTABLE(graphdemo ${graphdemo_SOURCES}
                        ${graphdemo_HEADERS_MOC}
                        ${graphdemo_RESOURCES_RCC}
                )
TARGET_LINK_LIBRARIES(graphdemo ${QT_LIBRARIES})

#INSTALL(TARGETS graphdemo
#    RUNTIME DESTINATION "$ENV{DESTDIR}/usr/bin"
#    )
