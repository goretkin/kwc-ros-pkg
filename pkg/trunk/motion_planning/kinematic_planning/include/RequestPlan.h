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

/** \Author Ioan Sucan */

#ifndef KINEMATIC_PLANNING_REQUEST_PLAN_
#define KINEMATIC_PLANNING_REQUEST_PLAN_

#include "KinematicPlanningXMLModel.h"
#include "StateValidityChecker.h"

#include <robot_msgs/KinematicPath.h>
#include <robot_msgs/KinematicSpaceParameters.h>

#include <iostream>

class RequestPlan
{
 public:
    
    /** Validate common space parameters */
    bool areSpaceParamsValid(ModelMap &modelsRef, robot_msgs::KinematicSpaceParameters &params) const
    { 
	ModelMap::const_iterator pos = modelsRef.find(params.model_id);
	
	if (pos == modelsRef.end())
	{
	    std::cerr << "Cannot plan for '" << params.model_id << "'. Model does not exist" << std::endl;
	    return false;	    
	}

	/* find the model */
	XMLModel *m = pos->second;
	
	/* if the user did not specify a planner, use the first available one */
	if (params.planner_id.empty())
	{
	    if (!m->planners.empty())
		params.planner_id = m->planners.begin()->first;
	}
	
	/* check if desired planner exists */
	std::map<std::string, KinematicPlannerSetup>::iterator plannerIt = m->planners.find(params.planner_id);
	if (plannerIt == m->planners.end())
	{
	    std::cerr << "Motion planner not found: '" << params.planner_id << "'" << std::endl;
	    return false;
	}
	
	KinematicPlannerSetup &psetup = plannerIt->second;

	/* check if the desired distance metric is defined */
	if (psetup.sde.find(params.distance_metric) == psetup.sde.end())
	{
	    std::cerr << "Distance evaluator not found: '" << params.distance_metric << "'" << std::endl;
	    return false;
	}
	return true;
    }
    
    /** Configure the state space for a given set of parameters and set the starting state */
    void setupStateSpaceAndStartState(XMLModel *m, KinematicPlannerSetup &psetup,
				      robot_msgs::KinematicSpaceParameters &params,
				      robot_msgs::KinematicState &start_state)
    {
	/* set the workspace volume for planning */
	// only area or volume should go through... not sure what happens on really complicated models where 
	// we have both multiple planar and multiple floating joints...
	static_cast<SpaceInformationXMLModel*>(psetup.si)->setPlanningArea(params.volumeMin.x, params.volumeMin.y,
									   params.volumeMax.x, params.volumeMax.y);
        static_cast<SpaceInformationXMLModel*>(psetup.si)->setPlanningVolume(params.volumeMin.x, params.volumeMin.y, params.volumeMin.z,
									     params.volumeMax.x, params.volumeMax.y, params.volumeMax.z);

	psetup.si->setStateDistanceEvaluator(psetup.sde[params.distance_metric]);
	
	/* set the starting state */
	const unsigned int dim = psetup.si->getStateDimension();
	ompl::SpaceInformationKinematic::StateKinematic_t start = new ompl::SpaceInformationKinematic::StateKinematic(dim);
	
	if (m->groupID >= 0)
	{
	    /* set the pose of the whole robot */
	    m->kmodel->computeTransforms(start_state.vals);
	    m->collisionSpace->updateRobotModel(m->collisionSpaceID);
	    
	    /* extract the components needed for the start state of the desired group */
	    for (unsigned int i = 0 ; i < dim ; ++i)
		start->values[i] = start_state.vals[m->kmodel->groupStateIndexList[m->groupID][i]];
	}
	else
	{
	    /* the start state is the complete state */
	    for (unsigned int i = 0 ; i < dim ; ++i)
		start->values[i] = start_state.vals[i];
	}
	
	psetup.si->addStartState(start);
    }
    
    /** Set the kinematic constraints to follow */
    void setupPoseConstraints(KinematicPlannerSetup &psetup, const std::vector<robot_msgs::PoseConstraint> &cstrs)
    {
	static_cast<StateValidityPredicate*>(psetup.si->getStateValidityChecker())->setPoseConstraints(cstrs);
    }    

    /** Compute the actual motion plan */
    void computePlan(KinematicPlannerSetup &psetup, int times, double allowed_time, bool interpolate,
		     ompl::SpaceInformationKinematic::PathKinematic_t &bestPath, double &bestDifference)
    {
		
	if (times <= 0)
	{
	    std::cerr << "Request specifies motion plan cannot be computed " << times << " times" << std::endl;
	    return;
	}

	/* do the planning */
	bestPath = NULL;
	bestDifference = 0.0;
        double totalTime = 0.0;
	ompl::SpaceInformation::Goal_t goal = psetup.si->getGoal();
	
	for (int i = 0 ; i < times ; ++i)
	{
	    ros::Time startTime = ros::Time::now();
	    bool ok = psetup.mp->solve(allowed_time); 
	    double tsolve = (ros::Time::now() - startTime).to_double();	
	    std::cout << (ok ? "[Success]" : "[Failure]") << " Motion planner spent " << tsolve << " seconds" << std::endl;
	    totalTime += tsolve;
	    
	    /* do path smoothing */
	    if (ok)
	    {
		ros::Time startTime = ros::Time::now();
		ompl::SpaceInformationKinematic::PathKinematic_t path = static_cast<ompl::SpaceInformationKinematic::PathKinematic_t>(goal->getSolutionPath());
		psetup.smoother->smoothVertices(path);
		double tsmooth = (ros::Time::now() - startTime).to_double();
		std::cout << "          Smoother spent " << tsmooth << " seconds (" << (tsmooth + tsolve) << " seconds in total)" << std::endl;
		if (interpolate)
		    psetup.si->interpolatePath(path);
		if (bestPath == NULL || bestDifference > goal->getDifference() || 
		    (bestPath && bestDifference == goal->getDifference() && bestPath->states.size() > path->states.size()))
		{
		    if (bestPath)
			delete bestPath;
		    bestPath = path;
		    bestDifference = goal->getDifference();
		    goal->forgetSolutionPath();
		    std::cout << "          Obtained better solution" << std::endl;
		}
	    }
	    psetup.mp->clear();	    
	}
	
	
        std::cout << std::endl << "Total planning time: " << totalTime << "; Average planning time: " << (totalTime / (double)times) << " (seconds)" << std::endl;
    }

    void fillSolution(KinematicPlannerSetup &psetup, ompl::SpaceInformationKinematic::PathKinematic_t bestPath, double bestDifference,
		      robot_msgs::KinematicPath &path, double &distance)
    {
	const unsigned int dim = psetup.si->getStateDimension();
	
	/* copy the solution to the result */
	if (bestPath)
	{
	    path.set_states_size(bestPath->states.size());
	    for (unsigned int i = 0 ; i < bestPath->states.size() ; ++i)
	    {
		path.states[i].set_vals_size(dim);
		for (unsigned int j = 0 ; j < dim ; ++j)
		    path.states[i].vals[j] = bestPath->states[i]->values[j];
	    }
	    distance = bestDifference;
	    delete bestPath;
	}
	else
	{
	    path.set_states_size(0);
	    distance = -1.0;
	}
    }
    
    void cleanupPlanningData(KinematicPlannerSetup &psetup)
    {
	/* cleanup */
	psetup.si->clearGoal();
	psetup.si->clearStartStates();	
    }
    
};



#endif
