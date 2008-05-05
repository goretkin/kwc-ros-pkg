#include "rosTF/rosTF.h"

class testServer : public ros::node
{
public:
  //constructor
  testServer() : ros::node("server"),count(2){
    pTFServer = new rosTFServer(*this);
  };

  rosTFServer * pTFServer;

  void test () {
    pTFServer->sendEuler(5,count++,1,1,1,1,1,1,100000,100000);
    pTFServer->sendDH(5,count++,1,1,1,1,100000,100000);
    pTFServer->sendQuaternion(5,count++,1,1,1,1,1,1,1,100000,100000);
  };

private:
  int count;

};

int main(int argc, char ** argv)
{
  ros::init(argc, argv);
  testServer myTestServer;
  
  while(myTestServer.ok())
    {
      myTestServer.test();
      sleep(1);
    }

  return 0;
};

