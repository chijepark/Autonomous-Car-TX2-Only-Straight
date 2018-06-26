#!/bin/bash
source ./devel/setup.bash
hostname -I 1.1.1.2
export ROS_IP=1.1.1.2
rosrun car_v1 car_v1
