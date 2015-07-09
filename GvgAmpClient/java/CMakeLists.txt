file(GLOB_RECURSE MYLIB_SWIG_FILES ${CMAKE_CURRENT_SOURCE_DIR} *.i)

find_package(SWIG REQUIRED)
find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)
include(UseSWIG)

set(MYLIB_PACKAGE_NAME "org.mtva.gvg.amp.client")
string(REPLACE "." "/" MYLIB_PACKAGE_DIR ${MYLIB_PACKAGE_NAME})

set(CMAKE_SWIG_FLAGS -package ${MYLIB_PACKAGE_NAME})
set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/src/${MYLIB_PACKAGE_DIR})

set_source_files_properties(${MYLIB_SWIG_FILES} PROPERTIES CPLUSPLUS ON)

include_directories(
  ${PROJECT_SOURCE_DIR}/include
  ${JNI_INCLUDE_DIRS}
)

swig_add_module(GvgAmpClientJava java ${MYLIB_SWIG_FILES})

swig_link_libraries(GvgAmpClientJava GvgAmpClient)

set(CMAKE_JAVA_TARGET_VERSION ${MYLIB_VERSION})
set(CMAKE_JNI_TARGET TRUE)
#FILE(GLOB SWIG_JAVA_FILES ${CMAKE_SWIG_OUTDIR}/*.java)
#MESSAGE( STATUS "SWIG_JAVA_FILES: ${SWIG_JAVA_FILES}")
#add_jar(jGvgAmpClient ${SWIG_JAVA_FILES})

#add_custom_command(OUTPUT scanned_files
#    COMMAND touch ARGS scanned_files
#    COMMENT "Scanning *.java files..." 
#)

add_custom_target(scanned_files ALL
    COMMAND ls -d1 ${CMAKE_SWIG_OUTDIR}/*.java > ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/jGvgAmpClient.dir/java_class_filelist
#     COMMAND FILE(GLOB SWIG_JAVA_FILES ${CMAKE_SWIG_OUTDIR}/*.java)
#
)

#add_jar(jGvgAmpClient ${CMAKE_SWIG_OUTDIR}/*.java)
#FILE(READ scanned_files ${SWIG_JAVA_FILES} 0 0 )
add_jar(jGvgAmpClient ${SWIG_JAVA_FILES})
add_dependencies(jGvgAmpClient GvgAmpClientJava scanned_files)

create_javadoc(javadoc
  PACKAGES ${MYLIB_PACKAGE_NAME}
  SOURCEPATH ${CMAKE_CURRENT_BINARY_DIR}/src
  CLASSPATH ${CMAKE_JAVA_INCLUDE_PATH}
  INSTALLPATH ${CMAKE_CURRENT_BINARY_DIR}
  WINDOWTITLE ${MYLIB_NAME}
  DOCTITLE "<h1>${MYLIB_NAME}</h1>"
  AUTHOR TRUE
  USE TRUE
  VERSION TRUE
)

install_jar(jGvgAmpClient java/lib)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}GvgAmpClientJava${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION java/lib
)
# TODO OP retrieve Doxygen doc in Javadoc?
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/javadoc
  DESTINATION java
)
