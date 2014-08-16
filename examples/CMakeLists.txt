CMAKE_MINIMUM_REQUIRED(VERSION 2.6)


FUNCTION(CreateExampleOpenCv NAME)
	SET(SAMPLE_NAME slimage-example-${NAME})
	ADD_EXECUTABLE(${SAMPLE_NAME} ${NAME}.cpp)
	TARGET_LINK_LIBRARIES(${SAMPLE_NAME} opencv_core opencv_highgui)
ENDFUNCTION(CreateExampleOpenCv)

CreateExampleOpenCv(view)
CreateExampleOpenCv(opencv_to_slimage)
CreateExampleOpenCv(slimage_to_opencv)
CreateExampleOpenCv(foo)
CreateExampleOpenCv(lena_opencv)


find_package(Qt4 REQUIRED)

FUNCTION(CreateExampleQt NAME)
	SET(SAMPLE_NAME slimage-example-${NAME})
	INCLUDE(${QT_USE_FILE})
	ADD_EXECUTABLE(${SAMPLE_NAME} ${NAME}.cpp)
	ADD_DEFINITIONS(${QT_DEFINITIONS})
	TARGET_LINK_LIBRARIES(${SAMPLE_NAME} ${QT_LIBRARIES})
ENDFUNCTION(CreateExampleQt)

CreateExampleQt(qt)
CreateExampleQt(lena_qt)
