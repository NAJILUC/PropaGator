#! /usr/bin/env python

import tools
from tools import line
import rospy
import numpy as np
from uf_common.msg import PoseTwist
from geometry_msgs.msg import Pose, Twist, Vector3, Point
import math
from geometry_msgs.msg import Vector3
from uf_common.orientation_helpers import xyz_array
import sensor_msgs.point_cloud2 as pc2
from sensor_msgs.msg import PointCloud2
import tf
import threading

from heapq import heappush, heappop # for priority queue
import math
'''
The a_star algoritm returns a string of numbers representing the optimal path
each step is represented by a number from 0 to 7
after each iteraton of the algoritim the trajectory generator passes the first step of the path 
as an /enu point
boat = >
if dirs == 8:
__________
|5_|6_|7_|
|4_|>_|0_|
|3_|2_|1_|

if dirs == 4:
__________
|__|1_|__|
|2_|>_|0_|
|__|3_|__|

'''
"""
    This class implements a simple reactive path planner
        The generation is as follows:
            The algorithim takes in a current position, a desired position, 
            posible movements(dirs), and occupied nodes and finds an optimal path 
            from the start to finish as a series of steps
            

"""

class a_star_rpp:
    # Initilization function
    def __init__(self):
        # Desired pose
        self.desired_position = self.current_position = np.zeros(3)
        self.desired_orientation = self.current_orientation = np.zeros(3)
        # Positional tolerance
        self.orientation_radius = rospy.get_param('orientation_radius', 1)
        
        #Management of occupied points
        '''The points are taken in through the pointcloud_callback which has a generator 
           that iterates through the PointCloud2 message and converts each point in the message into an occupancy grid coordinant 
           and adds it to a list called store_points 
           this list is then added to a list called store_pc
        '''
        self.raw_pc_sub = rospy.Subscriber("/lidar/raw_pc", PointCloud2, self.pointcloud_callback, queue_size=1)
        self.pc_to_keep = 16
        self.pc_count = 0
        self.store_pc = [0]
        self.xyz_point = [0,0,0]
        self.store_points = [[]]

        #Management of movement steps
        '''Static tf broadcasters in the azi launch file keep track of the /enu coordinants of the cells surounding the boat
           the step decision references a tf listner which generates a desired position of the boat which is passed to azi_waypoint 
           which publishes it to the trajectory topic
        '''
        self.listener = tf.TransformListener()
        self.cell_dir_tf = {'0':'/step0',
                            '1':'/step3',
                            '2':'/step2',
                            '3':'/step1',}
    

    # Accept a new goal in the form of a posetwist
    def new_goal(self, goal):
        self.desired_position = tools.position_from_posetwist(goal)
        self.desired_orientation = tools.orientation_from_posetwist(goal)

        # Twist not supported
        if (xyz_array(goal.twist.linear).any() or 
            xyz_array(goal.twist.angular).any() ):
            rospy.logwarn('None zero twist are not handled by the azi a_star_rpp trajectory generator. Setting twist to 0')

    # Preempt a goal
    def preempt_goal(self):
        self.desired_position = self.current_position
        self.desired_orientation = self.current_orientation

    # Recieve feedback in the form of a posetwist(pt)
    def feedback(self, pt):
        # Update current pt
        self.current_position = tools.position_from_posetwist(pt)
        # Zero the Z
        self.current_position[2] = 0
        self.current_orientation = tools.orientation_from_posetwist(pt)
        
    # Update the trajactory and return a posetwist
    def update(self):
        '''
        # Check if in the orientation radius for the first time
        if self.redraw_line and self.distance_to_goal < self.orientation_radius:
            self.redraw_line = False
            rospy.loginfo('Redrawing trajectory line')
            self.line = line(self.desired_position, tools.normal_vector_from_rotvec(self.desired_orientation) + self.desired_position)
        '''
        # Output a posetwist (carrot)
        distance = np.linalg.norm(self.desired_position - self.current_position)
        #angles so the boat isnt asked to straif too severly
        step_angle = {'0':self.line.angle, 
                      '1':self.line.angle+np.pi/3, 
                      '3':self.line.angle-np.pi/3,
                      '2':self.line.angle
                      }
        #desired speed in x direction
        x_velocity = {'0':0.7, 
                      '3':0, 
                      '1':0,
                      '2':0}
        
        decision = self.map_builder()
        print decision
        if decision == None:
            print 'EMPTY' 
            return PoseTwist(
                pose = Pose(
                    position = tools.vector3_from_xyz_array(self.desired_position),
                    orientation = tools.quaternion_from_rotvec(self.desired_orientation)),

                twist = Twist(
                    linear = Vector3(),        
                    angular = Vector3())
                )
        if decision in self.cell_dir_tf.keys():
            c_pos = self.look_up_step(self.cell_dir_tf[decision])
            c_pos=tools.vector3_from_xyz_array(c_pos)
            c_angle = step_angle[decision]
            c_velocity = x_velocity[decision]

        else:  
            c_pos = tools.vector3_from_xyz_array(self.current_position)
            c_angle =  self.line.angle#self.line.angle
            c_velocity = 0
        # If bproj is in threashold just set the carrot to the final position
        if distance < 0.5:
            c_pos = tools.vector3_from_xyz_array(self.current_position)
            c_angle =  self.line.angle

        carrot = PoseTwist(
                pose = Pose(
                    position = (c_pos),
                    orientation = tools.quaternion_from_rotvec([0, 0, c_angle])),
                twist = Twist(
                    linear = Vector3(c_velocity, 0, 0),        
                    angular = Vector3())
                )
        return carrot

    # Stop this path planner
    # Nothing required on stop
    def stop(self):
        pass

    # Start the trajectory generator with the current posetwist
    def start(self, current_pt):
        # Set current position and twist
        self.current_position = tools.position_from_posetwist(current_pt)
        # Zero the Z
        self.current_position[2] = 0
        self.current_orientation = tools.orientation_from_posetwist(current_pt)

        # Initlize Trajectory generator with current position as goal
        #   Set the line to be along our current orientation
        self.desired_position = self.current_position
        self.desired_orientation = self.current_orientation
        #   Make a line along the orientation
        self.line = line(self.current_position, tools.normal_vector_from_rotvec(self.current_orientation) + self.current_position)
        self.redraw_line = False

    def pointcloud_callback(self, msg):

        pointcloud = pc2.read_points(msg, field_names=("x", "y", "z"), skip_nans=False, uvs=[])
        # While there are points in the cloud to read...

        if self.pc_count<self.pc_to_keep:
            self.pc_count=self.pc_count+1
        #else:self.store_pc.pop(0)
        
        while True:
            try:
                # new point
                point = next(pointcloud)
                #print point
                #convert point to a_star map coordinates
                self.xyz_point = point
                #add point to list of points
                self.store_points.append(list(self.xyz_point))
                #self.store_points.pop(0)

            # When the last point has been processed
            except StopIteration:
                break
        
        self.store_pc.append(list(self.store_points))
  

        #print 'self.store_points', self.store_points
        #return self.xyz_point

    def map_builder(self):
        # MAIN
        print 'current_position',self.current_position
        print 'desi_position',self.desired_position
        print 'current_orientation', self.current_orientation
        rot = None
        #get transformation from /enu to /base_link
        try:
            (trans,rot) = self.listener.lookupTransform('/enu','/base_link', rospy.Time(0))
        except tf.Exception as e:
            rospy.logwarn('tf error' + str(e))
        if rot == None or trans == None: return
        
        rot = np.asarray(rot)
        trans = np.array(trans)
        angle = tools.quat_to_rotvec(rot)
        z_rotation = angle[2]
        rotMatrix = np.array([[np.cos(z_rotation), np.sin(z_rotation)], 
                              [-np.sin(z_rotation),  np.cos(z_rotation)]])

        #test = np.array([0,0,0])#self.current_position + [1,0,0]
        #print 'Current pose: ', test
        #print 'translate: ', test - trans
        #print 'translate rotat: ', rotMatrix.dot((test - trans)[:2])


        dirs = 4 # number of possible directions to move on the map
        if dirs == 4:
            dx = [1, 0, -1, 0]
            dy = [0, 1, 0, -1]
        elif dirs == 8:
            dx = [1, 1, 0, -1, -1, -1, 0, 1]
            dy = [0, 1, 1, 1, 0, -1, -1, -1]

        n = 30# horizontal size of the map
        m = 30# vertical size of the map
        the_map = []
        row = [0] * n
        for i in range(m): # create empty map
            the_map.append(list(row))
        #place starting position in the grid at (15,5)
        (xA, yA) = [5, 
                    int(math.floor(n/2))]
        #translate desired position into baselink
        (xB, yB) = [self.desired_position[0]-self.current_position[0], 
                    self.desired_position[1]-self.current_position[1]]
        print '1', xB, yB
        #apply rotation matrix
        des_pos_grid = np.array([xB, yB])
        des_pos_grid = rotMatrix.dot(des_pos_grid)
        print '2', des_pos_grid
        (xB, yB) = [int(math.floor(des_pos_grid[0]+xA)), 
                    int(math.floor(des_pos_grid[1]+yA))]
        print '3', xB, yB
        if (xA, yA) == (xB, yB):
            return None
        i=0
        j=0
        k=0
        a=0
        #print 'store pc: ', self.store_pc
        print 'len(self.pc_count)', (self.pc_count)
        if self.pc_count > 0:
            for x in range(0,self.pc_count):
                point_cloud = self.store_pc[x]

                # Workaround weird ints instead of pcs
                if type(point_cloud) == int: continue

                for k in range(0, len(point_cloud)):
                    point = point_cloud[k]
                    if point != None and len(point) > 1:
                        # Translate to base_link
                        point = point - trans
                        # Drop the z
                        point = point[:2]
                        # rotate to base_link
                        point = np.dot(rotMatrix, point)

                        # Convert to grid
                        baselink_point = [
                                    (int(math.floor(point[0])))+xA,
                                    (int(math.floor(point[1])))+yA,
                                    0]
                        #print 'baselink_point', baselink_point
                        #the_map[y][x] (row,column)
                        if baselink_point != None and len(baselink_point) > 1 and 0 < baselink_point[0] < 29 and 0 < baselink_point[1] < 29:
                            #print 'placeing baselink_point', baselink_point                        
                            the_map[baselink_point[1]][baselink_point[0]] = 1
                            #the_map[baselink_point[1]-1][baselink_point[0]] = 1
                            the_map[baselink_point[1]][baselink_point[0]-1] = 1
                            #the_map[baselink_point[1]][baselink_point[0]-2] = 1
                            the_map[baselink_point[1]+1][baselink_point[0]] = 1
        else:
            print 'NO POINTS'
        #the placing of the current position and the desired position on the occupancy grid    



        route = self.pathFind(the_map, n, m, dirs, dx, dy, xA, yA, xB, yB)
        print 'Route: ', '(', route, ')'

                
        # mark the route on the map
        if len(route) > 0:
            x = xA
            y = yA
            the_map[y][x] = 2
            for i in range(len(route)):
                j = int(route[i])
                x += dx[j]
                y += dy[j]
                the_map[y][x] = 3
            the_map[y][x] = 4

        # display the map with the route added
        
        print 'Map (Bottom up view):'
        for y in range(m):
            for x in range(n):
                xy = the_map[y][x]
                if xy == 0:
                    print '.', # space
                elif xy == 1:
                    print 'O', # obstacle
                elif xy == 2:
                    print 'S', # start
                elif xy == 3:
                    print 'R', # route
                elif xy == 4:
                    print 'F', # finish
            print


        if len(route)>0:
            return route[0]        
        
    # A-star algorithm.
    # The path returned will be a string of digits of directions.
    def pathFind(self, the_map, n, m, dirs, dx, dy, xA, yA, xB, yB):
        print "xA", xA
        print "xB", xB
        print "yA", yA
        print "yB", yB

        closed_nodes_map = [] # map of closed (tried-out) nodes
        open_nodes_map = [] # map of open (not-yet-tried) nodes
        dir_map = [] # map of dirs
        row = [0] * n
        #print 'row,' ,row
        for i in range(m): # create 2d arrays
            closed_nodes_map.append(list(row))
            open_nodes_map.append(list(row))
            dir_map.append(list(row))
        #print dir_map
        pq = [[], []] # priority queues of open (not-yet-tried) nodes
        pqi = 0 # priority queue index
        # create the start node and push into list of open nodes
        n0 = node(xA, yA, 0, 0)
        #print 'n0', n0
        n0.updatePriority(xB, yB)
        #print n0.updatePriority(xB, yB)
        heappush(pq[pqi], n0)
        open_nodes_map[yA][xA] = n0.priority # mark it on the open nodes map

        # A* search
        while len(pq[pqi]) > 0:
            # get the current node w/ the highest priority
            # from the list of open nodes
            n1 = pq[pqi][0] # top node
            n0 = node(n1.xPos, n1.yPos, n1.distance, n1.priority)
            x = n0.xPos
            y = n0.yPos
            heappop(pq[pqi]) # remove the node from the open list
            open_nodes_map[y][x] = 0
            closed_nodes_map[y][x] = 1 # mark it on the closed nodes map

            # quit searching when the goal is reached
            # if n0.estimate(xB, yB) == 0:
            if x == xB and y == yB:
                # generate the path from finish to start
                # by following the dirs
                path = ''
                while not (x == xA and y == yA):
                    j = dir_map[y][x]
                    c = str((j + dirs / 2) % dirs)
                    path = c + path
                    x += dx[j]
                    y += dy[j]
                return path

            # generate moves (child nodes) in all possible dirs
            for i in range(dirs):
                xdx = x + dx[i]
                ydy = y + dy[i]
                if not (xdx < 0 or xdx > n-1 or ydy < 0 or ydy > m - 1
                        or the_map[ydy][xdx] == 1 or closed_nodes_map[ydy][xdx] == 1):
                    # generate a child node
                    m0 = node(xdx, ydy, n0.distance, n0.priority)
                    m0.nextMove(dirs, i)
                    m0.updatePriority(xB, yB)
                    # if it is not in the open list then add into that
                    if open_nodes_map[ydy][xdx] == 0:
                        open_nodes_map[ydy][xdx] = m0.priority
                        heappush(pq[pqi], m0)
                        # mark its parent node direction
                        dir_map[ydy][xdx] = (i + dirs / 2) % dirs
                    elif open_nodes_map[ydy][xdx] > m0.priority:
                        # update the priority
                        open_nodes_map[ydy][xdx] = m0.priority
                        # update the parent direction
                        dir_map[ydy][xdx] = (i + dirs / 2) % dirs
                        # replace the node
                        # by emptying one pq to the other one
                        # except the node to be replaced will be ignored
                        # and the new node will be pushed in instead
                        while not (pq[pqi][0].xPos == xdx and pq[pqi][0].yPos == ydy):
                            heappush(pq[1 - pqi], pq[pqi][0])
                            heappop(pq[pqi])
                        heappop(pq[pqi]) # remove the target node
                        # empty the larger size priority queue to the smaller one
                        if len(pq[pqi]) > len(pq[1 - pqi]):
                            pqi = 1 - pqi
                        while len(pq[pqi]) > 0:
                            heappush(pq[1-pqi], pq[pqi][0])
                            heappop(pq[pqi])       
                        pqi = 1 - pqi
                        heappush(pq[pqi], m0) # add the better node instead
        return '' # if no route found
    
    def look_up_step(self, name):
        #rospy.init_node('step_listner')
        while not rospy.is_shutdown():
            try:
                step, _ = self.listener.lookupTransform('/enu', name, rospy.Time()) 
                return step
            except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
                continue





class node:
    def __init__(self, xPos, yPos, distance, priority):
        self.xPos = xPos
        self.yPos = yPos
        self.distance = distance
        self.priority = priority
    def __lt__(self, other): # comparison method for priority queue
        return self.priority < other.priority
    def updatePriority(self, xDest, yDest):
        self.priority = self.distance + self.estimate(xDest, yDest) * 10 # A*
    # give higher priority to going straight instead of diagonally
    def nextMove(self, dirs, d): # d: direction to move
        if dirs == 8 and d % 2 != 0:
            self.distance += 14
        else:
            self.distance += 10
    # Estimation function for the remaining distance to the goal.
    def estimate(self, xDest, yDest):
        xd = xDest - self.xPos
        yd = yDest - self.yPos
        # Euclidian Distance
        d = math.sqrt(xd * xd + yd * yd)
        # Manhattan distance
        # d = abs(xd) + abs(yd)
        # Chebyshev distance
        # d = max(abs(xd), abs(yd))
        return(d)


    
