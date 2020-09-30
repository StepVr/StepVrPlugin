/*************************************************************************
*
* G-WEARABLES CONFIDENTIAL
* ________________________
*
* Copyright © 2013 G-Wearables, Inc. All Rights Reserved.
*
* Version 1.0
*
* NOTICE: All information contained herein is, and remains the
* property of G-Wearables and its suppliers, if any. The intellectual
* and technical concepts contained herein are proprietary to
* G-Wearables and its suppliers and may be covered by Chinese and
* Foreign Patents, patents in process, and are protected by trade
* secret or copyright law. Dissemination of this information or
* reproduction of this material is strictly forbidden unless prior
* written permission is obtained from G-Wearables.
*
***********************************************************************
**/

#ifndef StepVR_h__
#define StepVR_h__

#ifdef STEPVR_EXPORTS
#define STEPVR_API __declspec(dllexport) 
#else
#define STEPVR_API __declspec(dllimport) 
#endif

#include <string>
#include <vector>

typedef unsigned char U8;
typedef void(*Glove_tcb_procFd)(U8* pFd_i, U8 comNum_i);
extern "C" STEPVR_API void StepVR_initGlove(Glove_tcb_procFd pcb_procFd_i);
extern "C" STEPVR_API void SendGloveCmd(U8 *cmd);

typedef void(*SpringVR_tcb_procFd)(U8* pFd_i, U8 comNum_i);
extern "C" STEPVR_API void StepVR_initSpring(SpringVR_tcb_procFd pcb_procFd_i);
extern "C" STEPVR_API void SendSpringCmd(U8 *cmd);

STEPVR_API struct ST_FASTDATA
{
	unsigned int idx;
	unsigned char data[63];
};

namespace StepVR {
	class Manager;
	class Frame;
	class SingleNode;

	/**
	* Enumerates the names of Engine.
	**/
	STEPVR_API enum Engine
	{
		Engine_Unreal = 1,
		Engine_Unity = 2,
	};

	/**
	* The Vector3f struct represents a three-component 
	* mathematical vector or point such as position or rotation 
	* in three-dimensional space.
	*/
	STEPVR_API struct Vector3f
	{
		float x;  
		float y;  
		float z;  
		Vector3f(){};
		/**
		* Creates a new Vector with all components set to specified values.
		**/
		Vector3f(float xx, float yy, float zz) :
			x(xx), y(yy), z(zz) {};
	};

	/**
	* The Vector4f struct represents a four-component 
	* mathematical vector, such as quaternion.
	*/
	STEPVR_API struct Vector4f
	{
		float w;
		float x;
		float y;
		float z;
		Vector4f(){
			w = 1.0f;
			x = y = z = 0.0f;
		}
		/**
		* Creates a new Vector with all components set to specified values.
		**/
		Vector4f(float ww, float xx, float yy, float zz) :
			w(ww), x(xx), y(yy), z(zz) {};
	};



	/**
	* The SingleNode class represents a set of position and rotation
	* tracking data detected in a single frame.
	*
	* The StepVR System detects all the single-nodes within the 
	* tracking area, reporting head, back, left foot, right foot, left 
	* hand, and right hand positions and rotations.
	*
	* Access SingleNode objects through the instance of the Frame
	* class. SingleNode instance created by the constructor is invalid.
	**/
	STEPVR_API class SingleNode
	{
	public:
		//定位件ID【1 - 255】
		typedef unsigned char NODEID;

		//废弃的
		STEPVR_API enum NodeID
		{
			NodeID_LeftWrist = 1,
			NodeID_RightWrist = 2,
			NodeID_LeftAnkle = 3,
			NodeID_RightAnkle = 4,
			NodeID_Neck = 5,
			NodeID_Head = 6,
			NodeID_LeftUpperArm = 7,
			NodeID_RightUpperArm = 8,
			NodeID_Weapon = 9,
			NodeID_Back = 10,
			NodeID_McpLeftHand=11,
			NodeID_McpRightHand=12,
			NodeID_McpLeftFoot=13,
			NodeID_McpRightFoot=14,
			NodeID_LeftForeArm,
			NodeID_RightForeArm
		};

		/**
		* Enumerates the names of key ID.
		**/
		STEPVR_API enum KeyID
		{
			KeyA = 1,
			KeyB,
			KeyC,
			KeyD,
			KeyE,
			KeyF,
			KeyG,
			KeyH,
			MAX_KEY
		};
		

		/**
		* Constructs a PositionFrame object.
		* 
		* Frame instances created with this constructor are invalid. Get 
		* valid Frame objects by calling the Manager::GetPositionFrame()
		* function.
		**/
		STEPVR_API SingleNode(float* data);
		STEPVR_API ~SingleNode();

		/**
		* The position of a specified node in the calibrated room.
		*
		* @param NodeID A specified NodeID.
		* @returns Vector3f The position of a specified node.
		**/
		STEPVR_API Vector3f GetPosition(NODEID id);
		STEPVR_API Vector3f GetPosition(NodeID id);

		/**
		* The rotation of a specified node in the calibrated room.
		*
		* @param NodeID A specified NodeID.
		* @param Engine A specified Engine you use.
		* @returns Vector3f The rotation of a specified node.
		**/
		STEPVR_API Vector3f GetRotation(NODEID id, Engine engine);
		STEPVR_API Vector3f GetRotation(NodeID id, Engine engine);

		/**
		* The quaternion of a specified node in the calibrated room.
		*
		* @param NodeID A specified NodeID.
		* @returns Vector4f The quaternion of a specified node.
		**/
		STEPVR_API Vector4f GetQuaternion(NODEID id);
		STEPVR_API Vector4f GetQuaternion(NodeID id);

		/**
		* The IsDown() function detect button down state.
		*
		* @params KeyID A specified key on the weapon. For version 1,
		*               There is only one button, which is KeyA.
		* @return true, the button is down; false, the button is up.
		**/
		STEPVR_API bool GetKeyDown(NODEID _nodeid, KeyID _keyid);
		STEPVR_API bool GetKeyDown(NodeID _nodeid, KeyID _keyid);

		/**
		* The IsUp() function detect button up state.
		*
		* @params KeyID A specified key on the weapon. For version 1,
		*               There is only one button, which is KeyA.
		* @return true, the button is down; false, the button is up.
		**/
		STEPVR_API bool GetKeyUp(NODEID _nodeid, KeyID _keyid);
		STEPVR_API bool GetKeyUp(NodeID _nodeid, KeyID _keyid);

		/**
		* The GetKey() function detect once button down and up state.
		*
		* @params KeyID A specified key on the weapon. For version 1,
		*               There is only one button, which is KeyA.
		* @return true, detect button down and up once.
		**/
		STEPVR_API bool GetKey(NODEID _nodeid, KeyID _keyid);
		STEPVR_API bool GetKey(NodeID _nodeid, KeyID _keyid);

		//new key functions
		STEPVR_API bool GetKeyDown1(NODEID _nodeid, KeyID _keyid);
		STEPVR_API bool GetKeyUp1(NODEID _nodeid, KeyID _keyid);
		STEPVR_API bool GetKey1(NODEID _nodeid, KeyID _keyid);;
		/**
		* Glove's button state
		* id: 0->left  1->right
		**/
		STEPVR_API bool GetGloveKeyDown(int id);
		STEPVR_API bool GetGloveKeyLongPress(int id);
		STEPVR_API bool GetGloveKeyUp(int id);

		/**
		* The GetVelocity() function return speed of node.
		**/
		STEPVR_API float GetVelocity(NODEID _nodeid);
		STEPVR_API float GetVelocity(NodeID _nodeid);

		/**
		* Check standard parts link or not.
		*
		*@ return true link success.
		**/
		STEPVR_API bool IsHardWareLink(NODEID _nodeid);
		STEPVR_API bool IsHardWareLink(NodeID _nodeid);

		/**
		* Get joystick position X Y
		*
		*@ return pos_x and pos_y.
		**/
		STEPVR_API float GetJoyStickPosX(NODEID _nodeid);
		STEPVR_API float GetJoyStickPosY(NODEID _nodeid);

		//-1.0 - 0 - 1.0
		STEPVR_API float GetJoyStickPosX1(NODEID _nodeid);
		STEPVR_API float GetJoyStickPosY1(NODEID _nodeid);
		/**
		* Check standard parts link or not.
		*
		*@ return true link success.
		**/
		STEPVR_API float GetElectricity(NODEID _nodeid);
		STEPVR_API float GetElectricity(NodeID _nodeid);

	
		/**
		* Get valve of every node.
		*
		* @return valve
		*/
		STEPVR_API unsigned short GetValve(NODEID _nodeid);

		/**
		* Get time stamp of every node.
		*
		* @return valve
		*/
		STEPVR_API float GetTimeStamp(NODEID _nodeid);

		//硬件时间ms
		STEPVR_API unsigned int GetTimeStamp1(NODEID _nodeid);

		/**
		* justify data valid or not
		*
		* @return data valid
		*/
		STEPVR_API int GetDataValid(NODEID _nodeid);

		//计算后的PC时间s
		STEPVR_API float GetTimeStamp2(NODEID _nodeid);
		//计算前的PC时间s
		STEPVR_API float GetTimeStampStartAndDelay(NODEID _nodeid, float & dTime);

		STEPVR_API int GetPowerVol(NODEID _nodeid);
		STEPVR_API int GetFpgaVersion(NODEID _nodeid);
		STEPVR_API int GetSTVersion(NODEID _nodeid);
		STEPVR_API int GetHWVersion(NODEID _nodeid);
		STEPVR_API int GetProductNo(NODEID _nodeid);
		STEPVR_API int GetSerialNo(NODEID _nodeid);


		STEPVR_API Vector3f GetImuAcc(NODEID id);
		STEPVR_API Vector3f GetImuGyro(NODEID id);
		STEPVR_API Vector3f GetImuMag(NODEID id);

		STEPVR_API Vector3f GetSpeedVec(NODEID id);
		STEPVR_API Vector3f GetSpeedAcc(NODEID id);
		STEPVR_API Vector3f GetSpeedGyro(NODEID id);
	};

	STEPVR_API struct GERData{
		int data0;	int data1;	int data2;	int data3;	int data4;	int data5;	int data6;	int data7;	int data8;	int data9;
		int data10;int data11;int data12;int data13;int data14;int data15;int data16;int data17;int data18;int data19;
		int data20;int data21;int data22;int data23;int data24;int data25;int data26;int data27;int data28;int data29;
		int data30;int data31;int data32;int data33;int data34;int data35;int data36;int data37;int data38;int data39;
		int data40;int data41;int data42;int data43;int data44;int data45;int data46;int data47;int data48;int data49;
		int data50;int data51;int data52;int data53;int data54;int data55;int data56;int data57;int data58;	
	};

	//废弃的
	STEPVR_API struct IMUData{
		int data0;	int data1;	int data2;	int data3;	int data4;	int data5;	int data6;	int data7;	int data8;
	};
	/**
	* The Frame class contains the all the detected data in a single
	* frame.
	*
	* Access Frame object through the instance of the Manager class. 
	* A Frame includes body skeleton posture, which can be polled at
	* any time using GetWholeBody() function; single-node position and 
	* rotation, which can be polled at any time using GetSingleNode() 
	* function.
	**/
	STEPVR_API class Frame
	{
	public:

		/**
		* Constructs a Frame object.
		*
		* Frame instance created with this constructor is invalid.
		* Get valid Frame object by calling the Manager::GetFrame()
		* function.
		**/
		STEPVR_API Frame(float* data);
		STEPVR_API ~Frame();


		/**
		* Returns the most recent frame of each single-node data.
		*
		* @returns PositionFrame The specified PositionFrame.
		**/
		STEPVR_API SingleNode GetSingleNode();

		//得到同步板的延迟的时间间隔
		STEPVR_API float GetTimeInterval();

		//获取歌尔的数据 59字节
		STEPVR_API void GetGERData(char *data);
		STEPVR_API GERData GetGERData_CSharp();

		//废弃的
		STEPVR_API void GetImuData(short *data);
		//废弃的
		STEPVR_API IMUData GetImuData_CSharp();
	};

	STEPVR_API struct CmdBuf{
		int buf0 = 0;
		int buf1 = 0;
		int buf2 = 0;
		int buf3 = 0;
		int buf4 = 0;
		int buf5 = 0;
		int buf6 = 0;
		int buf7 = 0;
		int buf8 = 0;
		int buf9 = 0;

		int buf10 = 0;
		int buf11 = 0;
		int buf12 = 0;
		int buf13 = 0;
		int buf14 = 0;
		int buf15 = 0;
		int buf16 = 0;
		int buf17 = 0;
		int buf18 = 0;
		int buf19 = 0;

		int buf20 = 0;
		int buf21 = 0;
		int buf22 = 0;
		int buf23 = 0;
		int buf24 = 0;
		int buf25 = 0;
		int buf26 = 0;
		int buf27 = 0;
		int buf28 = 0;
		int buf29 = 0;

		int buf30 = 0;
		int buf31 = 0;
		int buf32 = 0;
		int buf33 = 0;
		int buf34 = 0;
		int buf35 = 0;
		int buf36 = 0;
		int buf37 = 0;
		int buf38 = 0;
		int buf39 = 0;

		int buf40 = 0;
		int buf41 = 0;
		int buf42 = 0;
		int buf43 = 0;
		int buf44 = 0;
		int buf45 = 0;
		int buf46 = 0;
		int buf47 = 0;
		int buf48 = 0;
		int buf49 = 0;

		int buf50 = 0;
		int buf51 = 0;
		int buf52 = 0;
		int buf53 = 0;
		int buf54 = 0;
		int buf55 = 0;
		int buf56 = 0;
		int buf57 = 0;
		int buf58 = 0;
		int buf59 = 0;
		
		int buf60 = 0;
		int buf61 = 0;
		int buf62 = 0;
		int buf63 = 0;
	};

	
	/**
	* The Manager class is the main interface to the StepVR SDK.
	*
	* Create an instance of this Manager class to access frames of 
	* tracking data. Frame data includes body skeleton posture, 
	* single-node position and rotation, and key state.
	*
	* When an instance of Manager is created, call Start() function 
	* to prepare and start all devices and StepVR SDK; and call Stop()
	* function to stop all devices and software. To start receive 
	* correct whole-body, put your body T-Pose and call CalibrateMocap()
	* function. No need to calibrate, if you just use single-node data.
	*
	* Call GetFrame() function to get the most recent frame of 
	* tracking data.
	* 
	* Call GetDevConfig() function to get current sensor state.
	**/
	STEPVR_API class Manager{
	public:
		/**
		* Constructs a Manager object.
		**/
		STEPVR_API Manager();
		STEPVR_API Manager(const char* configpath);
		STEPVR_API ~Manager();

		/**
		* When you create a manager object, Call Start() function to 
		* start to capture frames of tracking data.
		* Returns:   STEPVR_API int.
		*            0 is ok
		*            other is failed
		**/
		STEPVR_API int Start();

		/**
		* Call Stop() function to stop capturing frames of tracking data.
		**/
		STEPVR_API bool Stop();

		/**
		* Returns the most recent frame of skeleton posture data.
		*
		* @returns The specified MocapFrame.
		**/
		STEPVR_API Frame GetFrame();
		
		/**
		* Set receive position and rotation mode. block or non block 
		*
		**/
		STEPVR_API void SetBlock(bool b);

		/**
		* Set client type.
		*
		* 1 game
		* 2 calibrate Qt program
		*
		**/
		STEPVR_API void SetClientType(char clientType);

		/**
		* Send vibrate command.
		*
		*
		**/
		STEPVR_API void SendVibrate(int nodeid, int vibrateTime);
		
		/**
		* Send special command.
		* reset time stamp
		*
		**/
		STEPVR_API void SendStampZeroCmd(const unsigned char* cmd);


		STEPVR_API void SendDebugCmd(char* cmd, int len, int iCom);
		/**
		* Send glove command.
		* 
		*
		**/
		
		STEPVR_API void SendCommonCmd_CPP(int id, const unsigned char* cmd, int cmd_len);
		STEPVR_API void SendCommonCmd_CSharp(int id,CmdBuf cbuf, int cmd_len);
		STEPVR_API std::string GetVersion();

		STEPVR_API void RecordOneMinuter();
		STEPVR_API bool IsServerAlive();

		STEPVR_API const char * GetCompileTime();
		STEPVR_API const char * GetVersion1();

		STEPVR_API const char * GetServerCompileTime();
		STEPVR_API const char * GetServerVersion();

		STEPVR_API bool IsConnected();

		STEPVR_API unsigned char GetServerStatus();
		STEPVR_API unsigned char GetServerComCount();

		STEPVR_API void GetMmapLog(std::vector<std::string> & vStr);
		STEPVR_API void GetFastData(std::vector<ST_FASTDATA> & vStr);
	};

	//this class just for Unreal develop
	STEPVR_API enum EulerOrd{
		EulerOrder_YXZ,
		EulerOrder_ZYX,
		EulerOrder_XZY,
		EulerOrder_XYZ,
		EulerOrder_YZX,
		EulerOrder_ZXY
	};

	STEPVR_API class StepVR_EnginAdaptor
	{
	public:
		STEPVR_API static Vector4f toUserQuat(const Vector4f &p);
		STEPVR_API static Vector3f toUserEuler(const Vector4f &p);
		STEPVR_API static Vector3f toUserPosition(const Vector3f &p);

		STEPVR_API static void MapCoordinate(Vector3f UserX, Vector3f UserY, Vector3f UserZ);
		//Yaw-Pitch-Roll Order
		STEPVR_API static void setEulerOrder(EulerOrd order);

	private:
		StepVR_EnginAdaptor(){};
	};


}

#endif // StepVR_h__