#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_NETWORK
#	define UPDATE_FPS 60

const int32 UpdateTime = 1000 / UPDATE_FPS;

	GameClient::GameClient() : Host(NULL), Peer(NULL)
	{
	};
	
	GameClient::~GameClient()
	{
		Destroy();
	};

	void GameClient::Destroy()
	{
		if(Host)
		{
			if(Peer)
			{
				enet_peer_disconnect(Peer, 0);
			};

			enet_host_flush(Host);
			enet_host_service(Host, NULL, 1000);
			enet_host_destroy(Host);
		};

		Host = NULL;
		Peer = NULL;
	};

	int8 GameClient::Initialize(const std::string &AddressString, uint16 Port, uint8 MaxChannels,
		uint32 MaxBandwidthIncoming, uint32 MaxBandwidthOutgoing)
	{
		Destroy();

		Host = enet_host_create(NULL, 1, MaxChannels, MaxBandwidthIncoming, MaxBandwidthOutgoing);

		if(Host == NULL)
		{
			Log::Instance.LogErr("GameClient", "Failed to initialize the network subsystem.");

			return -1;
		};

		ENetAddress Address;
		enet_address_set_host(&Address, AddressString.c_str());
		Address.port = Port;

		Peer = enet_host_connect(Host, &Address, MaxChannels, 0);

		Log::Instance.LogInfo("GameClient", "Client '0x%08x' is now connecting to '%s:%d'.", this, AddressString.c_str(), Port);

		OnConnected();

		return 1;
	};

	void GameClient::SendPacket(const MemoryStream &Packet, uint32 Flags)
	{
#if !ANDROID
		sf::Lock Lock(PacketQueueMutex);
#endif

		MemoryStream Temp;
		StringID CRC = CRC32::Instance.CRC((const uint8 *)Packet.Get(), (uint32)Packet.Length());
		Temp.Write2<StringID>(&CRC);
		Temp.Write2<uint8>((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		OutgoingPacketQueue[Flags].push_back(Temp);
	};

	void GameClient::SendPacketNoQueue(const MemoryStream &Packet, uint32 Flags)
	{
#if !ANDROID
		sf::Lock Lock(PacketQueueMutex);
#endif

		static MemoryStream Stream;

		Stream.Clear();

		uint32 Count = 1;
		Stream.Write2<uint32>(&Count);

		Count = (uint32)Packet.Length() + sizeof(StringID);
		Stream.Write2<uint32>(&Count);

		StringID CRC = CRC32::Instance.CRC((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		Stream.Write2<StringID>(&CRC);
		Stream.Write2<uint8>((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		ENetPacket *RealPacket = enet_packet_create(Stream.Get(), (uint32)Stream.Length(), Flags);

		enet_peer_send(Peer, 0, RealPacket);
		enet_host_flush(Host);
	};

	void GameClient::Update()
	{
		if(!Host)
			return;

		{
#if !ANDROID
			sf::Lock Lock(PacketQueueMutex);
#endif

			for(PacketQueue::iterator it = OutgoingPacketQueue.begin(); it != OutgoingPacketQueue.end(); it++)
			{
				if(!it->second.size())
				{
					continue;
				};

				MemoryStream Stream;
				uint32 Count = it->second.size();
				Stream.Write2<uint32>(&Count);

				while(it->second.begin() != it->second.end())
				{
					Count = (uint32)it->second.begin()->Length();
					Stream.Write2<uint32>(&Count);
					Stream.Write2<uint8>((uint8 *)it->second.begin()->Get(), Count);

					it->second.erase(it->second.begin());
				};

				ENetPacket *RealPacket = enet_packet_create(Stream.Get(), (uint32)Stream.Length(), it->first);

				enet_peer_send(Peer, 0, RealPacket);
				enet_host_flush(Host);
			};
		}

		static ENetEvent Event;

		if(enet_host_service(Host, &Event, 10) > 0)
		{
			switch(Event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				OnConnected();

				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				OnDisconnected();

				break;
			case ENET_EVENT_TYPE_RECEIVE:
				{
					if(Event.packet->dataLength < sizeof(StringID))
					{
						enet_packet_destroy(Event.packet);

						return;
					};

					MemoryStream Stream;
					Stream.Write2<uint8>((const uint8 *)Event.packet->data,
						Event.packet->dataLength);
					Stream.Seek(0);

					enet_packet_destroy(Event.packet);

					uint32 Count = 0;
					
					if(!Stream.Read2<uint32>(&Count))
					{
						Log::Instance.LogDebug("GameClient", "Unable to read packet count");

						break;
					};

					for(uint32 i = 0; i < Count; i++)
					{
						uint32 Length = 0;

						if(!Stream.Read2<uint32>(&Length))
						{
							Log::Instance.LogDebug("GameClient", "Unable to read packet length");

							break;
						};

						if(Stream.Position() + Length > Stream.Length())
						{
							Log::Instance.LogDebug("GameClient", "Invalid Packet Length: Too large for actual packet data");

							break;
						};

						StringID CRC = 0;

						if(!Stream.Read2<StringID>(&CRC))
						{
							Log::Instance.LogDebug("GameClient", "Unable to read packet CRC");

							break;
						};

						Length -= sizeof(StringID);

						StringID ActualCRC = CRC32::Instance.CRC((uint8 *)Stream.Get() + Stream.Position(), Length);

						if(CRC == ActualCRC)
						{
							MemoryStream NewStream;
							NewStream.Write2<uint8>((uint8 *)Stream.Get() + Stream.Position(), Length);
							NewStream.Seek(0);

							Stream.Seek(Stream.Position() + Length);

							OnPacketReceived(NewStream);
						}
						else
						{
							Log::Instance.LogWarn("GameClient", "CRC failure for a network packet of size '%d' ('%08x' != '%08x').",
								Length, CRC, ActualCRC);

							Stream.Seek(Stream.Position() + Length);
						};
					};
				};

				break;
			};
		};
	};

	GameServer::GameServer() : Host(NULL), ClientCounter(0)
	{
	};

	GameServer::~GameServer()
	{
		Destroy();
	};

	void GameServer::Destroy()
	{
		if(Host)
		{
			for(PeerMap::iterator it = Peers.begin(); it != Peers.end(); it++)
			{
				if(it->second)
				{
					enet_peer_disconnect(it->second, 0);
				};
			};

			enet_host_flush(Host);
			enet_host_service(Host, NULL, 1000);
			enet_host_destroy(Host);
		};

		Host = NULL;
	};

	int8 GameServer::Initialize(const std::string &AddressString, uint16 Port, uint16 MaxClients,
		uint8 MaxChannels, uint32 MaxBandwidthIncoming, uint32 MaxBandwidthOutgoing)
	{
		ENetAddress Address;

		if(AddressString == "localhost")
		{
			Address.host = ENET_HOST_ANY;
		}
		else
		{
			enet_address_set_host(&Address, AddressString.c_str());
		};

		Address.port = Port;

		Host = enet_host_create(&Address, MaxClients, MaxChannels, MaxBandwidthIncoming, MaxBandwidthOutgoing);

		if(Host == NULL)
		{
			Log::Instance.LogErr("GameServer", "Failed to initialize the network subsystem.");

			return -1;
		};

		Log::Instance.LogInfo("GameServer", "Successfully created a server.");

		return 1;
	};

	void GameServer::Update()
	{
		if(!Host)
			return;

		{
#if !ANDROID
			sf::Lock Lock(PacketQueueMutex);
#endif

			for(PacketQueue::iterator it = OutgoingPacketQueue.begin(); it != OutgoingPacketQueue.end(); it++)
			{
				if(!it->second.size())
				{
					continue;
				};

				for(PeerMap::iterator pit = Peers.begin(); pit != Peers.end(); pit++)
				{
					MemoryStream Stream;

					uint32 Count = 0;

					for(PacketQueueContent::iterator cit = it->second.begin(); cit != it->second.end(); cit++)
					{
						if(cit->ClientID == pit->first)
							Count++;
					};

					if(Count == 0)
						continue;

					Stream.Write2<uint32>(&Count);

					for(;;)
					{
						bool Found = false;

						for(PacketQueueContent::iterator cit = it->second.begin(); cit != it->second.end(); cit++)
						{
							if(cit->ClientID == pit->first)
							{
								Count = (uint32)cit->Stream.Length();
								Stream.Write2<uint32>(&Count);
								Stream.Write2<uint8>((uint8 *)cit->Stream.Get(), Count);

								it->second.erase(cit);

								Found = true;

								break;
							};
						};

						if(!Found)
							break;
					};

					ENetPacket *RealPacket = enet_packet_create(Stream.Get(), (uint32)Stream.Length(), it->first);

					enet_peer_send(pit->second, 0, RealPacket);
					enet_host_flush(Host);
				};
			};
		};

		static ENetEvent Event;

		if(enet_host_service(Host, &Event, 10) > 0)
		{
			switch(Event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				{
					std::vector<char> HostName(1024), IP(1024);

					if(0 != enet_address_get_host(&Event.peer->address, &HostName[0], HostName.size()))
					{
						HostName.clear();
					};

					if(0 != enet_address_get_host_ip(&Event.peer->address, &IP[0], IP.size()))
					{
						IP.clear();
					};

					ENetPeer *Peer = Event.peer;

					Peers[++ClientCounter] = Peer;

					if(IP.size() && HostName.size())
					{
						Log::Instance.LogInfo("GameServer", "A client connected from '%s' ('%s').",
							&IP[0], &HostName[0]);
					}
					else if(IP.size())
					{
						Log::Instance.LogInfo("GameServer", "A client connected from '%s'.", &IP[0]);
					}
					else if(HostName.size())
					{
						Log::Instance.LogInfo("GameServer", "A client connected from '%s'.", &HostName[0]);
					}
					else
					{
						Log::Instance.LogWarn("GameServer", "A client connected from an unknown location.");
					};

					OnClientConnected(ClientCounter);
				};

				break;
			case ENET_EVENT_TYPE_DISCONNECT:

				{
					ENetPeer *Peer = Event.peer;

					for(PeerMap::iterator it = Peers.begin(); it != Peers.end(); it++)
					{
						if(it->second == Peer)
						{
							OnClientDisconnected(it->first);
							Peers.erase(it);
							
							break;
						};
					};
				};

				break;
			case ENET_EVENT_TYPE_RECEIVE:
				{
					if(Event.packet->dataLength < sizeof(StringID))
					{
						enet_packet_destroy(Event.packet);

						return;
					};

					MemoryStream Stream;
					Stream.Write2<uint8>((const uint8 *)Event.packet->data,
						Event.packet->dataLength);
					Stream.Seek(0);

					enet_packet_destroy(Event.packet);

					uint32 Count = 0;
					
					if(!Stream.Read2<uint32>(&Count))
						break;

					for(uint32 i = 0; i < Count; i++)
					{
						uint32 Length = 0;

						if(!Stream.Read2<uint32>(&Length))
							break;

						if(Stream.Position() + Length > Stream.Length())
							break;

						StringID CRC = 0;

						if(!Stream.Read2<StringID>(&CRC))
							break;

						Length -= sizeof(StringID);

						StringID ActualCRC = CRC32::Instance.CRC((uint8 *)Stream.Get() + Stream.Position(), Length);

						if(CRC == ActualCRC)
						{
							MemoryStream NewStream;
							NewStream.Write2<uint8>((uint8 *)Stream.Get() + Stream.Position(), Length);
							NewStream.Seek(0);

							Stream.Seek(Stream.Position() + Length);

							for(PeerMap::iterator it = Peers.begin(); it != Peers.end(); it++)
							{
								if(it->second == Event.peer)
								{
									OnPacketReceived(it->first, NewStream);

									break;
								};
							};
						}
						else
						{
							Log::Instance.LogWarn("GameClient", "CRC failure for a network packet of size '%d' ('%08x' != '%08x').",
								Length, CRC, ActualCRC);

							Stream.Seek(Stream.Position() + Length);
						};
					};
				};

				break;
			};
		};
	};

	void GameServer::SendPacket(uint32 ClientID, const MemoryStream &Packet, uint32 Flags)
	{
#if !ANDROID
		sf::Lock Lock(PacketQueueMutex);
#endif

		if(Peers.find(ClientID) == Peers.end())
		{
			Log::Instance.LogDebug("GameServer", "Unable to find Client ID %d", ClientID);

			return;
		};

		static PacketInfo Info;

		Info.Stream.Clear();
		Info.ClientID = ClientID;

		StringID CRC = CRC32::Instance.CRC((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		Info.Stream.Write2<StringID>(&CRC);
		Info.Stream.Write2<uint8>((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		OutgoingPacketQueue[Flags].push_back(Info);
	};

	void GameServer::SendPacketNoQueue(uint32 ClientID, const MemoryStream &Packet, uint32 Flags)
	{
#if !ANDROID
		sf::Lock Lock(PacketQueueMutex);
#endif

		if(Peers.find(ClientID) == Peers.end())
		{
			Log::Instance.LogDebug("GameServer", "Unable to find Client ID %d", ClientID);

			return;
		};

		static MemoryStream Stream;

		Stream.Clear();

		uint32 Count = 1;
		Stream.Write2<uint32>(&Count);

		Count = (uint32)Packet.Length() + sizeof(StringID);
		Stream.Write2<uint32>(&Count);

		StringID CRC = CRC32::Instance.CRC((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		Stream.Write2<StringID>(&CRC);
		Stream.Write2<uint8>((const uint8 *)Packet.Get(), (uint32)Packet.Length());

		ENetPacket *RealPacket = enet_packet_create(Stream.Get(), (uint32)Stream.Length(), Flags);

		enet_peer_send(Peers[ClientID], 0, RealPacket);
		enet_host_flush(Host);
	};

	SuperSmartPointer<GameClient> GameNetwork::GetClient(StringID ID)
	{
		ClientMap::iterator it = Clients.find(ID);

		if(it == Clients.end() || it->second.Get() == NULL)
		{
			SuperSmartPointer<GameClient> Client(new GameClient());
			Clients[ID] = Client;

			return Client;
		};

		return it->second;
	};

	SuperSmartPointer<GameServer> GameNetwork::GetServer(StringID ID)
	{
		ServerMap::iterator it = Servers.find(ID);

		if(it == Servers.end() || it->second.Get() == NULL)
		{
			SuperSmartPointer<GameServer> Server(new GameServer());
			Servers[ID] = Server;

			return Server;
		};

		return it->second;
	};

	GameNetwork GameNetwork::Instance;

	void GameNetwork::StartUp(uint32 Priority)
	{
		SUBSYSTEM_STARTUP_CHECK()

		SubSystem::StartUp(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		Log::Instance.LogInfo("GameNetwork", "Initializing Game Network...");

		LastUpdateTime = GameClockTimeNoPause();

		enet_initialize();
	};

	void GameNetwork::Shutdown(uint32 Priority)
	{
		SUBSYSTEM_PRIORITY_CHECK();

		SubSystem::Shutdown(Priority);

		Log::Instance.LogInfo("GameNetwork", "Terminating Game Network...");

		enet_deinitialize();
	};

	void GameNetwork::Update(uint32 Priority)
	{
		PROFILE("GameNetwork::Update", StatTypes::Networking);

		SubSystem::Update(Priority);

		SUBSYSTEM_PRIORITY_CHECK();

		uint64 CurrentTime = GameClockTimeNoPause();

		if(CurrentTime - LastUpdateTime >= UpdateTime)
		{
			LastUpdateTime = CurrentTime;

			for(ClientMap::iterator it = Clients.begin(); it != Clients.end(); it++)
			{
				if(!it->second.Get())
				{
					continue;
				};

				it->second->Update();
			};

			for(ServerMap::iterator it = Servers.begin(); it != Servers.end(); it++)
			{
				if(!it->second.Get())
				{
					continue;
				};

				it->second->Update();
			};
		};
	};
#endif
};
