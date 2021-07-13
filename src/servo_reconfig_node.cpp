#include <ros/ros.h>

#include <dynamic_reconfigure/server.h>
#include <servo_reconfig/ServoConfig.h>

#include <servo_array_driver/Servo.h>
#include <servo_array_driver/ServoArray.h>

double speed;
double steer;
bool use = false;
bool key = false;

void reconfig_callback(servo_reconfig::ServoConfig &config, uint32_t level)
{
	speed = config.Speed_Servo;
	steer = config.Steer_Servo;
	use = config.Send_Message;
	key = true;
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "servo_reconfig_node");

	ros::NodeHandle n;

	ros::Publisher pub;

	

	pub = n.advertise<servo_array_driver::ServoArray>("/servos_absolute", 1);

	dynamic_reconfigure::Server<servo_reconfig::ServoConfig> server;
	dynamic_reconfigure::Server<servo_reconfig::ServoConfig>::CallbackType f;

	f = boost::bind(&reconfig_callback, _1, _2);
	server.setCallback(f);

	ros::Rate loop_rate(1);
	key = false;

	while (ros::ok())
	{
		servo_array_driver::Servo speed_servo;
		servo_array_driver::Servo steer_servo;
		servo_array_driver::ServoArray servo_msg;
		
		if (key)
		{
			key=false;
			speed_servo.value = speed;
			steer_servo.value = steer;
			speed_servo.servo = 1;
			steer_servo.servo = 2;
			servo_msg.servos.push_back(speed_servo);
			servo_msg.servos.push_back(steer_servo);
			if (use)
			{
				pub.publish(servo_msg);
			}
		}
		loop_rate.sleep();
		ros::spinOnce();
	}
	return 0;
}