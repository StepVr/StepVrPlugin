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

/**
* Spring glove
**/
typedef unsigned char U8;
typedef void(*SpringVR_tcb_procFd)(U8* pFd_i, U8 comNum_i);
extern "C" STEPVR_API void StepVR_initSpring(SpringVR_tcb_procFd pcb_procFd_i);
extern "C" STEPVR_API void SendSpringCmd(U8 *cmd);

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
		/**
		* Enumerates the names of node ID.
		**/
		typedef unsigned char NODEID;

		//THIS ENUM WILL DISCARD,PLEASE DON'T USE IT
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

		/**
		* justify data valid or not
		*
		* @return data valid
		*/
		STEPVR_API int GetDataValid(NODEID _nodeid);


		/**
		* Get north angle.
		*
		* @return angle
		*/
		STEPVR_API float GetNorthAngle(NODEID _nodeid);

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
		* Returns the most recent frame of raw skeleton posture data.
		*
		* @returns The specified CalibrateFrame.
		**/
		STEPVR_API unsigned char* GetCalFrame();
		
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