/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

#include <ros/node.h>
#include <ros/time.h>
#include <urdf/URDF.h>
#include <planning_models/kinematic.h>
#include <std_msgs/RobotBase2DOdom.h>
#include <rosTF/rosTF.h>

namespace planning_node_util
{
    
    class NodeWithRobotModel : public ros::node
    {

    public:
	
        NodeWithRobotModel(const std::string &robot_model, const std::string &name, uint32_t options = 0) : ros::node(name, options),
	                                                                                                    m_tf(*this, true, 1 * 1000000000ULL, 1000000000ULL)
	{
	    m_urdf = NULL;
	    m_kmodel = NULL;
	    m_basePos[0] = m_basePos[1] = m_basePos[2] = 0.0;
	    loadRobotDescription(robot_model);
	    
	    subscribe("localizedpose", m_localizedPose, &NodeWithRobotModel::localizedPoseCallback);
	}

	virtual ~NodeWithRobotModel(void)
	{
	    if (m_urdf)
		delete m_urdf;
	    if (m_kmodel)
		delete m_kmodel;
	}
	
	void setRobotDescriptionFromData(const char *data)
	{
	    robot_desc::URDF *file = new robot_desc::URDF();
	    if (file->loadString(data))
		setRobotDescription(file);
	    else
		delete file;
	}
	
	void setRobotDescriptionFromFile(const char *filename)
	{
	    robot_desc::URDF *file = new robot_desc::URDF();
	    if (file->loadFile(filename))
		setRobotDescription(file);
	    else
		delete file;
	}
	
	virtual void setRobotDescription(robot_desc::URDF *file)
	{
	    if (m_urdf)
		delete m_urdf;
	    if (m_kmodel)
		delete m_kmodel;
	    
	    m_urdf = file;
	    m_kmodel = new planning_models::KinematicModel();
	    m_kmodel->setVerbose(false);
	    m_kmodel->build(*file);
	}
	
	virtual void loadRobotDescription(const std::string &robot_model)
	{
	    if (!robot_model.empty() && robot_model != "-")
	    {
		std::string content;
		if (get_param(robot_model, content))
		    setRobotDescriptionFromData(content.c_str());
		else
		    fprintf(stderr, "Robot model '%s' not found!\n", robot_model.c_str());
	    }
	}
	
	virtual void defaultPosition(void)
	{
	    if (m_kmodel)
	    {
		double defaultPose[m_kmodel->stateDimension];
		for (unsigned int i = 0 ; i < m_kmodel->stateDimension ; ++i)
		    defaultPose[i] = 0.0;
		
		m_kmodel->computeTransforms(defaultPose);
	    }
	}
	
	bool loadedRobot(void) const
	{
	    return m_kmodel != NULL;
	}
	
    protected:
	
	void localizedPoseCallback(void)
	{
	    bool success = true;
	    libTF::TFPose2D pose;
	    pose.x = m_localizedPose.pos.x;
	    pose.y = m_localizedPose.pos.y;
	    pose.yaw = m_localizedPose.pos.th;
	    pose.time = m_localizedPose.header.stamp.to_ull();
	    pose.frame = m_localizedPose.header.frame_id;
	    
	    try
	    {
		pose = m_tf.transformPose2D("FRAMEID_MAP", pose);
	    }
	    catch(libTF::TransformReference::LookupException& ex)
	    {
		fprintf(stderr, "Discarding pose: Transform reference lookup exception\n");
		success = false;
	    }
	    catch(libTF::TransformReference::ExtrapolateException& ex)
	    {
		fprintf(stderr, "Discarding pose: Extrapolation exception: %s\n", ex.what());
		success = false;
	    }
	    catch(...)
	    {
		fprintf(stderr, "Discarding pose: Exception in pose computation\n");
		success = false;
	    }
	    
	    if (success)
	    {
		m_basePos[0] = pose.x;
		m_basePos[1] = pose.y;
		m_basePos[2] = pose.yaw;
		
		baseUpdate();
	    }
	}
	
	virtual void baseUpdate(void)
	{
	}
	
	rosTFClient                      m_tf; 
	std_msgs::RobotBase2DOdom        m_localizedPose;
	robot_desc::URDF                *m_urdf;
	planning_models::KinematicModel *m_kmodel;	
	double                           m_basePos[3];

    };
     
}
