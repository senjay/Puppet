![1](.\Screenshot\2.png)


![2](.\Screenshot\4.png)





### 一、Render Pipeline

#### 1.Render Pass:

- Input: Framebuffer spceification
- Pass: shadow、geo、post processing......
- output: framebuffer

#### 2. Scene renderer:

- Init:

  初始化各个render Pass

- BeginScene:

  设置场景参数:light、camera...

- Submit:

  提交模型，放入drawCommand

- EndScene:

  Flush drawCommand

  1. 处理各个RenderPass(有些可并行)

  2. 合成最终的framebuffer

#### 3. Render Command:

​        将RenderAPI封装成与平台无关的最小RenderCommand,Render时只提交Command到queue,执行渲染时从queue取出各个commad执行



### 二、dependencies



- **Math:** glm

- **OpenGL:** glad、glfw

- **UI:** imgui、ImGuizmo

- **SceneSerializer:** yaml-cpp

- **Image:** stb_image

- **ECS:** entt

- **Cross Platfrom Shader:** VulkanSDK(spir-v)




### 三、build

- premake5-alpha16

