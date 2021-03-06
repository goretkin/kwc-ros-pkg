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
#include "libKDL/dynamics.h"

#include <sys/time.h>
#include <unistd.h>


using namespace KDL;
using namespace PR2;
using namespace std;


void planarTwoLink_solution(double l1,double l2,JntArray &q,JntArray &q_dot,JntArray &q_dotdot,double m1,double m2,double I1,double I2)
{
	double q1 = q(0), q2 = q(1);
	double q1_dot = q_dot(0), q2_dot = q_dot(1);
	double q1_dotdot = q_dotdot(0), q2_dotdot = q_dotdot(1);
	double t1,t2;
	double l_c2 = l2/2, l_c1 = l1/2;
	double g = 9.8;

	t2 = I2*(q1_dotdot+q2_dotdot) + m2*l1*l_c2*sin(q2)*q1_dot*q1_dot +
			 m2*l1*l_c2*cos(q2)*q1_dotdot + 
			 m2*l_c2*l_c2*(q1_dotdot+q2_dotdot)+m2*l_c2*g*cos(q1+q2);
	t1 = t2 + m1*l_c1*l_c1*q1_dotdot + m1*l_c1*g*cos(q1) + m2*l1*g*cos(q1) + 
			 I1*q1_dotdot + m2*l1*l1*q1_dotdot -
			 m2*l1*l_c2*(q1_dot+q2_dot)*(q1_dot+q2_dot)*sin(q2) +
			 m2*l1*l_c2*(q1_dotdot+q2_dotdot)*cos(q2);

	cout<<"Expected joint torques, Spong Pg 282"<<endl;
	cout<<"t1: "<<t1<<endl<<"t2: "<<t2<<endl;
}

void checkTwoLinkPlanar_2()
{
	double q1 = deg2rad*30, q2 = deg2rad*-48;
	double q1_dot = deg2rad*63, q2_dot = deg2rad*-52;
	double q1_dotdot = deg2rad*-20, q2_dotdot = deg2rad*73;
	double m1 = 1., m2 = 1.;
	double I1 = 1., I2 = 1.;
	double l1 = 1.0, l2 = 2.0;

	Inertia gen_mass[2];
	gen_mass[0].m = m1, gen_mass[1].m = m2;
	gen_mass[0].I.data[0] = I1, gen_mass[1].I.data[8] = I2;

	Vector r_c[2];
	r_c[0][0]=0 , r_c[0][1]=l1/2 , r_c[0][2]=0;
	r_c[1][0]=0 , r_c[1][1]=l2/2 , r_c[1][2]=0;

	Chain chain;
	chain.addSegment(Segment(Joint(Joint::RotX),Frame(Rotation::RotY(deg2rad*90),Vector(0.0,l1,0.0)), gen_mass[0], r_c[0]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::RotZ(deg2rad*90),Vector(0.0,l2,0.0)), gen_mass[1], r_c[1]));

	int n = chain.getNrOfJoints();
	JntArray q = JntArray(n);
	JntArray q_dot = JntArray(n);
	JntArray q_dotdot = JntArray(n);
//	Vector *f = new Vector[n+1];
	Vector *t = new Vector[n+1];

	q(0) = q1, q(1) = q2;
	q_dot(0) = q1_dot, q_dot(1) = q2_dot;
	q_dotdot(0) = q1_dotdot, q_dotdot(1) = q2_dotdot;

	//-------- now using the classes added into KDL --------
	ChainIdSolver_NE inv_dyn_solver = ChainIdSolver_NE(chain);
	inv_dyn_solver.InverseDynamics(q, q_dot, q_dotdot,t);
//	newtonEulerSolver(chain, q, q_dot, q_dotdot, f,t);
	printf("====================================\n");
	planarTwoLink_solution(l1, l2, q, q_dot, q_dotdot, m1, m2, I1, I2);
	cout<<"Calculated Torque 1 (Code):"<<t[0]<<endl;
	cout<<"Calculated Torque 2 (Code):"<<t[1]<<endl;
	printf("====================================\n");
}

void checkPUMA560_akb_fk(Chain &chain)
{
	//-------- check fwd kinematics using compare_KDL_pytb_kinematics.py --------
	JntArray cfg = JntArray(6);
	Frame f;
	ChainFkSolverPos_recursive fk_pos_solver = ChainFkSolverPos_recursive(chain);

	cfg(0)=deg2rad*0, cfg(1)=deg2rad*0, cfg(2)=deg2rad*0;
	cfg(3)=deg2rad*0, cfg(4)=deg2rad*0, cfg(5)=deg2rad*0;
	fk_pos_solver.JntToCart(cfg,f);
	cout<<"qz:\n"<<f<<endl;

	cfg(0)=deg2rad*0, cfg(1)=deg2rad*-90, cfg(2)=deg2rad*90;
	cfg(3)=deg2rad*0, cfg(4)=deg2rad*0, cfg(5)=deg2rad*0;
	fk_pos_solver.JntToCart(cfg,f);
	cout<<"qr:\n"<<f<<endl;

	cfg(0)=deg2rad*11, cfg(1)=deg2rad*54, cfg(2)=deg2rad*22;
	cfg(3)=deg2rad*20, cfg(4)=deg2rad*-7, cfg(5)=deg2rad*40;
	fk_pos_solver.JntToCart(cfg,f);
	cout<<"q=[math.radians(11), math.radians(54), math.radians(22), math.radians(20), math.radians(-7),  math.radians(40)]\n"<<f<<endl;
}

void checkPUMA560_akb()
{
	//------- define the PUMA560 kinematics to match pytb-18, puma560akb.py --------
	
	//----- dynamic parameters ------
	int n = 6;
	JntArray q = JntArray(n);
	JntArray q_dot = JntArray(n);
	JntArray q_dotdot = JntArray(n);
//	Vector *f = new Vector[n+1];
	Vector *t = new Vector[n+1];

	int li=0;
	Inertia gen_mass[6];
	gen_mass[li].m=0,   gen_mass[li].I.data[0]=0,     gen_mass[li].I.data[4]=0,     gen_mass[li].I.data[8]=0.35;  li++;
	gen_mass[li].m=17.4,gen_mass[li].I.data[0]=0.13,  gen_mass[li].I.data[4]=0.524, gen_mass[li].I.data[8]=0.539; li++;
	gen_mass[li].m=4.8, gen_mass[li].I.data[0]=0.066, gen_mass[li].I.data[4]=.0125, gen_mass[li].I.data[8]=0.066; li++;
	gen_mass[li].m=0.82,gen_mass[li].I.data[0]=1.8e-3,gen_mass[li].I.data[4]=1.8e-3,gen_mass[li].I.data[8]=1.3e-3;li++;
	gen_mass[li].m=0.34,gen_mass[li].I.data[0]=0.3e-3,gen_mass[li].I.data[4]=0.3e-3,gen_mass[li].I.data[8]=0.4e-3;li++;
	gen_mass[li].m=0.09,gen_mass[li].I.data[0]=.15e-3,gen_mass[li].I.data[4]=.15e-3,gen_mass[li].I.data[8]=0.04e-3;li++;

	Vector r_c[6];
	r_c[0][0]=0.0 , r_c[0][1]=0.0 , r_c[0][2]=0    ;
	r_c[1][0]=.068, r_c[1][1]=.006, r_c[1][2]=-.016;
	r_c[2][0]=0.0 , r_c[2][1]=-.07, r_c[2][2]=.014 ;
	r_c[3][0]=0.0 , r_c[3][1]=0.0 , r_c[3][2]=-.019;
	r_c[4][0]=0.0 , r_c[4][1]=0.0 , r_c[4][2]=0    ;
	r_c[5][0]=0.0 , r_c[5][1]=0.0 , r_c[5][2]=.032 ;

	//-------- define the Chain --------
	Chain chain;

	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::RotX(deg2rad*-90),Vector(0.0,0.2435,0.0)),gen_mass[0],r_c[0]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::Identity(),Vector(0.4318,0.0,-0.0934)),gen_mass[1],r_c[1]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::RotX(deg2rad*90),Vector(-0.0203,-0.4331,0.0)),gen_mass[2],r_c[2]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::RotX(deg2rad*-90),Vector(0.0,0.0,0.0)),gen_mass[3],r_c[3]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Rotation::RotX(deg2rad*90),Vector(0.0,0.0,0.0)),gen_mass[4],r_c[4]));
	chain.addSegment(Segment(Joint(Joint::RotZ),Frame(Vector(0.0,0.0,0.0)),gen_mass[5],r_c[5]));

	//----- check kinematics -----
//	checkPUMA560_akb_fk(chain);

	q(0)=deg2rad*0,q(1)=deg2rad*-30,q(2)=deg2rad*52;
	q(3)=deg2rad*43,q(4)=deg2rad*-28,q(5)=deg2rad*-53;

	q_dot(0)=0,q_dot(1)=-2.,q_dot(2)=-4.2;
	q_dot(3)=4.3,q_dot(4)=8.2,q_dot(5)=-2.2;

	q_dotdot(0)=0,q_dotdot(1)=5,q_dotdot(2)=6.4;
	q_dotdot(3)=-2.7,q_dotdot(4)=1.7,q_dotdot(5)=-3.;


//	newtonEulerSolver(chain, q, q_dot, q_dotdot, f,t);
//	printf("====================================\n");
//	cout<<"Calculated Torque 1 (Code):"<<t[0]<<endl;
//	cout<<"Calculated Torque 2 (Code):"<<t[1]<<endl;
//	cout<<"Calculated Torque 3 (Code):"<<t[2]<<endl;
//	cout<<"Calculated Torque 4 (Code):"<<t[3]<<endl;
//	cout<<"Calculated Torque 5 (Code):"<<t[4]<<endl;
//	cout<<"Calculated Torque 6 (Code):"<<t[5]<<endl;
//	printf("====================================\n");

	//-------- now using the classes added into KDL --------
	ChainIdSolver_NE inv_dyn_solver = ChainIdSolver_NE(chain);
	inv_dyn_solver.InverseDynamics(q, q_dot, q_dotdot,t);
	printf("====================================\n");
	cout<<"Calculated Torque 1 (KDL class):"<<t[0]<<endl;
	cout<<"Calculated Torque 2 (KDL class):"<<t[1]<<endl;
	cout<<"Calculated Torque 3 (KDL class):"<<t[2]<<endl;
	cout<<"Calculated Torque 4 (KDL class):"<<t[3]<<endl;
	cout<<"Calculated Torque 5 (KDL class):"<<t[4]<<endl;
	cout<<"Calculated Torque 6 (KDL class):"<<t[5]<<endl;
	printf("====================================\n");


}


int main( int argc, char** argv )
{
//	checkOneLink_static();
//	checkTwoLinkPlanar();
//	checkTwoLinkPlanar_2();
	checkPUMA560_akb();
}




/*

//  Conventions for joint and link numbering - Spong page 74.

Vector jointAxis(const Joint &jnt)
{
	Joint::JointType type = jnt.getType();
	switch(type)
	{
		case Joint::RotX:
			return Vector(1.,0.,0.);
		case Joint::RotY:
			return Vector(0.,1.,0.);
		case Joint::RotZ:
			return Vector(0.,0.,1.);
		default:
			printf("This joint type does not have an axis. Type:%d\n",type);
			printf("Exiting...\n");
			exit(0);
	}
}


//-- will later add parameters for forces/torques on the individual joints.
//f,t arrays of size n+1, last element forced to zero in this function.
int newtonEulerSolver(const Chain &chain, const JntArray &q, const JntArray &q_dot, 
											const JntArray &q_dotdot, Vector *f, Vector *t)
{

	//-- source: Robot Modeling and Control (Spong,Hutchinson,Vidyasagar). Page 279.
	int nJoints = q.rows();

	Vector *omega = new Vector[nJoints+1];
	Vector *alpha = new Vector[nJoints+1];
	Vector *a_c = new Vector[nJoints+1];

	omega[0] = Vector::Zero();
	alpha[0] = Vector::Zero();
	a_c[0] = Vector::Zero();

	Vector a_e = Vector::Zero();

	Frame _iT0 = Frame::Identity();
	Frame _iTi_minus_one, _i_minus_oneTi;
	Rotation _iRi_minus_one = Rotation::Identity();
	Frame T_tip = Frame::Identity();

	Vector z_i_minus_one, b_i, r, r_cm;
	Segment link_i;

	// logical joint numbering begins from 1 and index from 0.

	// computing angular velocities and accelerations for each link.
	for(int i=1;i<=nJoints;i++)
	{
		link_i = chain.getSegment(i-1);
		z_i_minus_one = jointAxis(link_i.getJoint());

		r_cm = link_i.getCM();
		_iRi_minus_one = T_tip.M.Inverse();

		b_i = z_i_minus_one;
		T_tip = link_i.getFrameToTip();
		r = T_tip.p; // I want vector in body coordinates.

		_iTi_minus_one = link_i.pose(q(i-1)).Inverse();
		_iT0 = _iTi_minus_one*_iT0;
		_iRi_minus_one = T_tip.M*_iTi_minus_one.M * _iRi_minus_one;

		omega[i] = _iRi_minus_one*omega[i-1] + b_i*q_dot(i-1);
		alpha[i] = _iRi_minus_one*alpha[i-1] + b_i*q_dotdot(i-1) + omega[i]*b_i*q_dot(i-1);
		a_c[i] = _iRi_minus_one*a_e + alpha[i]*r_cm + omega[i]*(omega[i]*r_cm);
		a_e = _iRi_minus_one*a_e + alpha[i]*r + omega[i]*(omega[i]*r);

//		printf("-----------------------------\ni: %d\n", i);
//		cout<<"omega[i]: "<<omega[i]<<endl<<"alpha[i]: "<<alpha[i]<<endl<<"a_c[i]: "<<a_c[i]<<endl;
	}

	// now for joint forces and torques.
	Vector g(0.,0.,-9.8);
	Rotation rot, rot_g;
	Frame T;

	Inertia gen_mass;
	InertiaMatrix I;
	double m;

	f[nJoints] = Vector::Zero();
	t[nJoints] = Vector::Zero();
	// _iT0 is now _nT0
	rot_g = _iT0.M;
	g = rot_g*g;

	Vector r_iplusone_c; // Vector from o_{i+1} to c_i
	rot = Rotation::Identity(); // should be rotation matrix for end-effector forces to coord frame of last link.

	for(int i=nJoints-1;i>=0;i--)
	{
		link_i = chain.getSegment(i);
		T_tip = link_i.getFrameToTip();
		rot = T_tip.M * rot;
		r_cm = link_i.getCM();

		r = T_tip.p; // I want vector in body coordinates.
		r_iplusone_c = r_cm-r;

		gen_mass = link_i.getInertia();
		I = gen_mass.I;
		m = gen_mass.m;

		T = link_i.pose(q(i));

		g = rot*g;

		f[i] = rot*f[i+1] + m*a_c[i+1] - m*g;
		t[i] = rot*t[i+1] - f[i]*r_cm + (rot*f[i+1])*r_iplusone_c+I*alpha[i+1]
					 + omega[i+1]*(I*omega[i+1]);

//		printf("-----------------------------\ni: %d\n", i);
//		cout<<"g: "<<g<<endl<<"f[i]: "<<f[i]<<endl;

		rot = T.M*T_tip.M.Inverse();
	}

	return 0;
}


void checkTwoLinkPlanar()
{
//-------- one link ---------
	double q1 = deg2rad*0, q2 = deg2rad*30;
	double q1_dot = deg2rad*0, q2_dot = deg2rad*0;
	double q1_dotdot = deg2rad*0, q2_dotdot = deg2rad*0;
	double m1 = 2., m2 = 5.;
	double I1 = 11., I2 = 8.;
	double l1 = 2.3, l2 = 1.6;

	Inertia gen_mass[2];
	gen_mass[0].m = m1, gen_mass[1].m = m2;
	gen_mass[0].I.data[0] = I1, gen_mass[1].I.data[0] = I2;

	Vector r_c[2];
	r_c[0][0]=0 , r_c[0][1]=l1/2 , r_c[0][2]=0;
	r_c[1][0]=0 , r_c[1][1]=l2/2 , r_c[1][2]=0;

	Chain chain;
	chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,l1,0.0)), gen_mass[0], r_c[0]));
	chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,l2,0.0)), gen_mass[1], r_c[1]));

	int n = chain.getNrOfJoints();
	JntArray q = JntArray(n);
	JntArray q_dot = JntArray(n);
	JntArray q_dotdot = JntArray(n);
	Vector *f = new Vector[n+1];
	Vector *t = new Vector[n+1];

	q(0) = q1, q(1) = q2;
	q_dot(0) = q1_dot, q_dot(1) = q2_dot;
	q_dotdot(0) = q1_dotdot, q_dotdot(1) = q2_dotdot;

	newtonEulerSolver(chain, q, q_dot, q_dotdot, f,t);
	printf("====================================\n");
	planarTwoLink_solution(l1, l2, q, q_dot, q_dotdot, m1, m2, I1, I2);
	cout<<"Calculated Force 1 (Code):"<<f[0]<<endl;
	cout<<"Calculated Force 2 (Code):"<<f[1]<<endl;
	cout<<"Calculated Torque 1 (Code):"<<t[0]<<endl;
	cout<<"Calculated Torque 2 (Code):"<<t[1]<<endl;
	printf("====================================\n");
}

void checkOneLink_static()
{
//-------- one link ---------
	double theta = deg2rad*30;
	double mass = 3.;
	double l1 = 13.;

	Inertia gen_mass[1];
	gen_mass[0].m = mass;
	gen_mass[0].I.data[0] = 10.;

	Vector r_c[1];
	r_c[0][0]=0 , r_c[0][1]=l1/2 , r_c[0][2]=0;

	Chain chain;
	chain.addSegment(Segment(Joint(Joint::RotX),Frame(Vector(0.0,l1,0.0)), gen_mass[0], r_c[0]));

	int n = chain.getNrOfJoints();
	JntArray q = JntArray(n);
	JntArray q_dot = JntArray(n);
	JntArray q_dotdot = JntArray(n);
	Vector *f = new Vector[n+1];
	Vector *t = new Vector[n+1];

	q(0) = theta;
	q_dot(0) = deg2rad*0.;
	q_dotdot(0) = deg2rad*0.;

	newtonEulerSolver(chain, q, q_dot, q_dotdot,f,t);
	printf("====================================\n");
	cout<<"Expected Force (Math):"<<Vector(0,mass*9.8*sin(theta),mass*9.8*cos(theta))<<endl;
	cout<<"Calculated Force (Code):"<<f[0]<<endl;
	cout<<"Expected Torque (Math):"<<Vector(mass*9.8*cos(theta)*l1/2,0,0)<<endl;
	cout<<"Calculated Torque (Code):"<<t[0]<<endl;
	printf("====================================\n");
}



*/
