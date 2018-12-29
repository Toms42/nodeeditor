cmake_minimum_required(VERSION 3.12)

project(NodeEditor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

#set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
#set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include(CTest)

option(BUILD_EXAMPLES "Build Examples" ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core
             Widgets
             Gui
             OpenGL REQUIRED)

set(RESOURCES "resources/resources.qrc")

# because includes and sources in differend directories - we have to add includes
# in compile source for moc
set(INC_SRC
  include/nodes/Connection.hpp
  include/nodes/ConnectionGeometry.hpp
  include/nodes/ConnectionGraphicsObject.hpp
  include/nodes/DataFlowModel.hpp
  include/nodes/DataFlowScene.hpp
  include/nodes/FlowScene.hpp
  include/nodes/FlowSceneModel.hpp
  include/nodes/FlowView.hpp
  include/nodes/Node.hpp
  include/nodes/NodeComposite.hpp
  include/nodes/NodeDataModel.hpp
  include/nodes/NodeGeometry.hpp
  include/nodes/NodeGraphicsFrame.hpp
  include/nodes/NodeGraphicsObject.hpp
  include/nodes/NodeState.hpp
  )

set(CPP_SOURCE_FILES
  src/CompositeGeometry.cpp
  src/Connection.cpp
  src/ConnectionBlurEffect.cpp
  src/ConnectionGeometry.cpp
  src/ConnectionGraphicsObject.cpp
  src/ConnectionPainter.cpp
  src/ConnectionState.cpp
  src/ConnectionStyle.cpp
  src/DataModelRegistry.cpp
  src/FlowScene.cpp
  src/FlowSceneModel.cpp
  src/DataFlowModel.cpp
  src/DataFlowScene.cpp
  src/FlowView.cpp
  src/FlowViewStyle.cpp
  src/FrameGeometry.cpp
  src/FramePainter.cpp
  src/Node.cpp
  src/NodeComposite.cpp
  src/NodeConnectionInteraction.cpp
  src/NodeDataModel.cpp
  src/NodeGeometry.cpp
  src/NodeGraphicsFrame.cpp
  src/NodeGraphicsObject.cpp
  src/NodePainter.cpp
  src/NodeState.cpp
  src/NodeStyle.cpp
  src/Properties.cpp
  src/StyleCollection.cpp
)

# If we want to give the option to build a static library,
# leave off SHARED here and use BUILD_SHARED_LIBS to
# choose whether to add -D NODE_EDITOR_SHARED
add_library(nodes SHARED
  ${INC_SRC}
  ${CPP_SOURCE_FILES}
  ${RESOURCES}
)
add_library(NodeEditor::nodes ALIAS nodes)

target_include_directories(nodes
  PRIVATE include/nodes
  INTERFACE include)

target_link_libraries(nodes
  PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL
)

target_compile_definitions(nodes
  PUBLIC
    ${Qt5Widgets_DEFINITIONS}
    NODE_EDITOR_SHARED
  PRIVATE
    NODE_EDITOR_EXPORTS
    #NODE_DEBUG_DRAWING
)

target_compile_options(nodes
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127 /EHsc>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
)

target_compile_features(nodes
  PUBLIC
    cxx_std_17
)

###########
# Examples
##

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

##################
# Automated Tests
##

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

###############
# Installation
##

# include(GNUInstallDirs)

# set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/NodeEditor)

# install(TARGETS nodes
#   EXPORT NodeEditorTargets
#   LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )

# install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# install(EXPORT NodeEditorTargets
#   FILE NodeEditorTargets.cmake
#   NAMESPACE NodeEditor::
#   DESTINATION ${INSTALL_CONFIGDIR}
# )

# include(CMakePackageConfigHelpers)

# configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/NodeEditorConfig.cmake.in
#   ${CMAKE_CURRENT_BINARY_DIR}/NodeEditorConfig.cmake
#   INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
# )

# install(FILES
#   ${CMAKE_CURRENT_BINARY_DIR}/NodeEditorConfig.cmake
#   DESTINATION ${INSTALL_CONFIGDIR}
# )
