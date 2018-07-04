#pragma once
#include <string>

using namespace std;

struct ConnectionInformation {
	int port = 10001;
	int maxClients = 100;
	int timeout = 10000;
	string address;
};


class ENetworkInterface
{
public:
	bool connected = false;
	bool isServer;
	ConnectionInformation * connectionInformation;
	ENetworkInterface();
	~ENetworkInterface();

	void SetupServer();
	void SetupClient();
	void Connect(string ip);
	void Disconnect();
	void Sync();
	void SendPackage();
	void HandlePackages();
private:

	void SyncServer();
	void SyncClient();
};

