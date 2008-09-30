#include "iostream.h"

#include "opencv/cv.h"

#include "CvMat3X3.h"
#include "LevMarqTransformDispSpace.h"
#include "PoseEstimate.h"
#include "CvMatUtils.h"
#include "CvTestTimer.h"

using namespace cv::willow;

#undef DEBUG
#define USE_LEVMARQ

#if 0
#define TIMERSTART(x)
#define TIMEREND(x)
#define TIMERSTART2(x)
#define TIMEREND2(x)
#else
#define TIMERSTART(x) CvTestTimerStart(x)
#define TIMEREND(x) CvTestTimerEnd(x)
#define TIMERSTART2(x) CvTestTimerStart2(x)
#define TIMEREND2(x) CvTestTimerEnd2(x)
#endif

PoseEstimate::PoseEstimate():
	mNumRansacIter(50), mMinDet(0.1), mMinAngleForRansacTriple(10.),
	mNumTriesForRandomTriple(100),
	mErrMapping(NULL), mErrNormType(CV_C),	mErrThreshold(mDefErrThreshold),
	mResidue1(cvMat(3, 1, CV_64F, mResidue1_Data)),
	mResidue2(cvMat(3, 1, CV_64F, mResidue2_Data)),
	mW1(cvMat(4, 1, CV_64F, mW1_Data)),
	mT(cvMat(4, 4, CV_64F, mT_Data)),
#if 0
	mRng(SEED),
#endif
	mRandomTripletSetGenerator(0, 100),
	mInliers0(NULL),
	mInliers1(NULL),
	mInlierIndices(NULL)
{
	mRTBestWithoutLevMarq = cvMat(4, 4, CV_64F, mRTBestWithoutLevMarqData);
}

PoseEstimate::~PoseEstimate()
{
}

void PoseEstimate::configureErrorMeasurement(CvMat *mapping, double threshold, int normType) {
	this->mErrMapping   = mapping;
	this->mErrThreshold = threshold;
	this->mErrNormType  = normType;
}

bool PoseEstimate::estimateLeastSquare(CvMat *p0, CvMat *p1, CvMat *R, CvMat *T) {
	CvMat *P0 = cvCreateMat(p0->cols, p0->rows, CV_64FC1);
	CvMat *P1 = cvCreateMat(p1->cols, p1->rows, CV_64FC1);
	cvTranspose(p0, P0);
	cvTranspose(p1, P1);

	bool status =  estimateLeastSquareInCol(P0, P1, R, T);
	cvReleaseMat(&P0);
	cvReleaseMat(&P1);
	return status;
}

bool PoseEstimate::estimateLeastSquareInCol(CvMat *P0, CvMat *P1, CvMat *R, CvMat *T) {
	bool status = true;
	double _Q[9], _W[9], _Ut[9], _Vt[9], _C0[3], _C1[3];
	CvMat Q  = cvMat(3, 3, CV_64F, _Q); // Q = P1 * transpose(P0)
	CvMat W  = cvMat(3, 3, CV_64F, _W); // Eigen matrix of Q. Q = U * W * transpose(V)
	CvMat Ut = cvMat(3, 3, CV_64F, _Ut); // transpose(U)
	CvMat Vt = cvMat(3, 3, CV_64F, _Vt); // transpose(V)
	CvMat C0 = cvMat(3, 1, CV_64F, _C0); // centroid of the 3 points in P0
	CvMat C1 = cvMat(3, 1, CV_64F, _C1); // centroid of the 3 points in P1
	// compute the centroids of these 3 ponints for the two positions
	//cvCVAPI(void)  cvReduce( const CvArr* src, CvArr* dst, int dim CV_DEFAULT(-1),
    //               int op CV_DEFAULT(CV_REDUCE_SUM) );
	cvReduce(P0, &C0, -1, CV_REDUCE_AVG);
	// compute the relative vectors of the two groups of points w.r.t. their centroids.
	double _Temp[3*P0->cols];
	CvMat Temp = cvMat(3, P0->cols, CV_64F, _Temp);
	cvRepeat(&C0, &Temp);
	cvSub(P0, &Temp, P0);

	cvReduce(P1, &C1, -1, CV_REDUCE_AVG);
	cvRepeat(&C1, &Temp);
	cvSub(P1, &Temp, P1);

	// Q = P1 * P0^T
	cvGEMM(P1, P0, 1, NULL, 0, &Q, CV_GEMM_B_T);

	// do a SVD on Q. Q = U*W*transpose(V)
	// according to the documentation, specifying the flags speeds up the processing
	cvSVD(&Q, &W, &Ut, &Vt, CV_SVD_MODIFY_A|CV_SVD_U_T|CV_SVD_V_T);

	// R = U * S * V^T
	double _S[] = {
			1.,0.,0.,
			0.,1.,0.,
			0.,0.,1.
	};
	CvMat S;
	cvInitMatHeader(&S, 3, 3, CV_64FC1, _S);
	if (cvDet(&Ut)*cvDet(&Vt)<0) {
		// Ut*V is not a rotation matrix, although either of them are unitary
		// we shall set the last diag entry of S to be -1
		// so that Ut*S*V is a rotational matrix
		// setting S[2,2] = -1
		_S[8] = -1.;
		double _UxS[9];
		CvMat UxS;
		cvInitMatHeader(&UxS, 3, 3, CV_64FC1, _UxS);
		cvGEMM(&Ut, &S, 1, NULL, 0, &UxS, CV_GEMM_A_T);
		cvGEMM(&UxS, &Vt, 1, NULL, 0, R, 0);
	} else {
		cvGEMM(&Ut, &Vt, 1, NULL, 0, R, CV_GEMM_A_T);
	}

	// t = centroid1 - R*centroid0
	cvGEMM(R, &C0, -1, &C1, 1, T, 0);
	return status;
}

int PoseEstimate::estimate(CvMat *points0, CvMat *points1, CvMat *rot, CvMat *trans, bool smoothed){
  int numInLiers = 0;

  int numPoints = points0->rows;

  if (numPoints != points1->rows) {
    cerr << "number of points mismatched in input" << endl;
    return 0;
  }

  double _P0[3*3], _P1[3*3], _R[3*3], _T[3*1];
  double _RT[16];
  CvMat P0, P1;
  cvInitMatHeader(&P0, 3, 3, CV_64FC1, _P0); // 3 random points from camera0, stored in columns
  cvInitMatHeader(&P1, 3, 3, CV_64FC1, _P1); // 3 random points from caemra1, stored in columns

  CvMat R, T, RT;
  cvInitMatHeader(&R,  3, 3, CV_64FC1, _R);  // rotation matrix, R = V * tranpose(U)
  cvInitMatHeader(&T,  3, 1, CV_64FC1, _T);  // translation matrix
  cvInitMatHeader(&RT, 4, 4, CV_64FC1, _RT); // transformation matrix (including rotation and translation)

  int maxNumInLiers=0;
  mRandomTripletSetGenerator.reset(0, numPoints-1);
  for (int i=0; i< mNumRansacIter; i++) {
#ifdef DEBUG
    cout << "Iteration: "<< i << endl;
#endif
    // randomly pick 3 points. make sure they are not
    // tooCloseToColinear
    if (pick3RandomPoints(points0, points1, &P0, &P1)==false){
      break;
    }

    TIMERSTART2(SVD);
    this->estimateLeastSquareInCol(&P0, &P1, &R, &T);
    TIMEREND2(SVD);

    this->constructRT(&R, &T, &RT);

    CvTestTimerStart(CheckInliers);
    // scoring against all points
    numInLiers = checkInLiers(points0, points1, &RT);
    CvTestTimerEnd(CheckInliers);
#ifdef DEBUG
    cout << "R, T, and RT: "<<endl;
    CvMatUtils::printMat(&R);
    CvMatUtils::printMat(&T);
    CvMatUtils::printMat(&RT);
#endif

    // keep the best R and T
    if (maxNumInLiers < numInLiers) {
      maxNumInLiers = numInLiers;
      cvCopy(&R, rot);
      cvCopy(&T, trans);
    }
  }

  if (maxNumInLiers<6) {
    cout << "Too few inliers: "<< maxNumInLiers << endl;
    return maxNumInLiers;
  }

  int numInLiers0;
  CvMat *points0Inlier;
  CvMat *points1Inlier;
  int   *inlierIndices;
  TIMERSTART(CopyInliers);
  // get a copy of all the inliers, original and transformed
  points0Inlier = cvCreateMat(maxNumInLiers, 3, CV_64FC1);
  points1Inlier = cvCreateMat(maxNumInLiers, 3, CV_64FC1);
  inlierIndices = new int[maxNumInLiers];

  // construct homography matrix
  constructRT(rot, trans, &RT);

  numInLiers0 = getInLiers(points0, points1, &RT, maxNumInLiers, points0Inlier, points1Inlier, inlierIndices);

  TIMEREND(CopyInliers);

  // make a copy of the best RT before nonlinear optimization
  cvCopy(&RT, &mRTBestWithoutLevMarq);

  if (smoothed == true) {
    TIMERSTART(LevMarq);
    // get the euler angle from rot
    CvPoint3D64f eulerAngles;
    {
      double _R[9], _Q[9];
      CvMat R, Q;
      CvMat *pQx=NULL, *pQy=NULL, *pQz=NULL;  // optional. For debugging.
      cvInitMatHeader(&R,  3, 3, CV_64FC1, _R);
      cvInitMatHeader(&Q,  3, 3, CV_64FC1, _Q);

      cvRQDecomp3x3((const CvMat*)rot, &R, &Q, pQx, pQy, pQz, &eulerAngles);

    }

    // nonlinear optimization by Levenberg-Marquardt
    cv::willow::LevMarqTransform levMarq(numInLiers0);

    double param[6];

    //initialize the parameters
    param[0] = eulerAngles.x/180. * CV_PI;
    param[1] = eulerAngles.y/180. * CV_PI;
    param[2] = eulerAngles.z/180. * CV_PI;
    param[3] = cvmGet(trans, 0, 0);
    param[4] = cvmGet(trans, 1, 0);
    param[5] = cvmGet(trans, 2, 0);

#ifdef DEBUG
    //#if 1
    printf("initial parameters: %f(%f), %f(%f), %f(%f), %f, %f, %f\n",
        param[0], param[0]/CV_PI*180., param[1], param[1]/CV_PI*180.,
        param[2], param[2]/CV_PI*180.,
        param[3], param[4], param[5]);
#endif
#ifdef DEBUG_DISTURB_PARAM
    for (int i=0; i<6; i++) param[i] *= 1.1;
#endif
#ifdef DEBUG
    //#if 1
    printf("disturbed parameters: %f(%f), %f(%f), %f(%f), %f, %f, %f\n",
        param[0], param[0]/CV_PI*180., param[1], param[1]/CV_PI*180.,
        param[2], param[2]/CV_PI*180.,
        param[3], param[4], param[5]);
#endif
    int64 tDoit = cvGetTickCount();
    levMarq.optimize(points0Inlier, points1Inlier, param);
    CvTestTimer::getTimer().mLevMarqDoit += cvGetTickCount() - tDoit;

#ifdef DEBUG
    printf("optimized parameters: %f(%f), %f(%f), %f(%f), %f, %f, %f\n",
        param[0], param[0]/CV_PI*180., param[1], param[1]/CV_PI*180.,
        param[2], param[2]/CV_PI*180.,
        param[3], param[4], param[5]);
#endif

    // TODO: construct matrix with parameters from nonlinear optimization
    double _rot[9];

    CvMat3X3<double>::rotMatrix(param[0], param[1], param[2], _rot, CvMat3X3<double>::EulerXYZ);
    for (int i=0;i<3;i++)
      for (int j=0;j<3;j++) {
        cvmSet(rot, i, j, _rot[i*3+j]);
      }
    cvmSet(trans, 0, 0, param[3]);
    cvmSet(trans, 1, 0, param[4]);
    cvmSet(trans, 2, 0, param[5]);

    TIMEREND(LevMarq);
  }
  updateInlierInfo(points0Inlier, points1Inlier, inlierIndices);

  this->constructRT(rot, trans, &mT);
  return maxNumInLiers;
}

void PoseEstimate::updateInlierInfo(CvMat* points0Inlier, CvMat* points1Inlier, int* inlierIndices) {
  if (mInliers0) cvReleaseMat(&mInliers0);
  if (mInliers1) cvReleaseMat(&mInliers1);
  mInliers0 = points0Inlier;
  mInliers1 = points1Inlier;
  delete [] mInlierIndices;
  mInlierIndices = inlierIndices;
}

#if 0
bool PoseEstimate::tooCloseToColinear(CvMat* points){
	CvMat * temp = cvCloneMat(points);
	cvSetReal2D(temp, 0, 0, 1);
	cvSetReal2D(temp, 0, 1, 1);
	cvSetReal2D(temp, 0, 2, 1);
	if (cvDet(temp)>this->mMinDet){
		return false;
	}
	cvCopy(points, temp);
	cvSetReal2D(temp, 1, 0, 1);
	cvSetReal2D(temp, 1, 1, 1);
	cvSetReal2D(temp, 1, 2, 1);
	if (cvDet(temp)>this->mMinDet){
		return false;
	}
	cvCopy(points, temp);
	cvSetReal2D(temp, 2, 0, 1);
	cvSetReal2D(temp, 2, 1, 1);
	cvSetReal2D(temp, 2, 2, 1);
	if (cvDet(temp)>this->mMinDet){
		return false;
	}
	// TODO: release temp
	cvReleaseMat(&temp);
	return true;
}
#else
bool PoseEstimate::tooCloseToColinear(CvMat *points)  {
	CvMat p0, p1, p2;
	double _p01[3], _p02[3];
	CvMat p01, p02;
	cvGetCol(points, &p0, 0);
	cvGetCol(points, &p1, 1);
	cvGetCol(points, &p2, 2);
	cvInitMatHeader(&p01, 3, 1, CV_64FC1, _p01);
	cvInitMatHeader(&p02, 3, 1, CV_64FC1, _p02);
	cvSub(&p1, &p0, &p01);
	cvSub(&p2, &p0, &p02);
	double norm_p01 = cvNorm(&p01);
	double norm_p02 = cvNorm(&p02);
	double cosAngle = cvDotProduct(&p01, &p02)/norm_p01/norm_p02;
	if (cosAngle > cos(CV_PI/180.0*15.0)) {
#ifdef DEBUG
		cout << "Too close to colinear: angle = "<< acos(cosAngle)/CV_PI*180.0<<"(degree)"<<endl;
#endif
		return true;
	} else {
		return false;
	}
}
#endif

bool PoseEstimate::pick3RandomPoints(CvMat* points0, CvMat* points1, CvMat* P0, CvMat* P1,
		bool fInputPointsInRows){
	bool status = false;

	for (int i=0; i<mNumTriesForRandomTriple; i++){

	  CvTripletSet triplet;
	  if (mRandomTripletSetGenerator.nextSet(triplet)==false) {
	    // no more random triplet set available
	    status = false;
	    break;
	  }
	  int pa = triplet[0];
	  int pb = triplet[1];
	  int pc = triplet[2];

		// points in P0, and P1 are stored in columns
		if (fInputPointsInRows == true ) {
			cvSetReal2D(P0, 0, 0, cvmGet(points0, pa, 0));
			cvSetReal2D(P0, 1, 0, cvmGet(points0, pa, 1));
			cvSetReal2D(P0, 2, 0, cvmGet(points0, pa, 2));

			cvSetReal2D(P1, 0, 0, cvmGet(points1, pa, 0));
			cvSetReal2D(P1, 1, 0, cvmGet(points1, pa, 1));
			cvSetReal2D(P1, 2, 0, cvmGet(points1, pa, 2));

			cvSetReal2D(P0, 0, 1, cvmGet(points0, pb, 0));
			cvSetReal2D(P0, 1, 1, cvmGet(points0, pb, 1));
			cvSetReal2D(P0, 2, 1, cvmGet(points0, pb, 2));

			cvSetReal2D(P1, 0, 1, cvmGet(points1, pb, 0));
			cvSetReal2D(P1, 1, 1, cvmGet(points1, pb, 1));
			cvSetReal2D(P1, 2, 1, cvmGet(points1, pb, 2));

			cvSetReal2D(P0, 0, 2, cvmGet(points0, pc, 0));
			cvSetReal2D(P0, 1, 2, cvmGet(points0, pc, 1));
			cvSetReal2D(P0, 2, 2, cvmGet(points0, pc, 2));

			cvSetReal2D(P1, 0, 2, cvmGet(points1, pc, 0));
			cvSetReal2D(P1, 1, 2, cvmGet(points1, pc, 1));
			cvSetReal2D(P1, 2, 2, cvmGet(points1, pc, 2));
		} else {
			cerr << "Not Implemented Yet"<<endl;
			status = false;
			return status;
		}

#ifdef DEBUG
		printf("random points: %d, %d, %d\n", pa, pb, pc);
		cout << "P0:"<<endl;
		CvMatUtils::printMat(P0);
		cout << "P1:"<<endl;
		CvMatUtils::printMat(P1);
#endif

		// TODO: check if they are tooCloseToColinear
		if (tooCloseToColinear(P0) == false && tooCloseToColinear(P1) == false) {
			status = true;
			break;
		}
	}
	return status;
}

bool PoseEstimate::constructRT(CvMat *R, CvMat *T, CvMat *RT){
	if (R == NULL || T == NULL) {
		return false;
	}
	return constructTransform(*R, *T, *RT);
}

bool PoseEstimate::constructTransform(
		const CvMat& rot, const CvMat& shift, CvMat& transform) {
	bool status = true;
    // construct RT
    for (int r=0; r<3; r++) {
        for (int c=0; c<3; c++) {
            cvSetReal2D(&transform, r, c, cvmGet(&rot, r, c));
        }
        cvSetReal2D(&transform, r, 3, cvmGet(&shift, r, 0));
    }

    // last row
    cvSetReal2D(&transform, 3, 0, 0.0);
    cvSetReal2D(&transform, 3, 1, 0.0);
    cvSetReal2D(&transform, 3, 2, 0.0);
    cvSetReal2D(&transform, 3, 3, 1.0);
	return status;
}

int PoseEstimate::checkInLiers(CvMat *points0, CvMat *points1, CvMat* transformation){
  // check if we can use a faster implementation
  assert(points0 && points1 && transformation);

  if (CV_MAT_TYPE(points0->type)        == CV_64FC1 &&
      CV_MAT_TYPE(points1->type)        == CV_64FC1 &&
      CV_MAT_TYPE(transformation->type) == CV_64FC1 &&
      points0->step == 3*sizeof(double) &&
      points1->step == 3*sizeof(double) &&
      transformation->step == 4*sizeof(double) &&
      transformation->rows == 4) {
    return checkInliers(points0->rows, points0->data.db, points1->data.db,
        transformation->data.db, mErrThreshold);
  } else {
#if DEBUG
    cout << __PRETTY_FUNCTION__ << "falls back to the slower version"<<endl;
#endif
    return getInLiers(points0, points1, transformation, 0, NULL, NULL, NULL);
  }
}

bool PoseEstimate::isInLier(CvMat *points0, CvMat *points1, int i){
	{
		double p0x, p0y, p0z;

		p0x = cvmGet(points0, i, 0);
		p0y = cvmGet(points0, i, 1);
		p0z = cvmGet(points0, i, 2);

		double w0 = mT_Data[ 0]*p0x + mT_Data[ 1]*p0y + mT_Data[ 2]*p0z + mT_Data[ 3];
		double w1 = mT_Data[ 4]*p0x + mT_Data[ 5]*p0y + mT_Data[ 6]*p0z + mT_Data[ 7];
		double w2 = mT_Data[ 8]*p0x + mT_Data[ 9]*p0y + mT_Data[10]*p0z + mT_Data[11];
		double w3 = mT_Data[12]*p0x + mT_Data[13]*p0y + mT_Data[14]*p0z + mT_Data[15];

		double scale;

		mResidue1_Data[0] = cvmGet(points1, i, 0)-w0*(scale=1./w3);
		mResidue1_Data[1] = cvmGet(points1, i, 1)-w1*scale;
		mResidue1_Data[2] = cvmGet(points1, i, 2)-w2*scale;
	}

	// computing the norm
	double error;
	if (mErrMapping == NULL){
		error = cvNorm(&mResidue1, NULL, this->mErrNormType);
	} else {
		cout << "Not Implemented Yet"<<endl;
		cvMatMul(mErrMapping, &mResidue1, &mResidue2);
		error = cvNorm(&mResidue2, NULL, this->mErrNormType);
	}

	return error <= this->mErrThreshold;
}

int PoseEstimate::getInLiers(CvMat *points0, CvMat *points1, CvMat* transformation,
    int maxNumInliersReturned, CvMat* points0Inlier, CvMat* points1Inlier, int inlierIndices[]) {

  // check if we can use a faster but more specific implementation
  if (CV_MAT_TYPE(points0->type)        == CV_64FC1 &&
      CV_MAT_TYPE(points1->type)        == CV_64FC1 &&
      CV_MAT_TYPE(transformation->type) == CV_64FC1 &&
      points0->step == 3*sizeof(double) &&
      points1->step == 3*sizeof(double) &&
      transformation->step == 4*sizeof(double) &&
      transformation->rows == 4 && (
          (maxNumInliersReturned == 0) ||
          (points0Inlier && CV_MAT_TYPE(points0Inlier->type) == CV_64FC1 && (points0Inlier->step == 3*sizeof(double))) ||
          (points1Inlier && CV_MAT_TYPE(points1Inlier->type) == CV_64FC1 && (points1Inlier->step == 3*sizeof(double))) ||
          (inlierIndices != NULL)
      )
  ) {
    double* inliers0 = points0Inlier? points0Inlier->data.db : NULL;
    double* inliers1 = points1Inlier? points1Inlier->data.db : NULL;

    return getInliers(points0->rows, points0->data.db, points1->data.db,
        transformation->data.db, mErrThreshold,
        maxNumInliersReturned, inliers0, inliers1, inlierIndices);
  } else {
    // falls back to the slower but more general version
#if DEBUG
    cout << __PRETTY_FUNCTION__ << "falls back to the slower version"<<endl;
#endif
    int numInLiers = 0;
    int numPoints = points0->rows;

    for (int i=0; i<numPoints; i++) {

      if (isInLier(points0, points1, i) == true) {
        // store the inlier
        if (points0Inlier) {
          cvmSet(points0Inlier, numInLiers, 0, cvmGet(points0, i, 0));
          cvmSet(points0Inlier, numInLiers, 1, cvmGet(points0, i, 1));
          cvmSet(points0Inlier, numInLiers, 2, cvmGet(points0, i, 2));
        }
        if (points1Inlier) {
          cvmSet(points1Inlier, numInLiers, 0, cvmGet(points1, i, 0));
          cvmSet(points1Inlier, numInLiers, 1, cvmGet(points1, i, 1));
          cvmSet(points1Inlier, numInLiers, 2, cvmGet(points1, i, 2));
        }
        if (inlierIndices) {
          inlierIndices[numInLiers] = i;
        }
        numInLiers++;
      }
    }
    return numInLiers;
  }
}

int PoseEstimate::checkInliers(
    int numPoints,
    double *_P0,
    double *_P1,
    double *_T,
    double threshold
) {
  int numInLiers = 0;

  double thresholdM =  threshold;
  double thresholdm = -threshold;
  for (int i=0; i<numPoints; i++) {

    double p0x, p0y, p0z;

#if 1
    p0x = *_P0++;
    p0y = *_P0++;
    p0z = *_P0++;

    double w3 = _T[15] + _T[14]*p0z + _T[13]*p0y + _T[12]*p0x;
#else
    // not worth using the following code
    // 1) not sure if it helps on speed.
    // 2) not sure if the order of evaluation is preserved as left to right.
    double w3 =
      _T[12]*(p0x=*_P0++) +
      _T[13]*(p0y=*_P0++) +
      _T[14]*(p0z=*_P0++) +
      _T[15];
#endif

    double scale = 1.0/w3;

    double rx = *_P1++ - (_T[3] + _T[2]*p0z + _T[1]*p0y + _T[0]*p0x)*scale;
    if (rx> thresholdM || rx< thresholdm) {
      (++_P1)++;
      continue;
    }
    double ry = *_P1++ - (_T[7] + _T[6]*p0z + _T[5]*p0y + _T[4]*p0x)*scale;
    if (ry>thresholdM || ry< thresholdm) {
      _P1++;
      continue;
    }

    double rz = *_P1++ - (_T[11] + _T[10]*p0z + _T[9]*p0y + _T[8]*p0x)*scale;
    if (rz>thresholdM || rz< thresholdm) {
      continue;
    }

    numInLiers++;
  }
#ifdef DEBUG
  cout << "Num of Inliers: "<<numInLiers<<endl;
#endif
  return numInLiers;
}

int PoseEstimate::getInliers(
    int numPoints,
    double *_P0,
    double *_P1,
    double *_T,
    double threshold,
    int maxNumInlierReturned,
    double inliers0[],
    double inliers1[],
    int    inlierIndices[]
) {
  int numInLiers = 0;

  double thresholdM =  threshold;
  double thresholdm = -threshold;

  for (int i=0; i<numPoints; i++) {

    double p0x, p0y, p0z;

    p0x = *_P0++;
    p0y = *_P0++;
    p0z = *_P0++;

    double w3 = _T[15] + _T[14]*p0z + _T[13]*p0y + _T[12]*p0x;
    double scale = 1.0/w3;

    double rx = *_P1++ - (_T[3] + _T[2]*p0z + _T[1]*p0y + _T[0]*p0x)*scale;
    // Experiments have shown that checking rx, ry, rz separately and rejecting the current
    // point as early as possible helps speeding up the computation
    if (rx> thresholdM || rx< thresholdm) {
      (++_P1)++;
      continue;
    }
    double ry = *_P1++ - (_T[7] + _T[6]*p0z + _T[5]*p0y + _T[4]*p0x)*scale;
    if (ry>thresholdM || ry< thresholdm) {
      _P1++;
      continue;
    }

    double rz = *_P1++ - (_T[11] + _T[10]*p0z + _T[9]*p0y + _T[8]*p0x)*scale;
    if (rz>thresholdM || rz< thresholdm) {
      continue;
    }

    // store the inlier
    if (inliers0) {
      *inliers0++ = p0x;
      *inliers0++ = p0y;
      *inliers0++ = p0z;
    }
    if (inliers1) {
      *inliers1++ = *(_P1-3);
      *inliers1++ = *(_P1-2);
      *inliers1++ = *(_P1-1);
    }
    if (inlierIndices) {
      *inlierIndices++ = i;
    }
    numInLiers++;
  }
#ifdef DEBUG
  cout << "Num of Inliers: "<<numInLiers<<endl;
#endif
  return numInLiers;
}