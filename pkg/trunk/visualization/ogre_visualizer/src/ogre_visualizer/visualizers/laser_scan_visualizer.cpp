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

#include "laser_scan_visualizer.h"
#include "../common.h"
#include "../ros_topic_property.h"

#include "ros/node.h"
#include "ogre_tools/point_cloud.h"

#include <rosTF/rosTF.h>

#include <Ogre.h>
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#define SCAN_TOPIC_PROPERTY wxT("Scan Topic")
#define CLOUD_TOPIC_PROPERTY wxT("Cloud Topic")
#define COLOR_PROPERTY wxT("Color")
#define DECAY_TIME_PROPERTY wxT("Decay Time")
#define STYLE_PROPERTY wxT("Style")
#define BILLBOARD_SIZE_PROPERTY wxT("Billboard Size")

namespace ogre_vis
{

LaserScanVisualizer::LaserScanVisualizer( Ogre::SceneManager* sceneManager, ros::node* node, rosTFClient* tfClient, const std::string& name, bool enabled )
: VisualizerBase( sceneManager, node, tfClient, name, enabled )
, r_( 1.0 )
, g_( 0.0 )
, b_( 0.0 )
, intensity_min_( 999999.0f )
, intensity_max_( -999999.0f )
, point_decay_time_( 20.0f )
, style_( Billboards )
, billboard_size_( 0.003 )
{
  cloud_ = new ogre_tools::PointCloud( scene_manager_ );

  SetStyle( style_ );
  SetBillboardSize( billboard_size_ );

  if ( IsEnabled() )
  {
    OnEnable();
  }
}

LaserScanVisualizer::~LaserScanVisualizer()
{
  Unsubscribe();

  delete cloud_;
}

void LaserScanVisualizer::SetCloudTopic( const std::string& topic )
{
  Unsubscribe();

  cloud_topic_ = topic;

  Subscribe();
}

void LaserScanVisualizer::SetScanTopic( const std::string& topic )
{
  Unsubscribe();

  scan_topic_ = topic;

  Subscribe();
}

void LaserScanVisualizer::SetColor( float r, float g, float b )
{
  r_ = r;
  g_ = g;
  b_ = b;
}

void LaserScanVisualizer::SetStyle( Style style )
{
  {
    RenderAutoLock renderLock( this );

    style_ = style;
    cloud_->SetUsePoints( style == Points );
  }

  CauseRender();
}

void LaserScanVisualizer::SetBillboardSize( float size )
{
  {
    RenderAutoLock renderLock( this );

    billboard_size_ = size;
    cloud_->SetBillboardDimensions( size, size );
  }

  CauseRender();
}

void LaserScanVisualizer::OnEnable()
{
  cloud_->SetVisible( true );
  Subscribe();
}

void LaserScanVisualizer::OnDisable()
{
  Unsubscribe();

  cloud_->SetVisible( false );
  cloud_->Clear();
  points_.clear();
  point_times_.clear();
}

void LaserScanVisualizer::Subscribe()
{
  if ( !IsEnabled() )
  {
    return;
  }

  if ( !cloud_topic_.empty() )
  {
    ros_node_->subscribe( cloud_topic_, cloud_message_, &LaserScanVisualizer::IncomingCloudCallback, this, 1 );
  }

  if ( !scan_topic_.empty() )
  {
    ros_node_->subscribe( scan_topic_, scan_message_, &LaserScanVisualizer::IncomingScanCallback, this, 1 );
  }
}

void LaserScanVisualizer::Unsubscribe()
{
  if ( !cloud_topic_.empty() )
  {
    ros_node_->unsubscribe( cloud_topic_ );
  }

  if ( !scan_topic_.empty() )
  {
    ros_node_->unsubscribe( scan_topic_ );
  }
}

void LaserScanVisualizer::Update( float dt )
{
  cloud_message_.lock();

  D_float::iterator it = point_times_.begin();
  D_float::iterator end = point_times_.end();
  for ( ; it != end; ++it )
  {
    *it += dt;
  }

  CullPoints();

  cloud_message_.unlock();
}

void LaserScanVisualizer::CullPoints()
{
  if ( point_decay_time_ == 0.0f )
  {
    return;
  }

  while ( !point_times_.empty() && point_times_.front() > point_decay_time_ )
  {
    point_times_.pop_front();
    points_.pop_front();
  }
}

void LaserScanVisualizer::TransformCloud()
{
  if ( cloud_message_.header.frame_id.empty() )
  {
    cloud_message_.header.frame_id = target_frame_;
  }

  try
  {
    tf_client_->transformPointCloud(target_frame_, cloud_message_, cloud_message_);
  }
  catch(libTF::TransformReference::LookupException& e)
  {
    printf( "Error transforming laser scan '%s': %s\n", name_.c_str(), e.what() );
  }
  catch(libTF::TransformReference::ConnectivityException& e)
  {
    printf( "Error transforming laser scan '%s': %s\n", name_.c_str(), e.what() );
  }
  catch(libTF::TransformReference::ExtrapolateException& e)
  {
    printf( "Error transforming laser scan '%s': %s\n", name_.c_str(), e.what() );
  }

  uint32_t pointCount = cloud_message_.get_pts_size();
  for(uint32_t i = 0; i < pointCount; i++)
  {
    float& intensity = cloud_message_.chan[0].vals[i];
    // arbitrarily cap to 4096 for now
    intensity = std::min( intensity, 4096.0f );
    intensity_min_ = std::min( intensity_min_, intensity );
    intensity_max_ = std::max( intensity_max_, intensity );
  }

  float diffIntensity = intensity_max_ - intensity_min_;

  if ( point_decay_time_ == 0.0f )
  {
    points_.clear();
    point_times_.clear();
  }

  points_.push_back( V_Point() );
  V_Point& points = points_.back();
  points.resize( pointCount );

  point_times_.push_back( 0.0f );
  for(uint32_t i = 0; i < pointCount; i++)
  {
    Ogre::Vector3 point( cloud_message_.pts[i].x, cloud_message_.pts[i].y, cloud_message_.pts[i].z );
    RobotToOgre( point );

    float intensity = cloud_message_.chan[0].vals[i];

    float normalizedIntensity = (diffIntensity > 0.0f) ? ( intensity - intensity_min_ ) / diffIntensity : 1.0f;

    Ogre::Vector3 color( r_, g_, b_ );
    color *= normalizedIntensity;

    ogre_tools::PointCloud::Point& currentPoint = points[ i ];
    currentPoint.m_X = point.x;
    currentPoint.m_Y = point.y;
    currentPoint.m_Z = point.z;
    currentPoint.r_ = color.x;
    currentPoint.g_ = color.y;
    currentPoint.b_ = color.z;
  }

  {
    RenderAutoLock renderLock( this );

    cloud_->Clear();

    if ( !points_.empty() )
    {
      DV_Point::iterator it = points_.begin();
      DV_Point::iterator end = points_.end();
      for ( ; it != end; ++it )
      {
        V_Point& points = *it;

        if ( !points.empty() )
        {
          cloud_->AddPoints( &points.front(), points.size() );
        }
      }
    }
  }

  CauseRender();
}

void LaserScanVisualizer::IncomingCloudCallback()
{
  cloud_message_.lock();

  TransformCloud();

  cloud_message_.unlock();
}

void LaserScanVisualizer::IncomingScanCallback()
{
  cloud_message_.lock();

  if ( scan_message_.header.frame_id.empty() )
  {
    scan_message_.header.frame_id = target_frame_;
  }

  laser_projection_.projectLaser( scan_message_, cloud_message_ );
  TransformCloud();

  cloud_message_.unlock();
}

void LaserScanVisualizer::FillPropertyGrid( wxPropertyGrid* propertyGrid )
{
  wxArrayString styleNames;
  styleNames.Add( wxT("Billboards") );
  styleNames.Add( wxT("Points") );
  wxArrayInt styleIds;
  styleIds.Add( Billboards );
  styleIds.Add( Points );

  propertyGrid->Append( new wxEnumProperty( STYLE_PROPERTY, wxPG_LABEL, styleNames, styleIds, style_ ) );

  propertyGrid->Append( new ROSTopicProperty( ros_node_, SCAN_TOPIC_PROPERTY, wxPG_LABEL, wxString::FromAscii( scan_topic_.c_str() ) ) );
  propertyGrid->Append( new ROSTopicProperty( ros_node_, CLOUD_TOPIC_PROPERTY, wxPG_LABEL, wxString::FromAscii( cloud_topic_.c_str() ) ) );
  propertyGrid->Append( new wxColourProperty( COLOR_PROPERTY, wxPG_LABEL, wxColour( r_ * 255, g_ * 255, b_ * 255 ) ) );
  wxPGId prop = propertyGrid->Append( new wxFloatProperty( DECAY_TIME_PROPERTY, wxPG_LABEL, point_decay_time_ ) );

  propertyGrid->SetPropertyAttribute( prop, wxT("Min"), 0.0 );

  prop = propertyGrid->Append( new wxFloatProperty( BILLBOARD_SIZE_PROPERTY, wxPG_LABEL, billboard_size_ ) );
  propertyGrid->SetPropertyAttribute( prop, wxT("Min"), 0.0 );
}

void LaserScanVisualizer::PropertyChanged( wxPropertyGridEvent& event )
{
  wxPGProperty* property = event.GetProperty();

  const wxString& name = property->GetName();
  wxVariant value = property->GetValue();

  if ( name == SCAN_TOPIC_PROPERTY )
  {
    wxString topic = value.GetString();
    SetScanTopic( std::string(topic.char_str()) );
  }
  else if ( name == CLOUD_TOPIC_PROPERTY )
  {
    wxString topic = value.GetString();
    SetCloudTopic( std::string(topic.char_str()) );
  }
  else if ( name == COLOR_PROPERTY )
  {
    wxColour color;
    color << value;

    SetColor( color.Red() / 255.0f, color.Green() / 255.0f, color.Blue() / 255.0f );
  }
  else if ( name == DECAY_TIME_PROPERTY )
  {
    float val = value.GetDouble();
    SetDecayTime( val );
  }
  else if ( name == STYLE_PROPERTY )
  {
    int val = value.GetLong();
    SetStyle( (Style)val );
  }
  else if ( name == BILLBOARD_SIZE_PROPERTY )
  {
    float val = value.GetDouble();
    SetBillboardSize( val );
  }
}

} // namespace ogre_vis
