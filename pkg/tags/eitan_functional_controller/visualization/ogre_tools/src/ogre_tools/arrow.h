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

#include "object.h"

#ifndef OGRE_TOOLS_ARROW_H
#define OGRE_TOOLS_ARROW_H

namespace Ogre
{
class SceneManager;
class SceneNode;
class Vector3;
class Quaternion;
class Any;
}

namespace ogre_tools
{
class SuperEllipsoid;
class Cone;

/**
 * \class Arrow
 * \brief An arrow object
 */
class Arrow : public Object
{
public:
  /**
   * \brief Constructor
   *
   * @param scene_manager The scene manager to use to construct any necessary objects
   * @param parent_node A scene node to use as the parent of this object.  If NULL, uses the root scene node.
   * @param shaft_length Length of the arrow's shaft
   * @param shaft_radius Radius of the arrow's shaft
   * @param head_length Length of the arrow's head
   * @param head_radius Radius of the arrow's head
   */
  Arrow( Ogre::SceneManager* scene_manager, Ogre::SceneNode* parent_node = 0, float shaft_length = 1.0f, float shaft_radius = 0.1f,
      float head_length = 0.3f, float head_radius =  0.2f );
  virtual ~Arrow();

  /**
   * \brief Set the parameters for this arrow
   *
   * @param shaft_length Length of the arrow's shaft
   * @param shaft_radius Radius of the arrow's shaft
   * @param head_length Length of the arrow's head
   * @param head_radius Radius of the arrow's head
   */
  void set( float shaft_length, float shaft_radius, float head_length, float head_radius );

  /**
   * \brief Set the color of this arrow.  Sets both the head and shaft color to the same value.  Values are in the range [0, 1]
   *
   * @param r Red component
   * @param g Green component
   * @param b Blue component
   */
  virtual void setColor( float r, float g, float b, float a );

  /**
   * \brief Set the color of the arrow's head.  Values are in the range [0, 1]
   *
   * @param r Red component
   * @param g Green component
   * @param b Blue component
   */
  void setHeadColor( float r, float g, float b, float a = 1.0f );
  /**
   * \brief Set the color of the arrow's shaft.  Values are in the range [0, 1]
   *
   * @param r Red component
   * @param g Green component
   * @param b Blue component
   */
  void setShaftColor( float r, float g, float b, float a = 1.0f );

  virtual void setOrientation( const Ogre::Quaternion& orientation );
  virtual void setPosition( const Ogre::Vector3& position );
  virtual void setScale( const Ogre::Vector3& scale );
  virtual const Ogre::Vector3& getPosition();
  virtual const Ogre::Quaternion& getOrientation();

  /**
   * \brief Get the scene node associated with this arrow
   * @return the scene node associated with this arrow
   */
  Ogre::SceneNode* getSceneNode() { return scene_node_; }

  /**
   * \brief Sets user data on all ogre objects we own
   */
  void setUserData( const Ogre::Any& data );

private:
  Ogre::SceneNode* scene_node_;

  SuperEllipsoid* shaft_;   ///< Cylinder used for the shaft of the arrow
  Cone* head_;              ///< Cone used for the head of the arrow
};

} // namespace ogre_tools

#endif /* OGRE_TOOLS_ARROW_H */
