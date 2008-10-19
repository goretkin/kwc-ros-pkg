/*
 * Copyright (c) 2008, Willow Garage, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Willow Garage, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/* Author: Brian Gerkey */

#include "slam_gmapping.h"

#include <iostream>

#include <time.h>

#include "rosconsole/rosconsole.h"

#include "sensor/sensor_range/rangesensor.h"
#include "sensor/sensor_odometry/odometrysensor.h"

// compute linear index for given map coords
#define MAP_IDX(sx, i, j) ((sx) * (j) + (i))

SlamGMapping::SlamGMapping() 
{
  log4cxx::Logger::getLogger(ROSCONSOLE_DEFAULT_NAME)->setLevel(ros::console::g_level_lookup[ros::console::levels::Debug]);

  gsp_ = new GMapping::GridSlamProcessor(std::cerr);
  ROS_ASSERT(gsp_);

  node_ = new ros::node("gmapping");
  ROS_ASSERT(gsp_);

  /// @todo Disable extrapolation, and implement scan-buffering
  tf_ = new tf::TransformListener(*node_, true, 10000000000ULL, 200000000ULL);
  ROS_ASSERT(tf_);

  gsp_laser_ = NULL;
  gsp_odom_ = NULL;

  got_first_scan_ = false;

  node_->subscribe("base_scan", scan_, &SlamGMapping::laser_cb, this, 1);
  node_->advertise_service("dynamic_map", &SlamGMapping::map_cb, this);
}

SlamGMapping::~SlamGMapping()
{
  delete gsp_;
  if(gsp_laser_)
    delete gsp_laser_;
  if(gsp_odom_)
    delete gsp_odom_;
  if(tf_)
    delete tf_;
  delete node_;
}

bool
SlamGMapping::initMapper(const std_msgs::LaserScan& scan)
{
  // @todo Get the laser's pose, relative to base.
  tf::Stamped<tf::Pose> ident;
  tf::Stamped<btTransform> laser_pose;
  ident.data_.setIdentity();
  ident.frame_id_ = "base_laser";
  ident.stamp_ = scan.header.stamp;
  try
  {
    this->tf_->transformPose("base", ident, laser_pose);
  }
  catch(tf::TransformException e)
  {
    ROS_WARN("Failed to compute laser pose, aborting initialization (%s)", 
             e.what());
    return false;
  }
  double yaw,pitch,roll;
  btMatrix3x3 mat =  laser_pose.data_.getBasis();
  mat.getEulerZYX(yaw, pitch, roll);

  GMapping::OrientedPoint gmap_pose(laser_pose.data_.getOrigin().x(),
                                    laser_pose.data_.getOrigin().y(),
                                    yaw);
  ROS_DEBUG("laser's pose wrt base: %.3f %.3f %.3f",
            laser_pose.data_.getOrigin().x(),
            laser_pose.data_.getOrigin().y(),
            yaw);

  // The laser must be called "FLASER"
  gsp_laser_ = new GMapping::RangeSensor("FLASER",
                                         scan.ranges.size(),
                                         scan.angle_increment,
                                         gmap_pose,
                                         0.0,
                                         scan.range_max);
  ROS_ASSERT(gsp_laser_);

  GMapping::SensorMap smap;
  smap.insert(make_pair(gsp_laser_->getName(), gsp_laser_));
  gsp_->setSensorMap(smap);

  gsp_odom_ = new GMapping::OdometrySensor("odom");
  ROS_ASSERT(gsp_odom_);

  /// @todo Expose the mapper's params
  //double maxUrange = 80.0;
  double maxUrange = 16.0;
  //double maxrange = 80.0;
  double maxrange = 30.0;
  double sigma = 0.05;
  int kernelSize = 1;
  double lstep = 0.05;
  double astep = 0.05;
  int iterations = 5;
  double lsigma = 0.075;
  double ogain = 3.0;
  //unsigned int lskip = 0;
  unsigned int lskip = 10;
  double srr = 0.1;
  double srt = 0.1;
  double str = 0.1;
  double stt = 0.1;
  double linearUpdate = 1.0;
  double angularUpdate = 0.5;
  double resampleThreshold = 0.5;
  /// @todo Check the default on generateMap
  bool generateMap = false;
  int particles = 30;
  double xmin = -100.0;
  double ymin = -100.0;
  double xmax = 100.0;
  double ymax = 100.0;
  double delta = 0.05;
  GMapping::OrientedPoint initialPose(0.0, 0.0, 0.0);
  double llsamplerange = 0.01;
  double llsamplestep = 0.01;
  double lasamplerange = 0.005;
  double lasamplestep = 0.005;

  gsp_->setMatchingParameters(maxUrange, maxrange, sigma, 
                              kernelSize, lstep, astep, iterations, 
                              lsigma, ogain, lskip);

  gsp_->setMotionModelParameters(srr, srt, str, stt);
  gsp_->setUpdateDistances(linearUpdate, angularUpdate, resampleThreshold);
  gsp_->setgenerateMap(generateMap);
  gsp_->GridSlamProcessor::init(particles, xmin, ymin, xmax, ymax, 
                                delta, initialPose);
  gsp_->setllsamplerange(llsamplerange);
  gsp_->setllsamplestep(llsamplestep);
  /// @todo Check these calls; in the gmapping gui, they use
  /// llsamplestep and llsamplerange intead of lasamplestep and
  /// lasamplerange.  It was probably a typo, but who knows.
  gsp_->setlasamplerange(lasamplerange);
  gsp_->setlasamplestep(lasamplestep);

  // Call the sampling function once to set the seed.
  GMapping::sampleGaussian(1,time(NULL));

  ROS_INFO("Initialization complete");

  return true;
}

bool
SlamGMapping::addScan(const std_msgs::LaserScan& scan)
{
  // GMapping wants an array of doubles...
  double* ranges_double = new double[scan.ranges.size()];
  for(unsigned int i; i < scan.ranges.size(); i++)
    ranges_double[i] = (double)scan.ranges[i];

  GMapping::RangeReading reading(scan.ranges.size(),
                                 ranges_double,
                                 gsp_laser_, 
                                 scan.header.stamp.toSec());
  // ...but it deep copies them, so we don't need to keep our array around.
  delete[] ranges_double;

  // Get the robot's pose 
  tf::Stamped<tf::Pose> ident;
  tf::Stamped<btTransform> odom_pose;
  ident.data_.setIdentity();
  ident.frame_id_ = "base";
  ident.stamp_ = scan.header.stamp;
  try
  {
    this->tf_->transformPose("odom", ident, odom_pose);
  }
  catch(tf::TransformException e)
  {
    ROS_WARN("Failed to compute odom pose, skipping scan (%s)", e.what());
    return false;
  }
  double yaw,pitch,roll;
  btMatrix3x3 mat =  odom_pose.data_.getBasis();
  mat.getEulerZYX(yaw, pitch, roll);

  GMapping::OrientedPoint gmap_pose(odom_pose.data_.getOrigin().x(),
                                    odom_pose.data_.getOrigin().y(),
                                    yaw);
  reading.setPose(gmap_pose);

  /*
  ROS_DEBUG("scan pose (%.3f): %.3f %.3f %.3f",
            scan.header.stamp.toSec(),
            odom_pose.data_.getOrigin().x(),
            odom_pose.data_.getOrigin().y(),
            yaw);
            */

  return gsp_->processScan(reading);
}

void
SlamGMapping::laser_cb()
{
  // We can't initialize the mapper until we've got the first scan
  if(!got_first_scan_)
  {
    if(!initMapper(scan_))
      return;
    got_first_scan_ = true;
  }

  if(addScan(scan_))
  {
    ROS_DEBUG("scan processed");
    
    GMapping::OrientedPoint mpose = gsp_->getParticles()[gsp_->getBestParticleIndex()].pose;
    ROS_DEBUG("new best pose: %.3f %.3f %.3f", 
              mpose.x, mpose.y, mpose.theta);
    /*
    const GMapping::ScanMatcherMap* map = &(gsp_->getParticles()[gsp_->getBestParticleIndex()].map);

    GMapping::Point wmin(-5.0, -5.0);
    GMapping::Point wmax(5.0, 5.0);
    GMapping::IntPoint imin = map->world2map(wmin);
    GMapping::IntPoint imax = map->world2map(wmax);

    map_.map.width = imax.x - imin.x;
    map_.map.height = imax.y - imin.y;
    // Same as delta
    map_.map.resolution = 0.05;
    map_.map.origin.x = wmin.x;
    map_.map.origin.y = wmin.y;
    map_.map.origin.th = 0.0;
    map_.map.data.resize(map_.map.width * map_.map.height);

    for(int x=0; x < (imax.x - imin.x); x++)
    {
      for(int y=0; y < (imax.y - imin.y); y++)
      {
        GMapping::IntPoint p(imin.x + x, imin.y + y);
        double v=map->cell(p);
        printf("%.3f ", v);
        if(v >= 0)
        {
          double occ = (255.0 * v);

          if(occ > 0.5)
            map_.map.data[MAP_IDX(map_.map.width, x, y)] = 100;
          else if(occ < 0.1)
            map_.map.data[MAP_IDX(map_.map.width, x, y)] = 0;
          else
            map_.map.data[MAP_IDX(map_.map.width, x, y)] = -1;
        }
      }
    }
    puts("");

    got_map_ = true;
    */
  }
}

bool 
SlamGMapping::map_cb(std_srvs::StaticMap::request  &req,
                     std_srvs::StaticMap::response &res)
{
  if(got_map_)
  {
    res = map_;
    return true;
  }
  else
    return false;
}
