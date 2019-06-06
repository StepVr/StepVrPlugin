#ifndef cpuid_h__
#define cpuid_h__

#include <string>
#include <map>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef LICENSE_EXPORTS
#define LICENSE_API __declspec(dllexport)
#else
#define LICENSE_API __declspec(dllimport)
#endif

	LICENSE_API bool Compare(char *lpszid);

	LICENSE_API bool GetId(char *lpszid, int len = 128);

	LICENSE_API bool GetId_BD(char *lpszid, int len = 128);

	LICENSE_API bool GetId_MK(char *lpszid, int len = 128);

	LICENSE_API bool GetId_P(char *lpszid, int len = 128);

	//get game id from gamelicense.bin file
	LICENSE_API bool GetGameId(const char *lpszlicensefilepath, char *lpszgameid, int len = 4);
	//get pc id from gamelicense.bin file
	LICENSE_API bool GetPcId(const char *lpszlicensefilepath, char *lpszpcid, int len = 128);



	LICENSE_API int CheckGameOrder(std::string strUserId, std::string strGameId, std::string strKeyId);

	LICENSE_API int CheckGameOrderForCSharp(const char* strUserId, const char* strGameId, const char* strKeyId);

	LICENSE_API int CheckSDKOrder(std::string strUserId, std::string strSdkId, std::string strSdkKeyId);

	LICENSE_API int InsertGameOrder(std::string strUserId, std::string strGameId, std::string strKeyId);

	LICENSE_API int InsertSDKOrder(std::string strUserId, std::string strSDKId, std::string strSDKKeyId);

	LICENSE_API int CheckSDKBindEnable(std::string strSDKKeyId);

	LICENSE_API int CheckGameBindEnable(std::string strGameKeyId);

	LICENSE_API int CheckGameBindEnableForCSharp(const char* strGameKeyId);

	LICENSE_API int BindGameKeyWithPc(std::string strUserId, std::string strGameId, std::string strKeyId, std::string pcId);

	LICENSE_API int BindGameKeyWithPcForCSharp(const char* strUserId, const char* strGameId, const char* strKeyId, const char* pcId);

	LICENSE_API int BindSDKKeyWithPc(std::string strUserId, std::string strSdkId, std::string strSdkKeyId, std::string pcId);

	LICENSE_API int InsertCustomer(std::string strCustomerId, std::string strCustomerName);

	LICENSE_API int InsertGame(std::string strGameId, std::string strGameName);

	LICENSE_API int InsertSDKVersion(std::string strSDKId, std::string strSDKName);

	LICENSE_API int FetchAllCustomer(std::map<std::string, std::string>& customerVector);

	LICENSE_API int FetchAllGame(std::map<std::string, std::string>& gameVector);

	LICENSE_API int FetchAllSDK(std::map<std::string, std::string>& sdkVector);

	//the follow function will be discard.
	//LICENSE_API bool GetGameId(char *lpszgameid, int len = 4);

	//LICENSE_API bool GetPcId(char *lpszpcid, int len = 128);


#ifdef __cplusplus
}
#endif
#endif // cpuid_h__
