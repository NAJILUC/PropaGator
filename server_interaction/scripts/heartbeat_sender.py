#!/usr/bin/env python
import rospy
import roslib
roslib.load_manifest('server_interaction')
import sys
import requests
import json
from datetime import datetime
from time import strftime
from std_msgs.msg import String
from nav_msgs.msg import Odometry
import time
import math
from collections import OrderedDict

## This node just sends the heartbeat to the server.

def StoreMainServerUrl(serverUrl):
	#get the server url to send requests
	global mainUrl
	mainUrl = serverUrl.data

def StoreCourseInfo(courseInfo):
	#get the course information e.g. courseA
	global course	
	course = courseInfo.data

def GetCurrentTask(current_challenge):
	#get the nameof the challenge that is currently being attempted
	global currentChallenge
	currentChallenge = current_challenge.data

#convert ecef to latitude and longitude
def ecef2lla(ecef):
	a = 6378137 # earths's radius in meters
	e = 8.1819190842622e-2  # eccentricity
	asq = math.pow(a,2)
	esq = math.pow(e,2)
	x = ecef[0]
	y = ecef[1]
	z = ecef[2]
	b = math.sqrt( asq * (1-esq) )
	bsq = math.pow(b,2)
	ep = math.sqrt( (asq - bsq)/bsq)
	p = math.sqrt( math.pow(x,2) + math.pow(y,2) )
	th = math.atan2(a*z, b*p)
	lon = math.atan2(y,x)
	lat = math.atan2( (z + math.pow(ep,2)*b*math.pow(math.sin(th),3) ), (p - esq*a*math.pow(math.cos(th),3)) )
	N = a/( math.sqrt(1-esq*math.pow(math.sin(lat),2)) )
	alt = p / math.cos(lat) - N
	#mod lat to 0-2pi
	lon = lon % (2*math.pi)
	#correction for altitude near poles left out.
	#changing radians to degrees
	lat = math.degrees(lat)
	lon = math.degrees(lon)
	#normalizing angle
	lat = normalizeAngle(lat)
	lon = normalizeAngle(lon)
	ret = (lat, lon)
	return ret;

def normalizeAngle(angle):
	#normalize angle to keep it between -179 and 180 degrees
	newAngle = angle	
	while newAngle <= -180: 
		newAngle += 360	
	while newAngle > 180: 
		newAngle -= 360	
	return newAngle;	

def GetGpsData(gpsPos):
	#getting gps data from boat in ecef coordinates (x,y,z)
	global x,y,z
	x = gpsPos.pose.pose.position.x
	y = gpsPos.pose.pose.position.y
	z = gpsPos.pose.pose.position.z
def sendHeartBeat(blah):	
	ecef = (x,y,z)
	global gpsData
	gpsData = ecef2lla(ecef)
	# print "X data: "
	# print x
	# print(" ")
	# print "Y data: "
	# print y
	# print(" ")
	# print "Z data: "
	# print z
	# print(" ")	
	timeStamp = datetime.utcnow()
	timeStamp = timeStamp.strftime('%Y%m%d%H%M%S')
	latitude = str(gpsData[0])
	longitude = str(gpsData[1])
	latitude = latitude[0:9]
	longitude = longitude[0:10]	
	sublinkMain = '/heartbeat/%s/UF' %course
	url = mainUrl +  sublinkMain
	#payload to send to server
	time.sleep(1)
	position = OrderedDict([("datum","WGS84"),("latitude",latitude),("longitude",longitude)])
	payload = OrderedDict([("timestamp",timeStamp),("challenge",currentChallenge),("position",position)])
	heartbeat_pub = rospy.Publisher('gps_heartbeat', String, queue_size=10)
	headers = {'content-type': 'application/json'}
	print "Payload sent to server: "
	print json.dumps(payload)					
	r = requests.post(url, headers = headers, data = json.dumps(payload), verify = False)
	if r.status_code == 200:
		print r.text
		heartBeatStatus = "success"
		rospy.loginfo(heartBeatStatus)
		heartbeat_pub.publish(heartBeatStatus)
	else:
		raise rospy.ServiceException('Bad Request')

def main():
	
	rospy.init_node('heartbeat')
	rospy.Subscriber('main_server_url', String, StoreMainServerUrl)
	rospy.Subscriber('absodom', Odometry, GetGpsData)
	rospy.Subscriber('course_code', String, StoreCourseInfo)
	rospy.Subscriber('course_code', String, sendHeartBeat)
	rospy.Subscriber('current_challenge', String, GetCurrentTask)	
	rospy.spin()

if __name__ == '__main__':	
	try:	
		main()
	except rospy.ROSInterruptException:
		pass
