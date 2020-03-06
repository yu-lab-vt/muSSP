cmake_minimum_required (VERSION 3.5)

project(FollowMe)

set(SOURCES
    Graph.cpp
    main.cpp
    Node.cpp
    Sink.cpp
)

set(HEADERS
    Graph.h
    Node.h
    Sink.h
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})
