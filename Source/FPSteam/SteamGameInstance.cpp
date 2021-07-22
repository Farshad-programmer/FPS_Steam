// Fill out your copyright notice in the Description page of Project Settings.


#include "SteamGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "OnlineSubsystem.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"
#include "OnlineSessionSettings.h"

USteamGameInstance::USteamGameInstance()
{
	
	MySessionName = "My Session";
	
	//Hardcode Map 1
	static::ConstructorHelpers::FObjectFinder<UTexture2D>Map1Image(TEXT("/Game/Steam/MapImages/01"));
	if (Map1Image.Object)
	{
		FMapInfo Map;
		Map.MapName = "First Person Ex Map";
		Map.MapURL = "/Game/FirstPersonCPP/Maps/FirstPersonExampleMap";
		Map.MapImage = Map1Image.Object;
		MapList.Add(Map);

		//Set Default map for server
		SelectedMapURL = Map.MapURL;
	}
	// Hardcode Map 2
	static::ConstructorHelpers::FObjectFinder<UTexture2D>Map2Image(TEXT("/Game/Steam/MapImages/02"));
	if (Map2Image.Object)
	{
		FMapInfo Map;
		Map.MapName = "First Person Ex Map Number 2";
		Map.MapURL = "/Game/FirstPersonCPP/Maps/FirstPersonExampleMap2";
		Map.MapImage = Map2Image.Object;
		MapList.Add(Map);
	}
}

void USteamGameInstance::Init()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem)
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// Bind Delegate here
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USteamGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USteamGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USteamGameInstance::OnJoinSessionComplete);
		}
	}
}

void USteamGameInstance::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete,Succeeded : %d"), Succeeded);
	if (Succeeded)
	{
		GetWorld()->ServerTravel(SelectedMapURL + "?listen");
	}
}

void USteamGameInstance::OnFindSessionComplete(bool Succeeded)
{
	SearchingForServer.Broadcast(false);
	
	if (Succeeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete,Succeeded : %d"), Succeeded);
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		int32 ArrayIndex = 0;
		for (FOnlineSessionSearchResult Result : SearchResults)
		{
			if (!Result.IsValid())
			{
				continue;
			}

			FServerInfo Info;
			FString ServerName = "Empty Server Name";
			FString HostName = "Empty Host Name";

			Result.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), ServerName);

			Info.ServerName = ServerName;
			Info.MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			Info.CurrentPlayers = Info.MaxPlayers - Result.Session.NumOpenPublicConnections;
			Info.SetPlayerCount();
			Info.IsLan = Result.Session.SessionSettings.bIsLANMatch;
			Info.Ping = Result.PingInMs;
			Info.ServerArrayIndex = ArrayIndex;

			
			ServerListDel.Broadcast(Info);
			ArrayIndex++;
			
		}


		/*UE_LOG(LogTemp, Warning, TEXT("SearchResults,Server Count : %d"), SearchResults.Num());
		if (SearchResults.Num())
		{
			UE_LOG(LogTemp, Warning, TEXT("Joining Server"));
			SessionInterface->JoinSession(0, "My Session", SearchResults[0]);
		}*/
		
	}
}

void USteamGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete"));
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (PC)
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);
		if (JoinAddress != "")
		{
			PC->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void USteamGameInstance::CreateServer(FCreateServerInfo ServerInfo)
{
	
	UE_LOG(LogTemp, Warning, TEXT("Create Server"));
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = true;// IsLan
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = ServerInfo.MaxPlayers;

	SessionSettings.Set(FName("SERVER_NAME_KEY"), ServerInfo.ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);
}

void USteamGameInstance::FindServers()
{
	SearchingForServer.Broadcast(true);
	
	UE_LOG(LogTemp, Warning, TEXT("Find Servers"));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = true;// IsLan
	SessionSearch->MaxSearchResults = 10000;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	
}

void USteamGameInstance::JoinServer(int32 ArrayIndex)
{
	FOnlineSessionSearchResult Result = SessionSearch->SearchResults[ArrayIndex];
	if (Result.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining server at index : %d"), ArrayIndex);
		SessionInterface->JoinSession(0, MySessionName, Result);
	}
}

void USteamGameInstance::FillMapList()
{
	for (FMapInfo Map : MapList)
	{
		FMapNameDel.Broadcast(Map.MapName);
	}
}

UTexture2D* USteamGameInstance::GetMapImage(FString MapName)
{
	for (FMapInfo Map : MapList)
	{
		if (Map.MapName.Equals(MapName))
		{
			return Map.MapImage;
		}
		
	}
	return nullptr;
}

void USteamGameInstance::SetSelectedMap(FString MapName)
{
	for (FMapInfo Map : MapList)
	{
		if (Map.MapName.Equals(MapName))
		{
			SelectedMapURL = Map.MapURL;
		}
	}
}









