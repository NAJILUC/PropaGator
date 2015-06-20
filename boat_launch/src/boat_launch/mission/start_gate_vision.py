import numpy
import math
from txros import util
import cv2
import boat_scripting

@util.cancellableInlineCallbacks
def main(nh, boat=None):
    print "Q"
    if boat is None:
        boat = yield boat_scripting.get_boat(nh)
    
    case = True
    while case == True:
        
        print 'find gates'
        angle = yield boat.get_start_gate_vision()    
        print "raw angle", angle
        

        yield boat.move.as_MoveToGoal([3,0,0],angle).go()

        '''
        if abs(angle) < (numpy.pi/12):
            print "forward"
            yield boat.move.forward(2).go()                       
            print "forward command sent"
        elif angle < 0:
            print "turn_left: ", angle/2
            yield boat.move.turn_left(abs(angle/2)).go()
        elif angle > 0:
            print "turn_right: ", angle/2
            yield boat.move.turn_right(angle/2).go()
        
        print 'left'
        yield boat.move.turn_left(abs(numpy.pi/3)).go()

        print 'right'
        yield boat.move.turn_right(numpy.pi/3).go()
        '''