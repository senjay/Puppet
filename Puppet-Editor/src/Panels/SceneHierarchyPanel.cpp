#include "SceneHierarchyPanel.h"
#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include "Puppet/Scene/Components.h"
#include "Puppet/Math/MathUtils.h"
#include "Puppet/Utils/PlatformUtils.h"
#include "../ImGuiUtils/ImGuiWrapper.h"
namespace Puppet {
	extern const std::filesystem::path g_AssetPath;

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
	}
	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");

		m_Context->m_Registry.each([&](auto entityID)
			{
				Entity entity{ entityID , m_Context.get() };
				DrawEntityNode(entity);
			});

		if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		// Right-click on blank space
		if(ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight| ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
				m_Context->CreateEntity("Empty Entity");

			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectionContext = entity;
	}
	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked())
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened)
		{
			// TODO: Children
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
			bool opened = ImGui::TreeNodeEx((void*)9817239, flags, tag.c_str());
			if (opened)
				ImGui::TreePop();
			ImGui::TreePop();
		}
		// Defer deletion until end of node UI
		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static bool DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
	{
		bool modified = false;
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
		{
			values.x = resetValue;
			modified = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
		{
			values.y = resetValue;
			modified = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
		{
			values.z = resetValue;
			modified = true;
		}
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();

		ImGui::Columns(1);

		ImGui::PopID();
		return modified;
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed 
			| ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;

				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			std::strncpy(buffer, tag.c_str(), sizeof(buffer));
			if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Camera"))
			{
				if (!m_SelectionContext.HasComponent<CameraComponent>())
					m_SelectionContext.AddComponent<CameraComponent>();
				else
					PP_CORE_WARN("This entity already has the Camera Component!");
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
				else
					PP_CORE_WARN("This entity already has the Sprite Renderer Component!");
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				
				bool updateTransform = false;
				updateTransform |= DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				updateTransform |= DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				updateTransform |= DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				auto& camera = component.Camera;

				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
						if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
						{
							currentProjectionTypeString = projectionTypeStrings[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));

				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;
						Ref<Texture2D> texture = Texture2D::Create(texturePath.string());
						if (texture->IsLoaded())
							component.Texture = texture;
						else
							PP_WARN("Could not load texture {0}", texturePath.filename().string());
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
			});
		
		DrawComponent<MeshComponent>("Mesh Renderer", entity, [](MeshComponent& component)
			{
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnWidth(0, 100.0f);
				ImGui::Text("Mesh Path");
				ImGui::NextColumn();

				std::string standardPath = component.Path;
				ImGui::Text(standardPath.c_str());
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						component.Path = (std::filesystem::path("assets") / path).string();
						component.m_Mesh = CreateRef<Mesh>(component.Path);
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::SameLine();
				if (ImGui::Button("..."))
				{
					std::string filepath = FileDialogs::OpenFile("Model (*.obj *.fbx *.dae *.gltf)\0");
					if (filepath.find("Assets") != std::string::npos)
					{
						filepath = filepath.substr(filepath.find("Assets"), filepath.length());
					}
					else
					{
						// TODO: Import Mesh
						//HE_CORE_ASSERT(false, "HEngine Now Only support the model from Assets!");
						//filepath = "";
					}
					if (!filepath.empty())
					{
						component.m_Mesh = CreateRef<Mesh>(filepath);
						component.Path = filepath;
					}
				}
				ImGui::EndColumns();

				if (ImGuiUtils::TreeNodeExStyle2((void*)"Material", "Material"))
				{
					uint32_t matIndex = 0;

					const auto& materialNode = [&matIndex = matIndex](const char* name, Ref<Material>& material, Ref<Texture2D>& tex, void(*func)(Ref<Material>& mat)) {
						std::string label = std::string(name) + std::to_string(matIndex);
						ImGui::PushID(label.c_str());

						if (ImGui::TreeNode((void*)name, name))
						{
							ImGui::Image((ImTextureID)tex->GetRendererID(), ImVec2(64, 64), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
							if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
								{
									const wchar_t* path = (const wchar_t*)payload->Data;
									std::string Path = (std::filesystem::path("assets") / path).string();
									tex = TextureLibrary::GetInstance().FindorAdd("xx", Path);
								}
								ImGui::EndDragDropTarget();
							}

							func(material);

							ImGui::TreePop();
						}

						ImGui::PopID();
					};

					for (auto& material : component.m_Mesh->m_Material)
					{
						std::string label = std::string("material") + std::to_string(matIndex);
						ImGui::PushID(label.c_str());

						if (ImGui::TreeNode((void*)label.c_str(), std::to_string(matIndex).c_str()))
						{
							materialNode("Albedo", material, material->mAlbedoMap, [](Ref<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseAlbedoMap);

								if (ImGui::ColorEdit4("##albedo", glm::value_ptr(mat->col)))
								{
									if (!mat->bUseAlbedoMap)
									{
										unsigned char data[4];
										for (size_t i = 0; i < 4; i++)
										{
											data[i] = (unsigned char)(mat->col[i] * 255.0f);
										}
										mat->albedoRGBA->SetData(data, sizeof(unsigned char) * 4);
									}
								}
								});

							materialNode("Normal", material, material->mNormalMap, [](Ref<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseNormalMap);
								});

							materialNode("Metallic", material, material->mMetallicMap, [](Ref<Material>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Metallic", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									ImGui::Checkbox("Use", &mat->bUseMetallicMap);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									if (ImGui::SliderFloat("##Metallic", &mat->metallic, 0.0f, 1.0f))
									{
										if (!mat->bUseMetallicMap)
										{
											unsigned char data[4];
											for (size_t i = 0; i < 3; i++)
											{
												data[i] = (unsigned char)(mat->metallic * 255.0f);
											}
											data[3] = (unsigned char)255.0f;
											mat->metallicRGBA->SetData(data, sizeof(unsigned char) * 4);
										}
									}

									ImGui::EndTable();
								}
								});

							materialNode("Roughness", material, material->mRoughnessMap, [](Ref<Material>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Roughness", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();

									ImGui::Checkbox("Use", &mat->bUseRoughnessMap);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									if (ImGui::SliderFloat("##Roughness", &mat->roughness, 0.0f, 1.0f))
									{
										if (!mat->bUseRoughnessMap)
										{
											unsigned char data[4];
											for (size_t i = 0; i < 3; i++)
											{
												data[i] = (unsigned char)(mat->roughness * 255.0f);
											}
											data[3] = (unsigned char)255.0f;
											mat->roughnessRGBA->SetData(data, sizeof(unsigned char) * 4);
										}
									}

									ImGui::EndTable();
								}
								});

							materialNode("Ambient Occlusion", material, material->mAoMap, [](Ref<Material>& mat) {
								ImGui::SameLine();
								ImGui::Checkbox("Use", &mat->bUseAoMap);
								});

							ImGui::TreePop();
						}

						matIndex++;

						ImGui::PopID();
					}

					ImGui::TreePop();
				}

				if (component.m_Mesh->bAnimated)
				{
					if (ImGuiUtils::TreeNodeExStyle2((void*)"Animation", "Animation"))
					{
						ImGuiUtils::DrawTwoUI(
							[&mesh = component.m_Mesh]() {
							static std::string label = "Play";
							if (ImGui::Button(label.c_str()))
							{
								mesh->bPlayAnim = !mesh->bPlayAnim;
								if (mesh->bPlayAnim)
									label = "Stop";
								else
								{
									label = "Play";
									mesh->m_Animator.Reset();
								}
							}
						},
							[&mesh = component.m_Mesh]() {
							static std::string label = "Pause";
							if (ImGui::Button(label.c_str()))
							{
								mesh->bStopAnim = !mesh->bStopAnim;
								if (mesh->bStopAnim)
									label = "Resume";
								else
									label = "Pause";
							}
						},
							88.0f
							);

						ImGui::Columns(2, nullptr, false);
						ImGui::Text("Speed");
						ImGui::NextColumn();
						ImGui::SliderFloat("##Speed", &component.m_Mesh->mAnimPlaySpeed, 0.1f, 10.0f);
						ImGui::EndColumns();

						ImGui::ProgressBar(component.m_Mesh->m_Animator.GetProgress(), ImVec2(0.0f, 0.0f));

						ImGui::TreePop();
					}
				}
			});

			DrawComponent<PointLightComponent>("Point Light", entity, [](auto& component)
				{
					ImGuiUtils::DrawTwoUI(
						[]() { ImGui::Text("Light Intensity"); },
						[&component = component]() { ImGui::SliderFloat("##Light Intensity", &component.Intensity, 0.0f, 10000.0f, "%.1f"); }
					);

					ImGuiUtils::DrawTwoUI(
						[]() { ImGui::Text("Light Color"); },
						[&component = component]() { ImGui::ColorEdit3("##Light Color", (float*)&component.LightColor); }
					);
				});
			DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](auto& component)
				{
					ImGuiUtils::DrawTwoUI(
						[]() { ImGui::Text("Light Intensity"); },
						[&component = component]() { ImGui::SliderFloat("##Light Intensity", &component.Intensity, 0.0f, 10.0f, "%.2f"); }
					);
				});
	}
}
