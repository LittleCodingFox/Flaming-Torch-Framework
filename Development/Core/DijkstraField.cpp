#include "FlamingCore.hpp"
#define TAG "DijkstraField"

namespace FlamingTorch
{
	DijkstraNode *FromNode = NULL, *ToNode = NULL;

	static Vector2 DirectionsCross[4] = {
		Vector2(-1, 0),
		Vector2(0, -1),
		Vector2(1, 0),
		Vector2(0, 1),
	};

	static Vector2 DirectionsAdjacent[8] = {
		Vector2(-1, -1),
		Vector2(0, -1),
		Vector2(1, -1),
		Vector2(-1, 0),
		Vector2(1, 0),
		Vector2(-1, 1),
		Vector2(0, 1),
		Vector2(1, 1)
	};

	void DijkstraNode::Relax()
	{
		Visited = true;

		uint32 Length = Owner->EdgeMode == DijkstraEdgeMode::Adjacent ? 8 : 4;

		for(uint32 i = 0; i < Length; i++)
		{
			if(Edges[i].Target && Edges[i].Target->Cost > Cost + Edges[i].Weight)
			{
				Edges[i].Target->Cost = Cost + Edges[i].Weight;
				Edges[i].Target->Parent = this;
			};
		};
	};

	DijkstraField::DijkstraField(uint32 _EdgeMode) : EdgeMode(_EdgeMode)
	{
		NodeIsValidFunction = NULL;
	};

	void DijkstraField::AddRegionRect(const Vector2 &Position, const Vector2 &Size)
	{
		AxisAlignedBoundingBox AABB;
		AABB.min = Position;
		AABB.max = Position + Size;
		Regions.push_back(AABB);

		for(uint32 y = 0; y < Size.y; y++)
		{
			for(uint32 x = 0; x < Size.x; x++)
			{
				DijkstraNode Node;
				Node.Position = Vector2(Position.x + x, Position.y + y);
				Node.Owner = this;

				Nodes.push_back(Node);
			};
		};
	};

	void DijkstraField::Clear()
	{
		for(uint32 i = 0; i < Nodes.size(); i++)
		{
			uint32 Length = EdgeMode == DijkstraEdgeMode::Adjacent ? 8 : 4;

			Nodes[i].Visited = false;
			Nodes[i].Cost = 9999;
			Nodes[i].Parent = NULL;

			for(uint32 j = 0; j < Length; j++)
			{
				Vector2 Position = Nodes[i].Position + (EdgeMode == DijkstraEdgeMode::Cross ? DirectionsCross[j] : DirectionsAdjacent[j]);

				bool Found = false;

				for(uint32 k = 0; k < Nodes.size(); k++)
				{
					if(Nodes[k].Position == Position)
					{
						DijkstraEdge &Edge = Nodes[i].Edges[j];
						Edge.Weight = Nodes[i].Position.Distance(Position);
						Edge.Target = &Nodes[k];

						Found = true;

						break;
					};
				};

				if(!Found)
					Nodes[i].Edges[j].Target = NULL;
			};
		};
	};

	DijkstraNode *DijkstraField::GetNode(const Vector2 &Position)
	{
		int32 FoundIndex = -1;
		uint32 StartIndex = 0, SearchAmount = 0;

		for(uint32 i = 0; i < Regions.size(); i++)
		{
			Vector3 Size = Regions[i].max - Regions[i].min;

			SearchAmount = (uint32)(Size.x * Size.y);

			if(Regions[i].IsInside(Position))
			{
				for(uint32 i = StartIndex; i < StartIndex + SearchAmount; i++)
				{
					if(Nodes[i].Position == Position)
						return &Nodes[i];
				};
			};

			StartIndex += SearchAmount;
		};

		return NULL;
	};

	std::vector<Vector2> DijkstraField::GetValidNodesAroundPosition(const Vector2 &Position, uint32 Radius)
	{
		std::vector<Vector2> Result;

		for(int32 y = (int32)Position.y - Radius; y <= Position.y + (int32)Radius; y++)
		{
			for(int32 x = (int32)Position.x - Radius; x <= Position.x + (int32)Radius; x++)
			{
				Vector2 ActualPosition((f32)x, (f32)y);

				if(ActualPosition == Position || (NodeIsValidFunction && !NodeIsValidFunction(this, GetNode(ActualPosition))))
					continue;

				Result.push_back(ActualPosition);
			};
		};

		return Result;
	};

	DijkstraNode *DijkstraField::FindCheapestNode()
	{
		f32 BestCost = 9999;
		DijkstraNode *Target = NULL;

		for(uint32 i = 0; i < Nodes.size(); i++)
		{
			if(!Nodes[i].Visited && (NodeIsValidFunction && (NodeIsValidFunction(this, &Nodes[i]) ||
				FromNode == &Nodes[i] || ToNode == &Nodes[i])) && Nodes[i].Cost < BestCost)
			{
				BestCost = Nodes[i].Cost;
				Target = &Nodes[i];
			};
		};

		return Target;
	};

	bool DijkstraField::FindPath(DijkstraNode *From, DijkstraNode *To, std::vector<DijkstraNode *> &Directions)
	{
		Directions.clear();

		if(From == To)
			return true;

		Clear();

		FromNode = From;
		ToNode = To;

		From->Cost = 0;
		DijkstraNode *Target = NULL;

		for(;;)
		{
			Target = FindCheapestNode();

			if(!Target)
			{
				FromNode = NULL;
				ToNode = NULL;

				return false; //No nodes available. No path towards target.
			};

			Target->Relax();

			if(Target == To)
			{
				DijkstraNode *Parent = ToNode;

				while(Parent && Parent->Parent)
				{
					Directions.push_back(Parent);

					Parent = Parent->Parent;
				};

				DijkstraNode *Temp = NULL;

				for(uint32 i = 0, invI = Directions.size() - 1; i < Directions.size() / 2; i++, invI--)
				{
					Temp = Directions[invI];
					Directions[invI] = Directions[i];
					Directions[i] = Temp;
				};

				FromNode = NULL;
				ToNode = NULL;

				return true;
			};
		};
	};
};
