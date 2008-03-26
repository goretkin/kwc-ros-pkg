#include "libTF.hh"

RefFrame::RefFrame() :
  parent(0),
  active(false)
{
  return;
}

void RefFrame::setParams(double a,double b,double c,double d,double e,double f)
{
  params[0]=a;
  params[1]=b;
  params[2]=c;
  params[3]=d;
  params[4]=e;
  params[5]=f;

}

NEWMAT::Matrix RefFrame::getMatrix()
{
  NEWMAT::Matrix mMat(4,4);
  fill_transformation_matrix(mMat,params[0],params[1],params[2],params[3],params[4],params[5]);
  return mMat;
}

NEWMAT::Matrix RefFrame::getInverseMatrix()
{
  NEWMAT::Matrix mMat(4,4);
  fill_transformation_matrix(mMat,params[0],params[1],params[2],params[3],params[4],params[5]);
  //todo create a fill_inverse_transform_matrix call to be more efficient
  return mMat.i();
}


TransformReference::TransformReference():
  mMat(4,4)
{
  

  return;
}

void TransformReference::set(unsigned int frameID, unsigned int parentID, double a,double b,double c,double d,double e,double f)
{
  frames[frameID].setParent(parentID);
  frames[frameID].setParams(a,b,c,d,e,f);
}


NEWMAT::Matrix TransformReference::get(unsigned int target_frame, unsigned int source_frame)
{
  NEWMAT::Matrix myMat(4,4);
  TransformLists lists = lookUpList(target_frame, source_frame);
  myMat = computeTransformFromList(lists);
  //  std::cout << myMat;
  return myMat;
}




bool RefFrame::fill_transformation_matrix(NEWMAT::Matrix & matrix, double ax,
                                                   double ay, double az, double yaw,
                                                   double pitch, double roll)
{
  double ca = cos(yaw);
  double sa = sin(yaw);
  double cb = cos(pitch);
  double sb = sin(pitch);
  double cg = cos(roll);
  double sg = sin(roll);
  double sbsg = sb*sg;
  double sbcg = sb*cg;


  double* matrix_pointer = matrix.Store();
  if (matrix.Storage() != 16)
    return false;

  matrix_pointer[0] =  ca*cb;
  matrix_pointer[1] = (ca*sbsg)-(sa*cg);
  matrix_pointer[2] = (ca*sbcg)+(sa*sg);
  matrix_pointer[3] = ax;
  matrix_pointer[4] = sa*cb;
  matrix_pointer[5] = (sa*sbsg)+(ca*cg);
  matrix_pointer[6] = (sa*sbcg)-(ca*sg);
  matrix_pointer[7] = ay;
  matrix_pointer[8] = -sb;
  matrix_pointer[9] = cb*sg;
  matrix_pointer[10] = cb*cg;
  matrix_pointer[11] = az;
  matrix_pointer[12] = 0.0;
  matrix_pointer[13] = 0.0;
  matrix_pointer[14] = 0.0;
  matrix_pointer[15] = 1.0;

  return true;
};



TransformReference::TransformLists TransformReference::lookUpList(unsigned int target_frame, unsigned int source_frame)
{
  TransformLists mTfLs;

  //  std::vector<unsigned int> tList;
  //  std::vector<unsigned int> sList;

  //  std::vector<unsigned int> retVec;

  unsigned int frame = target_frame;
  while (frame != 0)
    {
      if (frames[frame].getParent() > 100) exit(-1); //todo cleanup
      mTfLs.inverseTransforms.push_back(frame);
      //   std::cout <<"Frame: " << frame <<std::endl;
      frame = frames[frame].getParent();
    }
  mTfLs.inverseTransforms.push_back(0);
  
  frame = source_frame;
  while (frame != 0)
    {
      if (frames[frame].getParent() > 100) exit(-1); //todo cleanup
      mTfLs.forwardTransforms.push_back(frame);
      frame = frames[frame].getParent();
    }
  mTfLs.forwardTransforms.push_back(0);
  

  //todo fixme throw something
  //  if (tList.size() <= 1) exit(-1);
  // if (sList.size() <= 1) exit(-1);   // I think below will catch all these anyway
  
  while (mTfLs.inverseTransforms.back() == mTfLs.forwardTransforms.back())
    {
      //      std::cout << "removing " << mTfLs.inverseTransforms.back() << std::endl;
      mTfLs.inverseTransforms.pop_back();
      mTfLs.forwardTransforms.pop_back();
    }
  
  return mTfLs;

}

NEWMAT::Matrix TransformReference::computeTransformFromList(TransformLists lists)
{
  NEWMAT::Matrix retMat(4,4);
  retMat << 1 << 0 << 0 << 0
	 << 0 << 1 << 0 << 0
	 << 0 << 0 << 1 << 0
	 << 0 << 0 << 0 << 1;
  
  for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      retMat *= frames[lists.inverseTransforms[i]].getInverseMatrix();
      //      std::cout <<"Multiplying by " << std::endl << frames[lists.inverseTransforms[i]].getInverseMatrix() << std::endl; 
      //std::cout <<"Result "<<std::endl << retMat << std::endl;
   }
  for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++) 
    {
      retMat *= frames[lists.forwardTransforms[lists.forwardTransforms.size() -1 - i]].getMatrix(); //Do this list backwards for it was generated traveling the wrong way
      //      std::cout <<"Multiplying by "<<std::endl << frames[lists.forwardTransforms[i]].getMatrix() << std::endl;
      //std::cout <<"Result "<<std::endl << retMat << std::endl;
    }

  return retMat;
}


void TransformReference::view(unsigned int target_frame, unsigned int source_frame)
{
  TransformLists lists = lookUpList(target_frame, source_frame);

  std::cout << "Inverse Transforms:" <<std::endl;
  for (unsigned int i = 0; i < lists.inverseTransforms.size(); i++)
    {
      std::cout << lists.inverseTransforms[i];
      //      retMat *= frames[lists.inverseTransforms[i]].getInverseMatrix();
    }
  std::cout << std::endl;

  std::cout << "Forward Transforms: "<<std::endl ;
  for (unsigned int i = 0; i < lists.forwardTransforms.size(); i++) 
    {
      std::cout << lists.forwardTransforms[i];
      //      retMat *= frames[lists.inverseTransforms[lists.forwardTransforms.size() -1 - i]].getMatrix(); //Do this list backwards for it was generated traveling the wrong way
    }
  std::cout << std::endl;
  
}
