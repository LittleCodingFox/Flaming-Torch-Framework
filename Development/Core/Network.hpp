#pragma once

/*!
*	Writes a network string with a 8-bit length (255 max value)
*	\param Packet the packet to write to
*	\param String the string to write
*/
inline void NetworkWriteString8(MemoryStream &Packet, const std::string &String)
{
	FLASSERT(String.length() <= 255, "String cannot be properly stored in a NetworkWriteString8");

	uint8 Length = (uint8)String.length();

	Packet.Write2<uint8>(&Length);

	if(String.length())
	{
		Packet.Write2<char>(String.c_str(), Length);
	};
};

/*!
*	Writes a network string with a 16-bit length (65535 max value)
*	\param Packet the packet to write to
*	\param String the string to write
*/
inline void NetworkWriteString16(MemoryStream &Packet, const std::string &String)
{
	uint16 Length = (uint16)String.length();

	Packet.Write2<uint16>(&Length);

	if(String.length())
	{
		Packet.Write2<char>(String.c_str(), Length);
	};
};

/*!
*	Reads a network string with a 8-bit length (255 max value)
*	\param Packet the packet to read from
*	\return empty string on failure (or if it's an empty string anyway)
*/
inline std::string NetworkReadString8(MemoryStream &Packet)
{
	std::string Out;

	uint8 Length = 0;

	if(!Packet.Read2<uint8>(&Length))
		return std::string();

	if(Length)
	{
		Out.resize(Length);

		if(!Packet.Read2<char>(&Out[0], Length))
			return std::string();
	};

	return Out;
};

/*!
*	Reads a network string with a 16-bit length (65535 max value)
*	\param Packet the packet to read from
*	\return empty string on failure (or if it's an empty string anyway)
*/
inline std::string NetworkReadString16(MemoryStream &Packet)
{
	std::string Out;

	uint16 Length = 0;

	if(!Packet.Read2<uint16>(&Length))
		return std::string();

	if(Length)
	{
		Out.resize(Length);

		if(!Packet.Read2<char>(&Out[0], Length))
			return std::string();
	};

	return Out;
};

class FLAMING_API GameClient
{
	friend class GameNetwork;
private:
	typedef std::list<MemoryStream> PacketQueueContent;
	typedef std::map<uint32, PacketQueueContent> PacketQueue;
	PacketQueue OutgoingPacketQueue;

#if !ANDROID //TODO
	sf::Mutex PacketQueueMutex;
#endif

	ENetHost *Host;
	ENetPeer *Peer;

	GameClient();
	void Update();
public:
	~GameClient();
	void Destroy();
	int8 Initialize(const std::string &Address, uint16 Port, uint8 MaxChannels, uint32 MaxBandwidthIncoming,
		uint32 MaxBandwidthOutgoing);
	void SendPacket(const MemoryStream &Packet, uint32 Flags);
	void SendPacketNoQueue(const MemoryStream &Packet, uint32 Flags);
	Signal0<void> OnConnected;
	Signal0<void> OnDisconnected;
	Signal1<MemoryStream &> OnPacketReceived;
};

class FLAMING_API GameServer
{
	friend class GameNetwork;
private:
	class PacketInfo
	{
	public:
		MemoryStream Stream;
		uint32 ClientID;
	};

	typedef std::list<PacketInfo> PacketQueueContent;
	typedef std::map<uint32, PacketQueueContent> PacketQueue;
	typedef std::map<uint32, ENetPeer *> PeerMap;
	PacketQueue OutgoingPacketQueue;
	PeerMap Peers;

	uint32 ClientCounter;

#if !ANDROID //TODO
	sf::Mutex PacketQueueMutex;
#endif

	ENetHost *Host;

	GameServer();
	void Update();
public:
	~GameServer();
	void Destroy();
	int8 Initialize(const std::string &Address, uint16 Port, uint16 MaxClients, uint8 MaxChannels,
		uint32 MaxBandwidthIncoming, uint32 MaxBandwidthOutgoing);
	void SendPacket(uint32 ClientID, const MemoryStream &Packet, uint32 Flags);
	void SendPacketNoQueue(uint32 ClientID, const MemoryStream &Packet, uint32 Flags);
	Signal1<uint32> OnClientConnected;
	Signal1<uint32> OnClientDisconnected;
	Signal2<uint32, MemoryStream &> OnPacketReceived;
};

class FLAMING_API GameNetwork : public SubSystem
{
private:
	typedef std::map<StringID, SuperSmartPointer<GameClient> > ClientMap;
	typedef std::map<StringID, SuperSmartPointer<GameServer> > ServerMap;
	ClientMap Clients;
	ServerMap Servers;
	uint64 LastUpdateTime;

	GameNetwork() : SubSystem(NETWORK_PRIORITY), LastUpdateTime(0) {};
public:
	static GameNetwork Instance;

	~GameNetwork() {};

	SuperSmartPointer<GameClient> GetClient(StringID ID);
	SuperSmartPointer<GameServer> GetServer(StringID ID);

	void StartUp(uint32 Priority);
	void Shutdown(uint32 Priority);
	void Update(uint32 Priority);
};
