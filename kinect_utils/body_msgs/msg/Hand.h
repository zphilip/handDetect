#ifndef kinect_Skeleton_h
#define kinect_Skeleton_h

#include "ros.h"
#include "geometry_msgs/Point32.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Transform.h"
#include "sensor_msgs/PointCloud2.h"
#include "PCLCommon.h"
#include <string> 

namespace body_msgs
{

  class Hand : public ros::Msg
  {
    public:
		//time stamp;
		int32 seq; 
			// seq is used for tracking the hand - 
			// if the seq number is the same, you can assume that the hand is the same as previously seen
		int32 thumb;
		bool left;   //# if this hand is the left hand
		geometry_msgs::Point arm;
		geometry_msgs::Transform palm;
		geometry_msgs::Point *fingers;
		sensor_msgs::PointCloud2 handcloud;
		//Possibilities for state variable:
		//open - open palm, usually five fingers
		//grip - fingers curled forward
		//paddle -  fingers together and straight
		//fist   
		std::string state;
	};

    virtual int serialize(unsigned char *outbuffer)
    {
      int offset = 0;
	  return offset;
    };

    virtual int deserialize(unsigned char *inbuffer)
    {
      int offset = 0;
	  return offset;
    };

    const char * getType(){ return "body_msgs/Hand"; };

}
#endif