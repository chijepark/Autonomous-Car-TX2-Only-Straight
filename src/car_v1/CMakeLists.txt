cmake_minimum_required(VERSION 2.8.3)
project(car_v1)
## Find catkin macros and libraries
find_package(catkin REQUIRED COMPONENTS
  message_generation
  roscpp
  std_msgs
  sensor_msgs
)
find_package(ZED 2.4 REQUIRED)
find_package(CUDA 9.0 REQUIRED)##before
find_package(OpenCV 3.4.1 REQUIRED
    NO_MODULE
    PATHS /usr/local
    NO_DEFAULT_PATH
)
## Generate messages in the 'msg' folder
add_message_files(
   FILES
   MsgControl.msg
)
## Generate added messages and services with any dependencies listed here
generate_messages(
   DEPENDENCIES
   std_msgs
)
###################################
## catkin specific configuration ##
###################################
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES car_v1
  CATKIN_DEPENDS roscpp std_msgs sensor_msgs
#  DEPENDS system_lib
)
###########
## Build ##
###########

set(OPENCV_CUDA_SAMPLES_REQUIRED_DEPS
  opencv_shape 
  opencv_videostab 
  opencv_photo 
  opencv_core
  opencv_flann
  opencv_imgproc
  opencv_imgcodecs
  opencv_videoio
  opencv_highgui
  opencv_ml
  opencv_video
  opencv_objdetect
  opencv_features2d
  opencv_calib3d
  opencv_superres
  opencv_cudaarithm
  opencv_cudafilters
  opencv_cudawarping
  opencv_cudaimgproc
  opencv_cudafeatures2d
  opencv_cudaoptflow
  opencv_cudabgsegm
  opencv_cudastereo
  opencv_cudalegacy
  opencv_cudaobjdetect
  opencv_cudev
  opencv_stitching
  opencv_dnn
  opencv_cudacodec
)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${ZED_INCLUDE_DIRS}
  ${CUDA_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
)
###line 89 #${OpenCV_INCLUDE_DIRS} out next block in
include_directories(SYSTEM
  ${OpenCV_INCLUDE_DIRS}
)

link_directories(${ZED_LIBRARY_DIR})
link_directories(${CUDA_LIBRARY_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})

add_definitions(-std=c++11)
set(LINK_LIBRARIES
		${catkin_LIBRARIES}
		${ZED_LIBRARIES}
		${CUDA_LIBRARIES}
    ${CUDA_npps_LIBRARY}
    ${CUDA_NPP_LIBRARIES_ZED}
		${OpenCV_LIBS}
)

add_executable(car_v1 src/main.cpp src/my_ZED.cpp src/my_CV.cpp)
add_dependencies(car_v1 ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(car_v1 ${LINK_LIBRARIES} ${OPENCV_CUDA_SAMPLES_REQUIRED_DEPS})

