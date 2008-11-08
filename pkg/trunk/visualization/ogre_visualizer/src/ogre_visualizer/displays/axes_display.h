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

#ifndef OGRE_VISUALIZER_AXES_DISPLAY_H
#define OGRE_VISUALIZER_AXES_DISPLAY_H

#include "display.h"

namespace ogre_tools
{
class Axes;
}

namespace ogre_vis
{

class FloatProperty;

/**
 * \class AxesDisplay
 * \brief Displays a set of XYZ axes at the origin
 */
class AxesDisplay : public Display
{
public:
  AxesDisplay( const std::string& name, VisualizationManager* manager );
  virtual ~AxesDisplay();

  /**
   * \brief Set the parameters for the axes
   * @param length Length of each axis
   * @param radius Radius of each axis
   */
  void set( float length, float radius );

  void setLength( float length );
  float getLength() { return length_; }
  void setRadius( float radius );
  float getRadius() { return radius_; }

  // Overrides from Display
  virtual void targetFrameChanged() {}
  virtual void fixedFrameChanged() {}
  virtual void createProperties();

  static const char* getTypeStatic() { return "Axes"; }
  virtual const char* getType() { return getTypeStatic(); }
  static const char* getDescription();

protected:
  /**
   * \brief Create the axes with the current parameters
   */
  void create();

  // overrides from Display
  virtual void onEnable();
  virtual void onDisable();

  float length_;                ///< Length of each axis
  float radius_;                ///< Radius of each axis
  ogre_tools::Axes* axes_;      ///< Handles actually drawing the axes

  FloatProperty* length_property_;
  FloatProperty* radius_property_;
};

} // namespace ogre_vis

 #endif