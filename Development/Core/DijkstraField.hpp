#pragma once

class DijkstraNode;
class DijkstraField;

/*!
	Dijkstra Edge Mode
*/
namespace DijkstraEdgeMode
{
	enum
	{
		Cross = 0, //!<Cross mode (up-down-left-right)
		Adjacent //!<Adjacent mode (all nodes around a certain node
	};
};

/*!
	A Dijkstra Edge
*/
struct DijkstraEdge 
{
	f32 Weight;
	DijkstraNode *Target;

	DijkstraEdge() : Weight(0.0f), Target(NULL) {};
};

/*!
	A Dijkstra Node
*/
class FLAMING_API DijkstraNode 
{
public:
	Vector2 Position;
	bool Visited;
	f32 Cost;
	DijkstraEdge Edges[8];
	DijkstraNode *Parent;
	MemoryStream UserData;
	DijkstraField *Owner;

	DijkstraNode() : Visited(false), Cost(9999), Parent(NULL), Owner(NULL) {};

	void Relax();
};

/*!
	Dijkstra Field
*/
class FLAMING_API DijkstraField 
{
	friend class DijkstraNode;
private:
	std::vector<DijkstraNode> Nodes;
	std::vector<AxisAlignedBoundingBox> Regions;
	DijkstraNode *FindCheapestNode();
	uint32 EdgeMode;
public:
	/*!
		A Callback to check whether a Node is Valid
		\param Owner the Dijkstra Field that owns this node
		\param Node the Node to check
		\note Usually you'll want to just get the Position inside the Dijkstra Node
		\sa DijkstraNode
	*/
	typedef bool (*NodeIsValidFn)(DijkstraField *Owner, DijkstraNode *Node);

	NodeIsValidFn NodeIsValidFunction;

	void *UserData;

	/*!
		Dijkstra Field Constructor
		\param EdgeMode one of DijkstraEdgeMode::*
		\sa DijkstraEdgeMode
	*/
	DijkstraField(uint32 EdgeMode = DijkstraEdgeMode::Cross);
	/*!
		Clears the Costs and Visited status of the Field
		\note Does NOT remove the nodes
	*/
	void Clear();

	/*!
		Removes the Nodes already allocated
	*/
	void ClearNodes()
	{ 
		Nodes.clear();
		Regions.clear();
	};

	/*!
		Add a rectangular region at position Position and size Size
		\param Position the Region's Position
		\param Size the Region's Size
	*/
	void AddRegionRect(const Vector2 &Position, const Vector2 &Size);

	/*!
		Obtains a Node from a Position
		\param Position the Node's Position
		\return the Node
	*/
	DijkstraNode *GetNode(const Vector2 &Position);

	/*!
		Finds a path between two nodes
		\param From the Node we're standing on
		\param To the targetted Node
		\param Directions the Directions to the target node
		\return Whether we found the path
	*/
	bool FindPath(DijkstraNode *From, DijkstraNode *To, std::vector<DijkstraNode *> &Directions);

	/*!
		Finds the Nodes around a certain Position
		\param Position our Targetted Position
		\param Radius the Radius of our search
		\return the List of Nodes
	*/
	std::vector<Vector2> GetValidNodesAroundPosition(const Vector2 &Position, uint32 Radius);
};
