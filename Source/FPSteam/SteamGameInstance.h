// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SteamGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FCreateServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString ServerName;
	UPROPERTY(BlueprintReadWrite)
	int32 MaxPlayers;
	UPROPERTY(BlueprintReadWrite)
	bool IsLan;
};


USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountStr;
	UPROPERTY(BlueprintReadOnly)
	bool IsLan;
	UPROPERTY(BlueprintReadOnly)
	int32 Ping;
	UPROPERTY(BlueprintReadOnly)
	int32 ServerArrayIndex;
	
	int32 CurrentPlayers;
	int32 MaxPlayers;

	// inja ma ba in metod kari mikonim ke "MaxPlayers" va "CurrentPlayers" ro kenar ham neshon bede.
	// Rasti Be khater in metod dige niazi nadarim ke variable haye "MaxPlayers" va "CurrentPlayers" dar blueprint namayesh dade shavand leza "BlueprintReadOnly" vasashon nazashtam
	void SetPlayerCount()
	{
		PlayerCountStr = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};

USTRUCT()
struct FMapInfo
{
	GENERATED_BODY()
public:
	FString MapName;
	FString MapURL;
	class UTexture2D* MapImage;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, FServerInfo, ServerListDel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerSearchingDel, bool, SearchingForServer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapInfoDel, FString, FMapNameDel);
/**
 * 
 */
UCLASS()
class FPSTEAM_API USteamGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USteamGameInstance();
	
protected:

	
	FName MySessionName;

	TArray<FMapInfo>MapList;

	FString SelectedMapURL;
	
	UPROPERTY(BlueprintAssignable)
	FServerDel ServerListDel;


	UPROPERTY(BlueprintAssignable)
	FMapInfoDel FMapNameDel;
	
	UPROPERTY(BlueprintAssignable)
	FServerSearchingDel SearchingForServer;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch>SessionSearch;

	virtual void Init()override;

	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName,EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FCreateServerInfo ServerInfo);

	UFUNCTION(BlueprintCallable)
	void FindServers();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 ArrayIndex);

	UFUNCTION(BlueprintCallable)
	void FillMapList();
	
	UFUNCTION(BlueprintCallable)
	class UTexture2D* GetMapImage(FString MapName);

	UFUNCTION(BlueprintCallable)
	void SetSelectedMap(FString MapName);
};
