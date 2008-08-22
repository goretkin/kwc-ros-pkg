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

// DO NOT INCLUDE THIS FROM WITHIN ANOTHER HEADER, AS IT WILL PULL IN ALL OF OGRE

#ifndef OGRE_VISUALIZER_COMMON_H
#define OGRE_VISUALIZER_COMMON_H

#include <Ogre.h>

namespace ogre_vis
{

extern Ogre::Matrix3 g_ogre_to_robot_matrix;
extern Ogre::Matrix3 g_robot_to_ogre_matrix;

extern Ogre::Quaternion g_ogre_to_robot_quat;
extern Ogre::Quaternion g_robot_to_ogre_quat;

void initializeCommon();

inline void robotToOgre( Ogre::Vector3& point )
{
  point = g_robot_to_ogre_matrix * point;
}

inline void robotToOgre( Ogre::Quaternion& quat )
{
  quat = g_robot_to_ogre_quat * quat;
}

inline void robotToOgre( Ogre::Matrix3& mat )
{
  mat = g_robot_to_ogre_matrix * mat;
}



inline void ogreToRobot( Ogre::Vector3& point )
{
  point = g_ogre_to_robot_matrix * point;
}

inline void ogreToRobot( Ogre::Quaternion& quat )
{
  quat = g_ogre_to_robot_quat * quat;
}

inline void ogreToRobot( Ogre::Matrix3& mat )
{
  mat = g_ogre_to_robot_matrix * mat;
}


inline Ogre::Matrix3 ogreMatrixFromRobotEulers( float yaw, float pitch, float roll )
{
  Ogre::Matrix3 mat;
  mat.FromEulerAnglesZYX( Ogre::Radian( yaw ), Ogre::Radian( pitch ), Ogre::Radian( roll ) );
  robotToOgre( mat );

  return mat;
}

}
#endif
