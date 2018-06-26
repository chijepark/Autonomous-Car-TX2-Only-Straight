#include "my_ZED.h"
#include "my_CV.h"
#include "ros/ros.h"
#include <cstdio>
#include "car_v1/MsgControl.h"

#ifndef __CV_STD_NAMESPACE__
#define __CV_STD_NAMESPACE__
using namespace cv;
using namespace std;
#endif

int main(int argc,char** argv)
{

    sl::Camera zed;
    sl::InitParameters init_params;
    // Set runtime parameters after opening the camera
    sl::RuntimeParameters runtime_parameters;
    zed_init(init_params, zed, runtime_parameters);

    ros::init(argc,argv,"TX2");
    ros::NodeHandle nh;
    ros::Publisher talker = nh.advertise<car_v1::MsgControl>("ros_msg",100);
    car_v1::MsgControl msg;

    sl::Mat image_zed;
    cv::Mat image_origin;
    int steeringInfo;
	double depth_zed;
    while(true)
    {
        image_origin=getCVImage(zed, runtime_parameters, image_zed);
        if(!image_origin.empty())
        {
			int vote = getDepth(zed);
			cout<<"vote value: "<<vote<<'\n';
			if(vote>2)
			{
				msg.data = 9999;
				for(int i=0;i<50;i++)
					talker.publish(msg);
				zed.close();
				return 0;
			}
            steeringInfo = imageProcess(image_origin);
            cout<<"wheel will go "<<steeringInfo<<"degree\n";
            msg.data = steeringInfo;
            talker.publish(msg);
        }
    }
}
