#pragma once
/***************************************************/
/*! \class CONTROLLER::HeadController
    \brief A PR2 Head controller
    
    This class implements controller loops for
    PR2 Head Control

*/
/***************************************************/
//#include <newmat10/newmat.h>
//#include <libKinematics/ik.h>
//#include <sys/types.h>
//#include <stdint.h>
//#include <libKDL/kdl_kinematics.h> // for kinematics using KDL -- util/kinematics/libKDL

#include <iostream>
#include <rosTF/rosTF.h>
#include <pr2Core/pr2Core.h>
#include <genericControllers/Controller.h>


namespace controller
{
  class HeadController : Controller
  {
    public:
    
      /*!
        * \brief Constructor,
        *
        * \param 
        */
      HeadController();
      
      /*!
        * \brief Destructor of Pid class.
        */       
      ~HeadController( );

      /*!
        * \brief Update controller
        */       
      void Update( );

      /*!
        * \brief Set yaw and pitch of head in Local Frames
        * 
        * Angles are defined by robot description file.<br>
        * Angle of 0 is the home position.
        *
        */       
      PR2::PR2_ERROR_CODE setPosition(double yaw , double pitch, double yawDot, double pitchDot);

      /*!
        * \brief Set yaw rate and pitch rate for the head in Local Frames
        * 
        */       
      PR2::PR2_ERROR_CODE setPositionRate(double yawDot, double pitchDot);

      /*!
        * \brief Set gaze point in global frame
        *
        * omit xDot, yDot, zDot to denote 0 velocity at target point.
        *
        * TODO: fix how to pass transforms or keep track of them in the RT loop, efficiently.
        *
        */
      PR2::PR2_ERROR_CODE setGazePoint(double x,double y, double z, double xDot, double yDot, double zDot);
      PR2::PR2_ERROR_CODE setGazePoint(double x,double y, double z);
      PR2::PR2_ERROR_CODE setGazePoint(double x,double y, double z, rosTFServer tf);

      /*!
        * \brief Set saccading speed
        *
        * Not sure how to saccade yet.
        *
        */       
      PR2::PR2_ERROR_CODE setSaccadeSpeed(double xDot, double yDot, double zDot);

      /*!
        * \brief Set parameters for this controller
        *
        * user can set maximum velocity
        * and maximum acceleration
        * constraints for this controller
        *
        * e.g.: <br>
        * <UL type="none">
        * <LI> setParam('maxVel', 1.0);
        * <LI> setParam('maxAcc', 1.0);
        * <LI> setParam('maxLim', 1.57);
        * <LI> setParam('minLim',-1.57);
        * </UL>
        *
        */
      PR2::PR2_ERROR_CODE setParam(std::string label,double value);
      PR2::PR2_ERROR_CODE setParam(std::string label,std::string value);

    private:
      PR2::PR2_CONTROL_MODE controlMode;      /**< Head controller control mode >*/
  };
}


