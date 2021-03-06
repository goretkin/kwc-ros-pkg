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

// Author: Advait Jain


#include "libKDL/kdl_kinematics.h"

#include <sys/time.h>
#include <unistd.h>


using namespace KDL;
using namespace PR2;
using namespace std;



int main( int argc, char** argv )
{
	PR2_kinematics pr2_kin;
	struct timeval t0, t1;

	JntArray pr2_config = JntArray(pr2_kin.nJnts);
	pr2_config(0) = 0.0, pr2_config(1) = -0, pr2_config(2)=0.0, pr2_config(3)=0.0;
	pr2_config(4) = 0.0, pr2_config(5) = deg2rad*0, pr2_config(6) = deg2rad*0.0;
	cout<<"Config of the arm:"<<pr2_config<<endl;

//	pr2_config(0) = -0.641904;
//	pr2_config(1) = -0.496190;
//	pr2_config(2) = +0.201855;
//	pr2_config(3) = +0.845452;
//	pr2_config(4) = -0.187794;
//	pr2_config(5) = +1.23572;
//	pr2_config(6) = -0.482375;

	Frame f;
	if (pr2_kin.FK(pr2_config,f))
		cout<<"End effector transformation:"<<f<<endl;

//	pr2_config(0) = 0.1, pr2_config(1) = -1, pr2_config(2)=0.3, pr2_config(3)=0.3;
//	pr2_config(4) = 0.2, pr2_config(5) = 0.5, pr2_config(6) = 0.0;
//	cout<<"Config of the arm:"<<pr2_config<<endl;
//
//	if (pr2_kin.FK(pr2_config,f))
//		cout<<"End effector transformation:"<<f<<endl;
//	else
//		cout<<"Could not compute Fwd Kin."<<endl;
//
//	(*pr2_kin.q_IK_guess)(0) = 0.1, (*pr2_kin.q_IK_guess)(1) = 0.0, (*pr2_kin.q_IK_guess)(2) = 0.0, (*pr2_kin.q_IK_guess)(3) = 0.0;
//	(*pr2_kin.q_IK_guess)(4) = 0.0, (*pr2_kin.q_IK_guess)(5) = 0.0, (*pr2_kin.q_IK_guess)(6) = 0.0;
//
//	gettimeofday(&t0,NULL);
//	if (pr2_kin.IK(f) == true)
//	{
//		gettimeofday(&t1, NULL);
//		cout<<"IK result:"<<*pr2_kin.q_IK_result<<endl;
//		double time_taken = (t1.tv_sec*1000000+t1.tv_usec - (t0.tv_sec*1000000+t0.tv_usec))/1000.;
//	  printf("Time taken: %f ms\n", time_taken);
//	}
//	else
//		cout<<"Could not compute Inv Kin."<<endl;
//
//	//------ checking that IK returned a valid soln -----
//	Frame f_ik;
//	if (pr2_kin.FK(*pr2_kin.q_IK_result,f_ik))
//		cout<<"End effector after IK:"<<f_ik<<endl;
//	else
//		cout<<"Could not compute Fwd Kin. (After IK)"<<endl;

}



