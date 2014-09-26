[
	"Frame": [
		"Content": {
			"Control": "Sprite",
			"Path": "/UIThemes/PolyCode/windowBg.png",
			"NinePatch": "24,24,24,24",
			"Wide": "100%",
			"Tall": "100%",
			"ContentPanel": true,
			"Visible": false
		}
	],
	"Button": [
		"Button": {
			"Control": "Sprite",
			"Path": "/UIThemes/PolyCode/button.png",
			"NinePatch": "12,12,12,12",
			"Wide": "100%",
			"Tall": "100%",
			"Visible": false,
			"OnMouseJustPressed": "
				Self.Sprite.Texture = g_ResourceManager:GetTextureFromPackage(\"/UIThemes/PolyCode/\", \"buttonFocused.png\")
			",
			"OnMouseReleased": "
				Self.Sprite.Texture = g_ResourceManager:GetTextureFromPackage(\"/UIThemes/PolyCode/\", \"button.png\")
			",
			"Properties": {
				"Caption": {
					"Set": "
						if type(Value) ~= type(\"\") then
							g_Log:Info(\"Unable to set Caption: Invalid Value type '\" .. tostring(type(Value)) .. \"'\")
							return
						end
						
						Self.Properties[PropertyName].Value = Value
						
						local TextSize = RenderTextUtils.MeasureTextSimple(Self.Manager.Renderer,
							Self.Properties[PropertyName].Value, Self.Children[1].TextParameters).FullSize
						
						if TextSize.x == 0 then
							return
						end
						
						local Difference = (Self.Size - TextSize) / 2
						Self.Parent.Size = TextSize + Self.ScaledExtraSize
						Self.Parent.Position = Self.Parent.Position + Difference
						Self.Size = TextSize
						Self.Sprite.Options:Scale(Self.Size)
						Self.Children[1].Size = TextSize
						Self.Children[1].Text = Value
					",

					"Get": "
						return Value
					"
				}
			},
			
			"Children": [
				"Caption": {
					"Control": "Text",
					"Wide": "100%",
					"Tall": "100%",
					"Xpos": "Parent.ExtraSize.x / 2",
					"Ypos": "Parent.ExtraSize.y / 2",
					"FontSize": 16,
					"MouseInput": false,
					"Alignment": "CENTER|VCENTER"
				}
			]
		}
	],
	"Checkbox": [
		"Checkbox": {
			"Control": "Group",
			"Wide": "100%",
			"Tall": "20",
			"Properties": {
				"Checked": {
					"Set": "
						if type(Value) ~= \"boolean\" then
							g_Log:Info(\"Unable to set Checked: Invalid Value type '\" .. tostring(type(Value)) .. \"'\")
							return
						end
						
						Self.Properties[PropertyName].Value = Value
						
						local TargetElement = Self.Children[2]

						if Self.Properties[PropertyName].Value then
							TargetElement.Sprite.Texture = g_ResourceManager:GetTextureFromPackage(\"/UIThemes/PolyCode/\",
								\"checkbox_checked.png\")
						else
							TargetElement.Sprite.Texture = g_ResourceManager:GetTextureFromPackage(\"/UIThemes/PolyCode/\",
								\"checkbox_unchecked.png\")
						end
						
						TargetElement.Sprite.Options:Scale(MathUtils.ScaleKeepingAspectRatio(TargetElement.Size,
							TargetElement.Sprite.Texture.Size) / TargetElement.Sprite.Texture.Size):
							Color(Vector4(1, 1, 1, 1))
					",
					"Get": "
						return Value
					",
					"Default": "false"
				},
				"Caption": {
					"Set": "
						if type(Value) ~= \"string\" then
							g_Log:Info(\"Unable to set Caption: Invalid Value type '\" .. tostring(type(Value)) .. \"'\")
							return
						end
						
						Self.Properties[PropertyName].Value = Value
						
						local TextSize = RenderTextUtils.MeasureTextSimple(Self.Manager.Renderer,
							Self.Properties[PropertyName].Value, Self.Children[1].TextParameters).FullSize
						
						if TextSize.x == 0 then
							return
						end
						
						local LastSizeY = Self.Size.y
						
						Self.Size = Vector2(25, 0) + TextSize
						
						if Self.Size.y < LastSizeY then
							Self.Size = Vector2(Self.Size.x, LastSizeY)
						end
						
						Self.Children[1].Size = TextSize
						Self.Children[1].Text = Value
					",
					"Get": "
						return Value
					",
					"Default": ""
				}
			},
			"OnClick": "
				if Button.Name ~= InputCenter.Mouse_Left then
					return
				end
				
				Self.Properties[\"Checked\"].Set(Self, \"Checked\", not Self.Properties[\"Checked\"].Value)
			",
			"Visible": false,
			
			"Children": [
				"Text": {
					"Control": "Text",
					"Wide": "100% - 25",
					"Tall": "100%",
					"Xpos": "25",
					"Text": "",
					"TextColor": "1,1,1,1",
					"MouseInput": false
				},
				"Check": {
					"Control": "Sprite",
					"Wide": "25",
					"Tall": "100%",
					"MouseInput": false
				}
			]
		}
	]
]
