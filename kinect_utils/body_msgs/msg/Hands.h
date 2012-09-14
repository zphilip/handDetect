#ifndef kinect_Skeleton_h
#define kinect_Skeleton_h

#include "ros.h"

namespace body_msgs
{

  class Skeleton : public ros::Msg
  {
    public:
	int playerid;
	unsigned long head;
	unsigned long neck;
	unsigned long right_hand;
	unsigned long left_hand;
	unsigned long right_shoulder;
	unsigned long left_shoulder;
	unsigned long right_elbow;
	unsigned long left_elbow;
	unsigned long torso;
	unsigned long left_hip;
	unsigned long right_hip;
	unsigned long left_knee;
	unsigned long right_knee;
	unsigned long left_foot;
	unsigned long right_foot;
	    
}
}
#endif