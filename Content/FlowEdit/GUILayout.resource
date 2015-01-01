[
	"TextBox": [
		"Content": {
			"Control": "Sprite",
			"Wide": "parent_wide - 6",
			"Tall": "parent_tall - 6",
			"XOffset": "3",
			"YOffset": "3",
			"Wireframe": true,
			"Color": "#000000",
			"Visible": false,
			
			"Children": [
				"Text": {
					"Control": "Text",
					"Wide": "parent_wide - 3",
					"Tall": "parent_tall - 3",
					"Text": "Cheesecakes",
					"FontSize": 14,
					"XOffset": "3",
					"OnGainFocus": "
						EditingText = Self
						EditingTextString = Self.Text
						EditingCursorOffset = 0
						EditingCursorPosition = 0
					",
					"OnLoseFocus": "
						EditingText = nil
					",
					"OnDraw": "
						if EditingText ~= nil and EditingText.ID == Self.ID then
							Self.Manager:DrawText(\"|\", RenderTextUtils.TextParams():Position(Self.ParentPosition +
								Vector2(EditingCursorOffset, 0)):FontSize(14))
						end
					",
					"OnCharacterEntered": "
						Self.Text = Self.Text .. Character
						EditingCursorPosition = EditingCursorPosition + 1
						EditingCursorOffset = EditingCursorOffset +
							RenderTextUtils.MeasureTextSimple(Self.Manager.Renderer, Character, RenderTextUtils.TextParams():FontSize(14)).Right
					",
					"OnMouseJustPressed": "
						local PreviousOffset = 0
						
						for i = 1, string.len(Self.Text) do
							local Size = RenderTextUtils.MeasureTextSimple(Self.Manager.Renderer, Self.Text:sub(1, i),
								RenderTextUtils.TextParams():FontSize(14))
							
							if Size.Right >= g_Input.MousePosition.x - Self.ParentPosition.x then
								EditingCursorOffset = PreviousOffset
								
								EditingCursorPosition = i - 1
								
								break
							elseif i == string.len(Self.Text) then
								EditingCursorOffset = Size.Right
								
								EditingCursorPosition = i
							else
								PreviousOffset = Size.Right
							end
						end
						
						if string.len(Self.Text) == 0 then
							EditingCursorOffset = 0
						end
					"
				}
			]
		}
	],
	"NodeRow": [
		"Content": {
			"Control": "Group",
			"Wide": "parent_wide",
			"Tall": "20",
			"Visible": false,
			"OnGainFocus": "
				Self.Parent.Parent.Parent.Parent.Children[1].Visible = true
			",
			"OnLoseFocus": "
				Self.Parent.Parent.Parent.Parent.Children[1].Visible = false
			",
			"OnMouseEntered": "
				if DrawingLine and SourceNode ~= nil and SourceNode.ID ~= Self.ID then
					Self.Children[4].Visible = true
					TargetNode = Self
				end
			",
			"OnMouseLeft": "
				if DrawingLine and SourceNode ~= nil and SourceNode.ID ~= Self.ID then
					Self.Children[4].Visible = false
					TargetNode = nil
				end
			",
			"OnMouseJustPressed": "
				if Button.Name == InputCenter.Mouse_Right then
					DrawingLine = true
					SourceNode = Self
					StartPositionOffset = 0
					
					if Self.ParentPosition.x + Self.Size.x / 2 < g_Input.MousePosition.x then
						StartPositionOffset = Self.Size.x
					end
				end
			",
			"OnMouseReleased": "
				if Button.Name == InputCenter.Mouse_Right then
					DrawingLine = false
					
					if TargetNode ~= nil then
						TargetNode.Children[4].Visible = false
						
						for key, value in pairs(NodeConnections) do
							if (key ~= nil and key.ID == Self.ID) or (value ~= nil and value.ID == Self.ID) then
								NodeConnections[key] = nil
							end
						end

						for key, value in pairs(NodeConnections) do
							if (key ~= nil and key.ID == TargetNode.ID) or (value ~= nil and value.ID == TargetNode.ID) then
								NodeConnections[key] = nil
							end
						end
						
						NodeConnections[Self] = TargetNode
						NodeConnections[TargetNode] = Self
						
						TargetNode = nil
					end
				end
			",
			"OnUpdate": "
				if DrawingLine and SourceNode ~= nil and Self.ID == SourceNode.ID then
					local StartPosition = Vector2(Self.ParentPosition)
					
					StartPosition.y = StartPosition.y + Self.Size.y / 2
					StartPosition.x = StartPosition.x + StartPositionOffset

					local EndPosition = Vector2(g_Input.MousePosition)
					
					DrawBezierLine(Self.Manager.Renderer, StartPosition, EndPosition)
				end
			",
			
			"Children": [
				"Background2": {
					"Control": "Sprite",
					"Wide": "parent_wide",
					"Tall": "parent_tall",
					"Color": "#5E5D5A",
					"MouseInput": false
				},
				"Title": {
					"Control": "Text",
					"Wide": "parent_wide",
					"Tall": "18",
					"FontSize": 16,
					"Text": "NAME",
					"Alignment": "VCenter|Center",
					"MouseInput": false
				},
				"Foreground": {
					"Control": "Sprite",
					"Wide": "parent_wide",
					"Tall": "parent_tall",
					"Color": "#000000",
					"Wireframe": true,
					"MouseInput": false
				},
				"Background": {
					"Control": "Sprite",
					"Wide": "parent_wide",
					"Tall": "parent_tall",
					"Color": "#1495A377",
					"Visible": false,
					"MouseInput": false
				}
			]
		}
	],
	"NodeTemplate": [
		"Content": {
			"Control": "Group",
			"Wide": "parent_wide",
			"Tall": "parent_tall",
			"XOffset": "3",
			"YOffset": "3",
			"Visible": false,
			"OnMouseMoved": "
				if g_Input:GetMouseButton(InputCenter.Mouse_Left).Pressed then
					Self.Parent.Position = Self.Parent.Position + g_Input.MouseMovement:ToVector2()
				end
			",
			"OnUpdate": "
				local Container = Self.Children[2].Children[2]
				local SizeY = 0
				
				for i = 1, #Container.Children do
					SizeY = SizeY + Container.Children[i].Size.y
				end
				
				Self.Parent.Size = Vector2(Self.Parent.Size.x, SizeY)
				Self.Size = Vector2(Self.Size.x, SizeY)
				Self.Children[1].Size = Vector2(Self.Children[1].Size.x, SizeY)
				Self.Children[1].Sprite.Options:Scale(Vector2(Self.Children[1].Size.x, SizeY + 6) / Self.Children[1].Sprite.Texture.Size)
				Self.Children[2].Size = Vector2(Self.Children[2].Size.x, SizeY)
				Self.Children[2].Children[1].Size = Vector2(Self.Children[2].Children[1].Size.x, SizeY)
				Self.Children[2].Children[1].Sprite.Options:Scale(Vector2(Self.Children[2].Children[1].Size.x, SizeY) / Self.Children[1].Sprite.Texture.Size)
				Self.Children[2].Children[2].Size = Vector2(Self.Children[2].Children[2].Size.x, SizeY)
			",
			
			"Children": [
				"Background": {
					"Control": "Sprite",
					"Wide": "parent_wide + 6",
					"Tall": "parent_tall + 6",
					"XOffset": "-3",
					"YOffset": "-3",
					"Color": "#1495A3",
					"Visible": false
				},
				"Inner": {
					"Control": "Group",
					"Wide": "parent_wide",
					"Tall": "parent_tall",

					"Children": [
						"Background": {
							"Control": "Sprite",
							"Wide": "parent_wide",
							"Tall": "parent_tall",
							"Color": "#5E5D5A"
						},
						"Content": {
							"Control": "HorizontalGroup",
							"Wide": "parent_wide",
							"Tall": "parent_tall",
							"Children": [
								"Node1": {
									"Control": "NodeRow"
								},
								"Node2": {
									"Control": "NodeRow"
								}
							]
						}
					]
				}
			]
		}
	],
	
	"Test": [
		"Content": {
			"Control": "Group",
			"Wide": "parent_wide",
			"Tall": "parent_tall",
			
			"Children": [
				"Test": {
					"Control": "NodeTemplate",
					"Wide": "100",
					"Tall": "40",
					"Xpos": "left",
					"Ypos": "center"
				},
				"Test2": {
					"Control": "NodeTemplate",
					"Wide": "100",
					"Tall": "40",
					"Xpos": "right",
					"Ypos": "center"
				}
			]
		}
	]
]
