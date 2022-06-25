#include "SceneHierarchyPanel.h"
#include <filesystem>
#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include "Puppet/Scene/Components.h"
#include "Puppet/Math/MathUtils.h"
#include "Puppet/Utils/PlatformUtils.h"
#include "Puppet/ImGui/ImGuiWrapper.h"
namespace Puppet {
	extern const std::filesystem::path g_AssetPath;
	glm::mat4 Mat4FromAssimpMat4(const aiMatrix4x4& matrix);

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
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Empty Entity"))
				{
					auto newEntity = m_Context->CreateEntity("Empty Entity");
					m_SelectionContext = newEntity;
				}
				if (ImGui::MenuItem("Mesh"))
				{
					auto newEntity = m_Context->CreateEntity("Mesh");
					newEntity.AddComponent<MeshComponent>();
					m_SelectionContext = newEntity;
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Directional Light"))
				{
					auto newEntity = m_Context->CreateEntity("Directional Light");
					newEntity.AddComponent<DirectionalLightComponent>();
					newEntity.GetComponent<TransformComponent>().Rotation = glm::radians(glm::vec3{ 80.0f, 10.0f, 0.0f });
					m_SelectionContext = newEntity;
				}
				if (ImGui::MenuItem("Sky Light"))
				{
					auto newEntity = m_Context->CreateEntity("Sky Light");
					newEntity.AddComponent<SkyLightComponent>();
					m_SelectionContext = newEntity;
				}
				ImGui::EndMenu();
			}
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
		ImGui::AlignTextToFramePadding();
		ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			char buffer[256];
			memset(buffer, 0, 256);
			memcpy(buffer, tag.c_str(), tag.length());
			ImGui::PushItemWidth(contentRegionAvailable.x * 0.5f);
			if (ImGui::InputText("##Tag", buffer, 256))
			{
				tag = std::string(buffer);
			}
			ImGui::PopItemWidth();
		}

		ImGui::SameLine();
		auto id = entity.GetComponent<IDComponent>().ID;
		ImGui::TextDisabled("%llx", id);
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 textSize = ImGui::CalcTextSize("Add Component");
		ImGui::SameLine(contentRegionAvailable.x - (textSize.x + GImGui->Style.FramePadding.y));
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!m_SelectionContext.HasComponent<CameraComponent>())
			{
				if (ImGui::Button("Camera"))
				{
					m_SelectionContext.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<MeshComponent>())
			{
				if (ImGui::Button("Mesh"))
				{
					m_SelectionContext.AddComponent<MeshComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<DirectionalLightComponent>())
			{
				if (ImGui::Button("Directional Light"))
				{
					m_SelectionContext.AddComponent<DirectionalLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<SkyLightComponent>())
			{
				if (ImGui::Button("Sky Light"))
				{
					m_SelectionContext.AddComponent<SkyLightComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<NativeScriptComponent>())
			{
				if (ImGui::Button("Script"))
				{
					m_SelectionContext.AddComponent<NativeScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!m_SelectionContext.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::Button("Sprite Renderer"))
				{
					m_SelectionContext.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

		DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& component)
			{
				
				bool updateTransform = false;
				updateTransform |= DrawVec3Control("Translation", component.Translation);
				glm::vec3 rotation = glm::degrees(component.Rotation);
				updateTransform |= DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);
				updateTransform |= DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		DrawComponent<CameraComponent>("Camera", entity, [](CameraComponent& component)
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
					//float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					//if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					//	camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					//float perspectiveNear = camera.GetPerspectiveNearClip();
					//if (ImGui::DragFloat("Near", &perspectiveNear))
					//	camera.SetPerspectiveNearClip(perspectiveNear);

					//float perspectiveFar = camera.GetPerspectiveFarClip();
					//if (ImGui::DragFloat("Far", &perspectiveFar))
					//	camera.SetPerspectiveFarClip(perspectiveFar);

					ImGuiUtils::BeginPropertyGrid();
					float perspectiveVerticalFov = glm::degrees(camera.GetPerspectiveVerticalFOV());
					if (ImGuiUtils::Property("Vertical FOV", perspectiveVerticalFov))
						camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.GetPerspectiveNearClip();
					if (ImGuiUtils::Property("Near Clip", perspectiveNear))
						camera.SetPerspectiveNearClip(perspectiveNear);
					float perspectiveFar = camera.GetPerspectiveFarClip();
					if (ImGuiUtils::Property("Far Clip", perspectiveFar))
						camera.SetPerspectiveFarClip(perspectiveFar);
					ImGuiUtils::EndPropertyGrid();
				}
				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					ImGuiUtils::BeginPropertyGrid();
					float orthoSize = camera.GetOrthographicSize();
					if(ImGuiUtils::Property("Size", orthoSize))
						camera.SetOrthographicSize(orthoSize);
					float orthoNear = camera.GetOrthographicNearClip();
					if(ImGuiUtils::Property("Near", orthoNear))
						camera.SetOrthographicNearClip(orthoNear);
					float orthoFar = camera.GetOrthographicFarClip();
					if(ImGuiUtils::Property("Far", orthoFar))
						camera.SetOrthographicFarClip(orthoFar);
					ImGuiUtils::Property("Fixed Aspect Ratio", component.FixedAspectRatio);
					ImGuiUtils::EndPropertyGrid();

				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
			{

				ImGui::Columns(3);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, 300);
				ImGui::SetColumnWidth(2, 40);
				ImGui::Text("Texture Path");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				if (component.Texture)
					ImGui::InputText("##Texturepath",(char*)component.Texture->GetPath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
				else
					ImGui::InputText("##Texturepath", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::NextColumn();
				if (ImGui::Button("...##openTexture"))
				{
					std::filesystem::path texturePath =FileDialogs::OpenFile("Pic (*.jpg *.png *.tga)\0");
					if (!texturePath.empty())
						component.Texture = TextureLibrary::GetInstance().FindorAdd(texturePath.stem().string(), texturePath.string());
				}
				ImGui::Columns(1);
				Ref<Texture2D> inputTexture=ImGuiUtils::PropertyImage(component.Texture);
				if (inputTexture)
					component.Texture = inputTexture;
				ImGuiUtils::BeginPropertyGrid();
				ImGuiUtils::PropertyColor4("Color", component.Color);
				ImGuiUtils::Property("Intensity", component.TilingFactor,0.1,0,100);
				ImGuiUtils::EndPropertyGrid();
			});
		DrawComponent<DirectionalLightComponent>("Directional Light", entity, [](DirectionalLightComponent& dlc)
			{
				ImGuiUtils::BeginPropertyGrid();
				ImGuiUtils::PropertyColor3("Radiance", dlc.Radiance);
				ImGuiUtils::Property("Intensity", dlc.Intensity);
				ImGuiUtils::Property("Cast Shadows", dlc.CastShadows);
				ImGuiUtils::Property("Soft Shadows", dlc.SoftShadows);
				ImGuiUtils::Property("Source Size", dlc.LightSize);
				ImGuiUtils::EndPropertyGrid();
			});
		DrawComponent<SkyLightComponent>("Sky Light", entity, [](SkyLightComponent& slc)
			{
				ImGui::Columns(3);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, 300);
				ImGui::SetColumnWidth(2, 40);
				ImGui::Text("File Path");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				if (!slc.SceneEnvironment.FilePath.empty())
					ImGui::InputText("##envfilepath", (char*)slc.SceneEnvironment.FilePath.c_str(), 256, ImGuiInputTextFlags_ReadOnly);
				else
					ImGui::InputText("##envfilepath", (char*)"Empty", 256, ImGuiInputTextFlags_ReadOnly);
				ImGui::PopItemWidth();
				ImGui::NextColumn();
				if (ImGui::Button("...##openenv"))
				{
					std::string file = FileDialogs::OpenFile("env (*.hdr)\0");
					if (!file.empty())
						slc.SceneEnvironment = Environment::Load(file);
				}
				ImGui::Columns(1);

				ImGuiUtils::BeginPropertyGrid();
				ImGuiUtils::Property("Intensity", slc.Intensity, 0.01f, 0.0f, 5.0f);
				ImGuiUtils::EndPropertyGrid();
			});
		DrawComponent<MeshComponent>("Mesh", entity, [](MeshComponent& component)
			{
				ImGui::Columns(3);
				ImGui::SetColumnWidth(0, 100);
				ImGui::SetColumnWidth(1, 300);
				ImGui::SetColumnWidth(2, 40);

				ImGui::Text("File Path");
				ImGui::NextColumn();
				ImGui::PushItemWidth(-1);
				if (component.Mesh)
					ImGui::InputText("##meshfilepath", (char*)component.Mesh->GetFilePath().c_str(), 256, ImGuiInputTextFlags_ReadOnly);
				else
					ImGui::InputText("##meshfilepath", (char*)"Null", 256, ImGuiInputTextFlags_ReadOnly);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						auto path = (const wchar_t*)payload->Data;
						std::string meshPath = (std::filesystem::path("assets") / path).string();
						component.Mesh = CreateRef<Mesh>(meshPath);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::PopItemWidth();
				ImGui::NextColumn();
				if (ImGui::Button("...##openmesh"))
				{
					std::string file = FileDialogs::OpenFile("Model (*.obj *.fbx *.dae *.gltf)\0");
					if (!file.empty())
						component.Mesh = Ref<Mesh>::Create(file);
				}
				ImGui::Columns(1);
				if (component.Mesh&&ImGuiUtils::TreeNodeExStyle2((void*)"Material", "Material"))
				{
					uint32_t matIndex = 0;

					const auto& materialNode = [&matIndex = matIndex](const char* name, Ref<MaterialInstance>& material, const std::string& textureName, void(*func)(Ref<MaterialInstance>& mat)) {
						std::string label = std::string(name) + std::to_string(matIndex);
						ImGui::PushID(label.c_str());
						Ref<Texture2D> tex = material->TryGetResource<Texture2D>(textureName);
						if (ImGui::TreeNode((void*)name, name))
						{
							Ref<Texture2D> inputTexture = ImGuiUtils::PropertyImage(tex);
							if (inputTexture)
							{
								material->Set(textureName, inputTexture);
							}
							func(material);
							ImGui::TreePop();
						}

						ImGui::PopID();
					};

					for (auto& subMesh : component.Mesh->m_Submeshes)
					{
						
						auto& materialInstance = component.Mesh->m_Materials[subMesh.MaterialIndex];
						std::string label = std::string("material") + std::to_string(matIndex);
						ImGui::PushID(label.c_str());
						if(ImGui::TreeNodeEx((void*)label.c_str(), ImGuiTreeNodeFlags_Framed, subMesh.NodeName.c_str()))
						{
							
							materialNode("Albedo", materialInstance,"u_AlbedoTexture", [](Ref<MaterialInstance>& mat) {
								ImGui::SameLine();
								glm::vec3& albedoColor = mat->Get<glm::vec3>("u_AlbedoColor");
								bool useAlbedoMap = mat->Get<float>("u_AlbedoTexToggle");
								if(ImGui::Checkbox("Use", &useAlbedoMap))
									mat->Set<float>("u_AlbedoTexToggle", useAlbedoMap ? 1.0f : 0.0f);
								ImGui::ColorEdit4("##albedo", glm::value_ptr(albedoColor));
								
								});

							materialNode("Normal", materialInstance, "u_NormalTexture", [](Ref<MaterialInstance>& mat) {
								ImGui::SameLine();
								bool useNormalMap = mat->Get<float>("u_NormalTexToggle");
								if(ImGui::Checkbox("Use", &useNormalMap))
									mat->Set<float>("u_NormalTexToggle", useNormalMap ? 1.0f : 0.0f);
								});

							materialNode("Metallic", materialInstance, "u_MetalnessTexture", [](Ref<MaterialInstance>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Metallic", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									float& metalnessValue = mat->Get<float>("u_Metalness");
									bool useMetalnessMap = mat->Get<float>("u_MetalnessTexToggle");
									if (ImGui::Checkbox("Use", &useMetalnessMap))
										mat->Set<float>("u_MetalnessTexToggle", useMetalnessMap ? 1.0f : 0.0f);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									ImGui::SliderFloat("##MetallicInput", &metalnessValue, 0.0f, 1.0f);
									ImGui::EndTable();
								}
								});

							materialNode("Roughness", materialInstance, "u_RoughnessTexture", [](Ref<MaterialInstance>& mat) {
								ImGui::SameLine();

								if (ImGui::BeginTable("Roughness", 1))
								{
									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									
									float& roughnessValue = mat->Get<float>("u_Roughness");
									bool useRoughnessMap = mat->Get<float>("u_RoughnessTexToggle");

									if (ImGui::Checkbox("Use", &useRoughnessMap))
										mat->Set<float>("u_RoughnessTexToggle", useRoughnessMap ? 1.0f : 0.0f);

									ImGui::TableNextRow();
									ImGui::TableNextColumn();
									ImGui::SliderFloat("##RoughnessInput", &roughnessValue, 0.0f, 1.0f);
									ImGui::EndTable();
								}
								});
							ImGui::TreePop();
						}

						matIndex++;

						ImGui::PopID();
					}

					ImGui::TreePop();
				}

				if (component.Mesh&&component.Mesh->m_IsAnimated)
				{
					ImGui::SliderFloat("##AnimationTime", &component.Mesh->m_AnimationTime, 0.0f, (float)component.Mesh->m_Scene->mAnimations[0]->mDuration);
					ImGui::DragFloat("Time Scale", &component.Mesh->m_TimeMultiplier, 0.05f, 0.0f, 10.0f);

					if (ImGuiUtils::TreeNodeExStyle2((void*)"Animation", "Animation"))
					{
						static std::string label = "Play";
						if (ImGui::Button(label.c_str()))
						{
							component.Mesh->m_AnimationPlaying = !component.Mesh->m_AnimationPlaying;
							if (component.Mesh->m_AnimationPlaying)
								label = "Stop";
							else
							{
								label = "Play";
								component.Mesh->m_AnimationTime = 0.0f;
							}
						}
						ImGui::Columns(2, nullptr, false);
						ImGui::Text("Speed");
						ImGui::NextColumn();
						ImGui::SliderFloat("##Speed", &component.Mesh->m_TimeMultiplier, 0.1f, 10.0f);
						ImGui::EndColumns();

						ImGui::ProgressBar(component.Mesh->GetAnimationProgress(), ImVec2(0.0f, 0.0f));

						ImGui::TreePop();
					}
				}
			});
	}
	void SceneHierarchyPanel::DrawMeshNode(const Ref<Mesh>& mesh, uint32_t& imguiMeshID)
	{
		static char imguiName[128];
		memset(imguiName, 0, 128);
		sprintf(imguiName, "Mesh##%d", imguiMeshID++);

		// Mesh Hierarchy
		if (ImGui::TreeNode(imguiName))
		{
			auto rootNode = mesh->m_Scene->mRootNode;
			MeshNodeHierarchy(mesh, rootNode);
			ImGui::TreePop();
		}
	}
	void SceneHierarchyPanel::MeshNodeHierarchy(const Ref<Mesh>& mesh, aiNode* node, const glm::mat4& parentTransform, uint32_t level)
	{
		glm::mat4 localTransform = Mat4FromAssimpMat4(node->mTransformation);
		glm::mat4 transform = parentTransform * localTransform;

		if (ImGui::TreeNode(node->mName.C_Str()))
		{
			{
				auto [translation, rotation, scale] = Math::DecomposeTransform(transform);
				ImGui::Text("World Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}
			{
				auto [translation, rotation, scale] = Math::DecomposeTransform(localTransform);
				ImGui::Text("Local Transform");
				ImGui::Text("  Translation: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z);
				ImGui::Text("  Scale: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
			}

			for (uint32_t i = 0; i < node->mNumChildren; i++)
				MeshNodeHierarchy(mesh, node->mChildren[i], transform, level + 1);

			ImGui::TreePop();
		}

	}
}
