#include "FlamingCore.hpp"
namespace FlamingTorch
{
#	if USE_GRAPHICS
#	define TAG "UIManager"

	void UIGroup::PerformLayout()
	{
		f32 CurrentExtent = 0;

		switch(LayoutMode)
		{
		case UIGroupLayoutMode::Horizontal:
			for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetSize().x + Children[i]->GetExtraSize().x, i++)
			{
				Children[i]->SetPosition(Vector2(CurrentExtent + Children[i]->GetExtraSize().x / 2, Children[i]->GetPosition().y));
			};

			break;

		case UIGroupLayoutMode::Vertical:
			for(uint32 i = 0; i < Children.size(); CurrentExtent += Children[i]->GetSize().y + Children[i]->GetExtraSize().y, i++)
			{
				Children[i]->SetPosition(Vector2(Children[i]->GetPosition().x, CurrentExtent + Children[i]->GetExtraSize().y / 2));
			};

			break;
		};
	};
#endif
};
