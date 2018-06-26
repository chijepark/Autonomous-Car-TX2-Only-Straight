#include "my_ZED.h"
void zed_init(sl::InitParameters &init_params, sl::Camera &zed, sl::RuntimeParameters &runtime_parameters)
{
    init_params.camera_resolution = sl::RESOLUTION_HD1080;
    init_params.depth_mode = sl::DEPTH_MODE_PERFORMANCE;
    init_params.coordinate_units = sl::UNIT_METER;

    sl::ERROR_CODE zerr = zed.open(init_params);
    if(zerr != sl::SUCCESS)
    {
        std::cout<<zerr<<std::endl;
        zed.close();
        return;// Quit if an error occurred
    }

    runtime_parameters.sensing_mode = sl::SENSING_MODE_STANDARD;
}
cv::Mat slMat2cvMat(sl::Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM_CPU));
}
int getDepth(sl::Camera &zed)
{
	int image_width, image_height;
	int x[5], y[5];
  	double distance[5];	
 
	sl::Mat zed_point_cloud;
	sl::float4 point_cloud_value[5];
	zed.retrieveMeasure(zed_point_cloud,sl::MEASURE_XYZRGBA);
	
	image_width = zed_point_cloud.getWidth();
	image_height = zed_point_cloud.getHeight();

	x[0] = image_width/3;
	y[0] = image_height/3;
 	x[1] = image_width/2;
	y[1] = image_height/2;
 	x[2] = (image_width*2)/3.0;
	y[2] = image_height/3;
 	x[3] = (image_width*2)/3.0;
	y[3] = (image_height*2)/3.0;
 	x[4] = image_width/3;
	y[4] = (image_height*2)/3.0;
 
   for(int i=0; i<5; i++)
   {
     zed_point_cloud.getValue(x[i], y[i], &point_cloud_value[i]);
   }
   
   for(int i=0; i<5; i++)
   {
      distance[i] =  sqrt(point_cloud_value[i].x * point_cloud_value[i].x + point_cloud_value[i].y * point_cloud_value[i].y + point_cloud_value[i].z * point_cloud_value[i].z);
   	  //cout<<"distance["<<i<<"] : "<<distance[i]<<'\n';
   }
  
  int vote=0;
  for(int i=0;i<5;i++)
  {
    if(distance[i]>=0 && distance[i]<2)
      vote+=1;
  }
  return vote;
}
cv::Mat getCVImage(sl::Camera &zed, sl::RuntimeParameters &runtime_parameters, sl::Mat &image_zed)
{
	int delay=1;
	cv::Mat image_origin;
  if(zed.grab(runtime_parameters) == sl::SUCCESS)
  {
    //Retrieve left image
    zed.retrieveImage(image_zed, sl::VIEW_LEFT, sl::MEM_CPU);
    image_origin = slMat2cvMat(image_zed);
  }
  int ckey = waitKey(delay);
  return image_origin;
}
