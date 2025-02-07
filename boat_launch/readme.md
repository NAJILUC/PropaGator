#Boat Launch 
## Launch file descriptions
### run.launch
run.launch launches everything needed for the boat to move safely. The following nodes/launch files are started:
* dynamixel_servo_server.launch
  * Starts the dynamixel servo server which allows commands to be sent to the four servos, port thruster, starboard thruster, hydrophone winch, and lidar
* static_transform_publisher (forward_camera_tf)
  * This std node launches a static transform between base_link and lidar
* kill_master
  * This node listens to all kill_broadcaster sources and publishes them out for all kill_listeners to listen to. This node must be on to ensure the boat is safe to run. Please see [kill_handling readme](https://github.com/uf-mil/software-common/blob/master/kill_handling/readme.md) for more information on the kill system.
* communication_monitoring_node
  * This node monitors communication between the boat and a shore communication_monitoring_node. By default after 10 seconds of no connection this node will emit a kill signal. Once connection is restablished the kill is removed automatically. __For this node to function properly another communication node must be registered on a shore computer__. To do so run the following cmd on a shore computer:
```
$ roslaunch communication_monitor shore_communication_monitor.launch
```
* stm32f3discovery_imu_driver
  * This node interfaces with an stm32f3discovery board to produce imu data (which we do not use since we have a much much better imu) and most importantly pwm signals for the thruster's electronic speed controllers(ESC). Note that this node takes about thirty seconds before becoming operational. __Warn: when starting this node you must ensure that there are no publishers to /stm32f3discovery_imu_driver/pwm[1/2] or that any of said publishers are publishing pwm zero (1.5e-3)__. If a non zero pwm is published and the ESCs were not previously armed they will enter programing mode and destroy all previous settings. If this occurs they must be manually reprogramed back to 1e-3 minimum, 1.5e-3 zero, 2e-3 max.

## Mission descriptions
### start_gate_laser
This mission attempts to go through the start gate and speed gate using only lidar data

Algorithim: 
* Set lidar angle to pan between max (calculated below) and slightly above horizontal, so as to only see start/speed gates which should be the tallest object
* Ignore everything behind the boat
* Find the two clossest objects
* If they look like a start gate move towards there centroid
* repete one more time

Math:
* Maximum angle of lidar, 3.529 deg at 50 ft and 7.031 deg at 25ft:
  * The competitoin start/speed gate buoys are 49" tall
  * The lidar sits above the water line about 1 ft
  * Some of the gate will be submerged
  * The max angle is the angle formed between the ray drawn from the lidar to the top of the buoy and the line drawn through the lidar parrallel to horizontal
  * The buoys could be anywhere between 25-50 ft away
 
TODO:
* See this [issue](https://github.com/uf-mil/PropaGator/issues/35)
  
References:
* [Rules] (https://s3.amazonaws.com/com.felixpageau.roboboat/RoboBoat_2015_final_rules_20150527.pdf)
  
## TODO
* Add descriptions for scripts
* Add descriptions for missions
* Finish adding descriptions of launch files
* Consider a master launch file which launches everything needed all in one
