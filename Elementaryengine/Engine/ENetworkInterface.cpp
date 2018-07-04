#include "ENetworkInterface.h"
#include <Game.h>




ENetworkInterface::ENetworkInterface()
{

	connectionInformation = new ConnectionInformation();
}


ENetworkInterface::~ENetworkInterface()
{
	delete connectionInformation;
}

void ENetworkInterface::SetupServer()
{

}

void ENetworkInterface::SetupClient()
{

}

void ENetworkInterface::Connect(string ip)
{
	
}

void ENetworkInterface::Disconnect()
{

}

void ENetworkInterface::Sync()
{
	if (isServer) {
		SyncServer();
	}
	else {

	}
	
}

void ENetworkInterface::SendPackage()
{
}

void ENetworkInterface::HandlePackages()
{

}

void ENetworkInterface::SyncServer()
{

}

void ENetworkInterface::SyncClient()
{
}

DWORD NetworkReciveThread(LPVOID lpParam)
{
	while (!Game::shouldClose) {
		Game::networkInterface->HandlePackages();
	}
	return 0;
}
