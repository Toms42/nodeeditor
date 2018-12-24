# cmake for custom

cmake_minimum_required(VERSION 3.12)

project(custom)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}")

set(custom_SRC
  Receiver.cpp
  ReceiverNode.cpp
  Sender.cpp
  SenderNode.cpp
  main.cpp
  )

add_executable(${PROJECT_NAME} ${custom_SRC})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PRIVATE nodes)
