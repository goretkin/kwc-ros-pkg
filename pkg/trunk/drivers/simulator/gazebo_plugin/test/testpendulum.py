#!/usr/bin/env python
# Software License Agreement (BSD License)
#
# Copyright (c) 2008, Willow Garage, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above
#    copyright notice, this list of conditions and the following
#    disclaimer in the documentation and/or other materials provided
#    with the distribution.
#  * Neither the name of the Willow Garage nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

## Gazebo test Pendulum physics
##          see error in joint constraint for ODE

PKG = 'gazebo_plugin'
NAME = 'testpendulum'

import math
import rostools
rostools.update_path(PKG)
rostools.update_path('rostest')
rostools.update_path('std_msgs')
rostools.update_path('robot_msgs')
rostools.update_path('rostest')
rostools.update_path('rospy')


import sys, unittest
import os, os.path, threading, time
import rospy, rostest
from std_msgs.msg import *

class E:
    def __init__(self,x,y,z):
        self.x = x
        self.y = y
        self.z = z

class Q:
    def __init__(self,x,y,z,u):
        self.x = x
        self.y = y
        self.z = z
        self.u = u
    def normalize(self):
        s = math.sqrt(self.u * self.u + self.x * self.x + self.y * self.y + self.z * self.z)
        self.u /= s
        self.x /= s
        self.y /= s
        self.z /= s
    def getEuler(self):
        self.normalize()
        squ = self.u
        sqx = self.x
        sqy = self.y
        sqz = self.z
        # init euler angles
        vec = E(0,0,0)
        # Roll
        vec.x = math.atan2(2 * (self.y*self.z + self.u*self.x), squ - sqx - sqy + sqz);
        # Pitch
        vec.y = math.asin(-2 * (self.x*self.z - self.u*self.y));
        # Yaw
        vec.z = math.atan2(2 * (self.x*self.y + self.u*self.z), squ + sqx - sqy - sqz);
        return vec


class PendulumTest(unittest.TestCase):
    def __init__(self, *args):
        super(PendulumTest, self).__init__(*args)
        self.success = False
        self.reached_target_vw = False
        self.duration_start = 0
        self.error1_total = 0
        self.error1_count = 0
        self.error2_total = 0
        self.error2_count = 0
        self.error1_max = 0
        self.error2_max = 0
        self.tolerance    = 0.01  # tolerance: 1cm error
        self.min_runs     = 1000  # average error over this many runs



    #def printPendulum(self, p3d):
        #print "P3D pose translan: " + "x: " + str(p3d.transform.translation.x)
        #print "                   " + "y: " + str(p3d.transform.translation.y)
        #print "                   " + "z: " + str(p3d.transform.translation.z)
        #print "P3D pose rotation: " + "x: " + str(p3d.transform.rotation.x)
        #print "                   " + "y: " + str(p3d.transform.rotation.y)
        #print "                   " + "z: " + str(p3d.transform.rotation.z)
        #print "                   " + "w: " + str(p3d.transform.rotation.w)
        #print "P3D rate translan: " + "x: " + str(p3d.rate.translation.x)
        #print "                   " + "y: " + str(p3d.rate.translation.y)
        #print "                   " + "z: " + str(p3d.rate.translation.z)
        #print "P3D rate rotation: " + "x: " + str(p3d.rate.rotation.x)
        #print "                   " + "y: " + str(p3d.rate.rotation.y)
        #print "                   " + "z: " + str(p3d.rate.rotation.z)
        #print "                   " + "w: " + str(p3d.rate.rotation.w)


    def p3dInput1(self, p3d):
        #print "link1 pose ground truth received"
        #self.printPendulum(p3d)
        tmpx = p3d.transform.translation.x
        tmpz = p3d.transform.translation.z - 2.0
        #print "link1 origin (" + str(tmpx) + " , " + str(tmpz) + ")"
        self.error1_total += math.sqrt(tmpx*tmpx+tmpz*tmpz)
        self.error1_count += 1
        if math.sqrt(tmpx*tmpx+tmpz*tmpz) > self.error1_max:
            self.error1_max =  math.sqrt(tmpx*tmpx+tmpz*tmpz)
 
    def p3dInput2(self, p3d):
        #print "link2 pose ground truth received"
        #self.printPendulum(p3d)
        q = Q(p3d.transform.rotation.x , p3d.transform.rotation.y , p3d.transform.rotation.z , p3d.transform.rotation.w)
        q.normalize()
        v = q.getEuler()

        #FIXME: something wrong with the transform, need to fix it.  abs masks the problem for now.
        #FIXME: something wrong with the transform, need to fix it.  abs masks the problem for now.
        #FIXME: something wrong with the transform, need to fix it.  abs masks the problem for now.
        #FIXME: something wrong with the transform, need to fix it.  abs masks the problem for now.
        tmpx = abs(p3d.transform.translation.x) +0.0 - abs(math.cos(v.z)*math.cos(v.y))
        tmpz = abs(p3d.transform.translation.z) -2.0 + abs(math.sin(v.y))
        #math.cos(v.x)*math.cos(v.z)-math.cos(v.x)*math.sin(v.y)*math.cos(v.z))
        #print "link2 origin (" + str(tmpx) + " , " + str(tmpz) + ")"
        #print "link2 raw    (" + str(p3d.transform.translation.x) + " , " + str(p3d.transform.translation.z) + ") total: " + str(self.error2_total)
        #print "link2 correc (" + str(math.cos(v.y)) + " , " + str(math.sin(v.y)) + ") angle: " + str(v.x) +","+str(v.y)+","+str(v.z)

        self.error2_total += math.sqrt(tmpx*tmpx+tmpz*tmpz)
        self.error2_count += 1
        if math.sqrt(tmpx*tmpx+tmpz*tmpz) > self.error2_max:
            self.error2_max =  math.sqrt(tmpx*tmpx+tmpz*tmpz)
    
    def test_pendulum(self):
        print "LNK\n"
        rospy.Subscriber("link1_pose", TransformWithRateStamped, self.p3dInput1)
        rospy.Subscriber("link2_pose", TransformWithRateStamped, self.p3dInput2)
        rospy.init_node(NAME, anonymous=True)
        timeout_t = time.time() + 20.0
        while not rospy.is_shutdown() and not self.success and time.time() < timeout_t:
            print "E1 " + str(self.error1_count) + ":" + str(self.error1_total) + " E2 " + str(self.error2_count) + ":" + str(self.error2_total)
            if self.error2_count > self.min_runs and self.error1_count > self.min_runs:
              if self.error1_total / self.error1_count < self.tolerance and self.error2_total / self.error2_count < self.tolerance:
                if self.error1_max < self.tolerance and self.error2_max < self.tolerance:
                  self.success = True
            time.sleep(0.1)
        self.assert_(self.success)
        
if __name__ == '__main__':
    rostest.run(PKG, sys.argv[0], PendulumTest, sys.argv) #, text_mode=True)


