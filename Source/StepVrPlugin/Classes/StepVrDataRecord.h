// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"



#define DEAL_INTERVAL 		static int64 Ticks = 0; DealTicks(Ticks);

class FStepSaveData
{
public:
	virtual ~FStepSaveData() {}

	/**
	 *  计算数据间隔，使用静态变量
	 * 	static int64 _CacheTicks;
	 *	DealTicks(_CacheTicks);
	 */
	void DealTicks(int64& LastTicks)
	{
		int64 CurTicks = FDateTime::Now().GetTicks();
		DataInterval = FTimespan(CurTicks - LastTicks).GetTotalMilliseconds();
		LastTicks = CurTicks;
	}

	virtual void GetLine(FString& OutLine)
	{
		OutLine = FString::Format(TEXT("{0}\n"),
			{
				TEXT("None")
			});
	}

protected:
	double	DataInterval;
};



/**
 * 每一行数据需要FStepSaveData的子类
 * AddData() FStepSaveData子类
 */
template<class ItemType>
class FStepVrDataRecord
{
public:
	~FStepVrDataRecord()
	{
		CloseFile();
	}

	void CreateFile(const FString& FileName)
	{
		CloseFile();

		FString Paths = FPaths::ProjectSavedDir() + TEXT("StepReocrdOnline");

		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.DirectoryExists(*Paths))
		{
			if (!PlatformFile.CreateDirectoryTree(*Paths))
			{
				return;
			}
		}

		FString AllFileName = FileName +TEXT("_") + FDateTime::Now().ToString() + TEXT(".csv");
		Paths.Append("/");
		Paths.Append(AllFileName);
		HandleFile = IFileManager::Get().CreateFileWriter(*Paths);
		bRecord = true;
	}

	void CloseFile()
	{
		bRecord = false;

		//写入磁盘
		if (HandleFile)
		{
			HandleFile->Flush();
			delete HandleFile;
			HandleFile = nullptr;
		}
	}

	bool IsRecord()
	{   
		return bRecord;
	}

	void AddData(ItemType& Data)
	{
		QueueReceiveData.Enqueue(Data);
	}

	void SaveLineData()
	{
		if (bRecord && HandleFile)
		{
			ItemType __CacheSaveReceive;
			if (QueueReceiveData.Dequeue(__CacheSaveReceive))
			{
				FString strLine;
				(&__CacheSaveReceive)->GetLine(strLine);

				FTCHARToUTF8 UTF8String(*strLine);
				HandleFile->Serialize((UTF8CHAR*)UTF8String.Get(), UTF8String.Length() * sizeof(UTF8CHAR));
			}
		}
	}

private:
	TQueue<ItemType>	QueueReceiveData;
	TAtomic<bool>		bRecord = false;
	FArchive*			HandleFile = nullptr;
};


