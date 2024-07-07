// Fill out your copyright notice in the Description page of Project Settings.

#include "SAP_ODBC_Manager.h"

// Sets default values
ASAP_ODBC_Manager::ASAP_ODBC_Manager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASAP_ODBC_Manager::BeginPlay()
{
	Super::BeginPlay();
}

void ASAP_ODBC_Manager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ASAP_ODBC_Manager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ASAP_ODBC_Manager::SAP_ODBC_Init_Environment()
{
	odbc::EnvironmentRef Environment = odbc::Environment::create();

	if (Environment.isNull())
	{
		return false;
	}

	this->SAP_ODBC_Environment = Environment;

	return true;
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Create(FString& Out_Code, USAP_ODBC_Connection*& CreatedConnection, FString In_Server, FString In_UserName, FString In_Password, bool bUseAutoCommit)
{
	if (In_Server.IsEmpty())
	{
		Out_Code = "Server address should not be empty !";
		return false;
	}

	if (In_UserName.IsEmpty())
	{
		Out_Code = "User name should not be empty !";
		return false;
	}

	if (this->SAP_ODBC_Environment.isNull())
	{
		Out_Code = "You have to create ODBC environment first !";
		return false;
	}

	odbc::ConnectionRef ConnectionRef = this->SAP_ODBC_Environment->createConnection();
	if (ConnectionRef.isNull())
	{
		Out_Code = "There is a problem while creating ODBC Connection referance !";
		return false;
	}

	const FString ConnectionId = In_Server + "&&" + In_UserName;

	USAP_ODBC_Connection* Connection_Object = NewObject<USAP_ODBC_Connection>();
	if (!Connection_Object->SetConnection(ConnectionRef, ConnectionId))
	{
		return false;
	}

	CreatedConnection = Connection_Object;
	this->MAP_Connections.Add(ConnectionId, Connection_Object);
	
	return true;
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Delete_Id(FString& Out_Code, FString ConnectionId)
{
	if (ConnectionId.IsEmpty())
	{
		return false;
	}

	USAP_ODBC_Connection* TargetConnection = *this->MAP_Connections.Find(ConnectionId);

	if (!IsValid(TargetConnection))
	{
		return false;
	}
	
	if (TargetConnection->IsConnectionValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	FString Out_Code_Stop;
	TargetConnection->Connection_Stop(Out_Code_Stop);

	MAP_Connections.Remove(ConnectionId);

	Out_Code = "Connection successfully deleted !";
	return true;
}

bool ASAP_ODBC_Manager::SAP_ODBC_Connection_Delete_Object(FString& Out_Code, UPARAM(ref) USAP_ODBC_Connection*& TargetConnection)
{
	if (!IsValid(TargetConnection))
	{
		return false;
	}

	if (TargetConnection->IsConnectionValid())
	{
		Out_Code = "Connection reference is not valid !";
		return false;
	}

	FString Out_Code_Stop;
	TargetConnection->Connection_Stop(Out_Code_Stop);
	
	this->MAP_Connections.Remove(TargetConnection->GetConnectionId());
	TargetConnection = nullptr;

	Out_Code = "Connection successfully deleted !";
	return true;
}