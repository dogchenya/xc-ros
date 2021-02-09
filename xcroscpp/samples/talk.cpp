#include "ros/ros.h"  
#include "std_msgs/String.h"  

#include <sstream>  
int main(int argc, char **argv)  
{  
 
  xcros::init(argc, argv, "talker");  
  
  xcros::NodeHandle n;  

  xcros::Publisher chatter_pub = n.advertise<std_msgs::String>("chatter", 1000);  

  xcros::Rate loop_rate(10);  

  /** 
   * A count of how many messages we have sent. This is used to create 
   * a unique string for each message. 
   */  
  int count = 0;  
  while (xcros::ok())  
  {   
    std_msgs::String msg;  
    std::stringstream ss;  
    ss << "hello world " << count;  
    msg.data = ss.str();  
    ROS_INFO("%s", msg.data.c_str());  
    chatter_pub.publish(msg); 
    xcros::spinOnce();  
    loop_rate.sleep();  
    ++count;  
   }
    return 0;  
} 