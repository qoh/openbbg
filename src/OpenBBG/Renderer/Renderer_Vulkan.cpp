#include "stdafx.h"
#if OPENBBG_WITH_VULKAN

// OpenBBG
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#include <OpenBBG/Renderer/Utility_Vulkan.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/Window.h>
#include <OpenBBG/Log.h>

// REF: https://vulkan.lunarg.com/doc/sdk/1.0.42.1/windows/tutorial/html/index.html

// TODO: Figure out a way to instantly present to window without waiting for the first frame to queue

namespace openbbg {

deque<vk::GraphicsPipeline *> vk::GraphicsPipeline::s_pipelines;

static const char *vertShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec4 pos;\n"
    "layout (location = 1) in vec4 inColor;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "out gl_PerVertex { \n"
    "    vec4 gl_Position;\n"
    "};\n"
    "void main() {\n"
    "   outColor = inColor;\n"
    "   gl_Position = myBufferVals.mvp * pos;\n"
    "}\n";

static const char *fragShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (location = 0) in vec4 color;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "   outColor = color;\n"
    "}\n";

struct Vertex {
    float posX, posY, posZ, posW;  // Position data
    float r, g, b, a;              // Color
};

struct VertexUV {
    float posX, posY, posZ, posW;  // Position data
    float u, v;                    // texture u,v
};

#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f
#define UV(_u_, _v_) (_u_), (_v_)

static const Vertex g_vbData[] = {
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)}, {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)}, {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},  {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)}, {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},

    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},  {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},  {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},   {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},  {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},

    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},  {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},   {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},  {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},

    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},   {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)}, {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},  {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)}, {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)},

    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},  {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},   {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},  {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},

    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},   {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)}, {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},  {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)}, {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)},
};

static const Vertex g_vb_solid_face_colors_Data[] = {
    // red face
    {XYZ1(-1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 0.f)},
    // green face
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    // blue face
    {XYZ1(-1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 1.f)},
    // yellow face
    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 1.f, 0.f)},
    // magenta face
    {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    // cyan face
    {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
};

static const VertexUV g_vb_texture_Data[] = {
    // left face
    {XYZ1(-1, -1, -1), UV(1.f, 0.f)},  // lft-top-front
    {XYZ1(-1, 1, 1), UV(0.f, 1.f)},    // lft-btm-back
    {XYZ1(-1, -1, 1), UV(0.f, 0.f)},   // lft-top-back
    {XYZ1(-1, 1, 1), UV(0.f, 1.f)},    // lft-btm-back
    {XYZ1(-1, -1, -1), UV(1.f, 0.f)},  // lft-top-front
    {XYZ1(-1, 1, -1), UV(1.f, 1.f)},   // lft-btm-front
    // front face
    {XYZ1(-1, -1, -1), UV(0.f, 0.f)},  // lft-top-front
    {XYZ1(1, -1, -1), UV(1.f, 0.f)},   // rgt-top-front
    {XYZ1(1, 1, -1), UV(1.f, 1.f)},    // rgt-btm-front
    {XYZ1(-1, -1, -1), UV(0.f, 0.f)},  // lft-top-front
    {XYZ1(1, 1, -1), UV(1.f, 1.f)},    // rgt-btm-front
    {XYZ1(-1, 1, -1), UV(0.f, 1.f)},   // lft-btm-front
    // top face
    {XYZ1(-1, -1, -1), UV(0.f, 1.f)},  // lft-top-front
    {XYZ1(1, -1, 1), UV(1.f, 0.f)},    // rgt-top-back
    {XYZ1(1, -1, -1), UV(1.f, 1.f)},   // rgt-top-front
    {XYZ1(-1, -1, -1), UV(0.f, 1.f)},  // lft-top-front
    {XYZ1(-1, -1, 1), UV(0.f, 0.f)},   // lft-top-back
    {XYZ1(1, -1, 1), UV(1.f, 0.f)},    // rgt-top-back
    // bottom face
    {XYZ1(-1, 1, -1), UV(0.f, 0.f)},  // lft-btm-front
    {XYZ1(1, 1, 1), UV(1.f, 1.f)},    // rgt-btm-back
    {XYZ1(-1, 1, 1), UV(0.f, 1.f)},   // lft-btm-back
    {XYZ1(-1, 1, -1), UV(0.f, 0.f)},  // lft-btm-front
    {XYZ1(1, 1, -1), UV(1.f, 0.f)},   // rgt-btm-front
    {XYZ1(1, 1, 1), UV(1.f, 1.f)},    // rgt-btm-back
    // right face
    {XYZ1(1, 1, -1), UV(0.f, 1.f)},   // rgt-btm-front
    {XYZ1(1, -1, 1), UV(1.f, 0.f)},   // rgt-top-back
    {XYZ1(1, 1, 1), UV(1.f, 1.f)},    // rgt-btm-back
    {XYZ1(1, -1, 1), UV(1.f, 0.f)},   // rgt-top-back
    {XYZ1(1, 1, -1), UV(0.f, 1.f)},   // rgt-btm-front
    {XYZ1(1, -1, -1), UV(0.f, 0.f)},  // rgt-top-front
    // back face
    {XYZ1(-1, 1, 1), UV(1.f, 1.f)},   // lft-btm-back
    {XYZ1(1, 1, 1), UV(0.f, 1.f)},    // rgt-btm-back
    {XYZ1(-1, -1, 1), UV(1.f, 0.f)},  // lft-top-back
    {XYZ1(-1, -1, 1), UV(1.f, 0.f)},  // lft-top-back
    {XYZ1(1, 1, 1), UV(0.f, 1.f)},    // rgt-btm-back
    {XYZ1(1, -1, 1), UV(0.f, 0.f)},   // rgt-top-back
};

//-----------------------------------------------

Renderer_Vulkan::Renderer_Vulkan(Window *window)
	: window { window }
	, isInitialized { false }
	, isFirstFrame { true }
#if !OPENBBG_VULKAN_VSYNC
	, frameCPULog { 1000.f }
	, frameGPULog { 1000.f }
#endif
{
	info = {};
	Init();
}

Renderer_Vulkan::~Renderer_Vulkan()
{
	Destroy();
}

VkSemaphore imageAcquiredSemaphore;
VkFence drawFence;
vk::GraphicsPipeline *graphicsPipeline = nullptr;
void Renderer_Vulkan::Init()
{
	if (isInitialized)
		return;

	const char *appName = "opennbg";
    const bool depthPresent = true;

	// Init global instance
	global.width = window->framebufferSize.x;
	global.height = window->framebufferSize.y;
	global.Init(appName, window->glfwWindow);

	//-----------------------------

	graphicsPipeline = new vk::GraphicsPipeline(
		// vector<VkPipelineShaderStageCreateInfo>
		{
			{
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_VERTEX_BIT,
				nullptr,
				"main",
				nullptr
			}, {
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				VK_SHADER_STAGE_FRAGMENT_BIT,
				nullptr,
				"main",
				nullptr
			}
		},
		// vector<const char *> glslSources
		{
			vertShaderText,
			fragShaderText
		},
		// vector<vector<VkDescriptorSetLayoutBinding>>
		{
			{
				{
					0,
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					1,
					VK_SHADER_STAGE_VERTEX_BIT,
					nullptr
				}
			}
		},
		// vector<VkVertexInputBindingDescription>
		{
			{
				0,
				sizeof(g_vb_solid_face_colors_Data[0]),
				VK_VERTEX_INPUT_RATE_VERTEX
			}
		},
		// vector<VkVertexInputAttributeDescription>
		{
			{
				0,
				0,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				0
			}, {
				1,
				0,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				16
			}
		},
		// VkPipelineInputAssemblyStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			VK_FALSE
		},
		// VkPipelineTessellationStateCreateInfo
		{
		},
		// VkPipelineViewportStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			nullptr,
			0,
			1,
			nullptr,
			1,
			nullptr
		},
		// VkPipelineRasterizationStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_FALSE,
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
			VK_FALSE,
			0.f,
			0.f,
			0.f,
			1.f
		},
		// VkPipelineMultisampleStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			nullptr,
			0,
			NUM_SAMPLES,
			VK_FALSE,
			0.f,
			nullptr,
			VK_FALSE,
			VK_FALSE
		},
		// VkPipelineDepthStencilStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_TRUE,
			VK_TRUE,
			VK_COMPARE_OP_LESS_OR_EQUAL,
			VK_FALSE,
			VK_FALSE,
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_ALWAYS,
				0,
				0,
				0
			},
			{
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_STENCIL_OP_KEEP,
				VK_COMPARE_OP_ALWAYS,
				0,
				0,
				0
			},
			0.f,
			0.f
		},
		// VkPipelineColorBlendStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			nullptr,
			0,
			VK_FALSE,
			VK_LOGIC_OP_NO_OP,
			0,
			nullptr,
			{ 1.f, 1.f, 1.f, 1.f }
		},
		// vector<VkPipelineColorBlendAttachmentState>
		{
			{
				VK_FALSE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				0xf
			}
		},
		// vector<VkDynamicState>
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		}
	);

	graphicsPipeline->Init(global.device);
	global.testPipeline = graphicsPipeline->CreatePipeline(global.device, global.pipelineCache, global.renderNode, 0);






	//--------------------------

	init_uniform_buffer(global, info);
	init_vertex_buffer(global, info, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data),
					   sizeof(g_vb_solid_face_colors_Data[0]), false);
	init_descriptor_pool(global, info, false);
	init_descriptor_set(global, info, graphicsPipeline, false);
	
	VkResult res;

	VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
	imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	imageAcquiredSemaphoreCreateInfo.pNext = NULL;
	imageAcquiredSemaphoreCreateInfo.flags = 0;

	res = vkCreateSemaphore(global.device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
	assert(res == VK_SUCCESS);
	
	VkFenceCreateInfo fenceInfo;
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.pNext = NULL;
	fenceInfo.flags = 0;
	vkCreateFence(global.device, &fenceInfo, NULL, &drawFence);

	isInitialized = true;
	isFirstFrame = true;
}

void Renderer_Vulkan::ResizeFramebuffer(int x, int y)
{
	if ((global.width == x && global.height == y) || x == 0 || y == 0)
		return;
	global.width = x;
	global.height = y;
	global.renderNode->DestroyFramebuffers(global.device);
	global.renderNode->DestroyImagesAndViews(global.device);
	global.renderNode->DestroySwapchainViews(global.device);
	global.renderNode->SetResolution(x, y);
	global.DestroySwapChain();
	global.CreateSwapChain();
	global.renderNode->CreateSwapchainViews(global.device, global.swapchain);
	global.renderNode->CreateImagesAndViews(global.device, global.physicalDevices[0], global.deviceMemoryProperties);
	global.renderNode->CreateFramebuffers(global.device);

	// Uniform Buffer & Descriptor Set
	vkDestroyBuffer(global.device, info.uniform_data.buf, NULL);
	vkFreeMemory(global.device, info.uniform_data.mem, NULL);
	vkFreeDescriptorSets(global.device, info.desc_pool, (uint32_t)info.desc_set.size(), info.desc_set.data());
	init_uniform_buffer(global, info);
	init_descriptor_set(global, info, graphicsPipeline, false);
}

void Renderer_Vulkan::Render()
{
	// TEMP - Frame Time Logging (No VSync)
#if !OPENBBG_VULKAN_VSYNC
	if (isFirstFrame == false) {
		GetTime(frameEnd);
		frameCPULog.Push(GetTimeDurationMS(frameStart, frameQueue));
		if (frameGPULog.Push(GetTimeDurationMS(frameQueue, frameEnd))) {
			float frameTime = frameCPULog.average + frameGPULog.average;
			LOG_DEBUG("|  fps: {:12.3f}  |  mspf: {:6.3f}  |  cpu: {:6.3f}  |  gpu: {:6.3f}  |", 1000.f / frameTime, frameTime, frameCPULog.average, frameGPULog.average);
		}
	}
	GetTime(frameStart);
#endif

	VkResult res;

	// Build Command Buffer
	global.primaryCommandPool.BeginCurrentBuffer();

	Game::Get()->jobsFrameStart.ProcessAllCurrent();

	{
		VkClearValue clear_values[2];
		clear_values[0].color.float32[0] = fmod((float)glfwGetTime(), 1.f);
//		clear_values[0].color.float32[0] = 0.2f;
		clear_values[0].color.float32[1] = 0.2f;
		clear_values[0].color.float32[2] = 0.2f;
		clear_values[0].color.float32[3] = 0.2f;
		clear_values[1].depthStencil.depth = 1.0f;
		clear_values[1].depthStencil.stencil = 0;

#if 0
		VkSemaphore imageAcquiredSemaphore;
		VkSemaphoreCreateInfo imageAcquiredSemaphoreCreateInfo;
		imageAcquiredSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		imageAcquiredSemaphoreCreateInfo.pNext = NULL;
		imageAcquiredSemaphoreCreateInfo.flags = 0;

		res = vkCreateSemaphore(global.device, &imageAcquiredSemaphoreCreateInfo, NULL, &imageAcquiredSemaphore);
		assert(res == VK_SUCCESS);
#endif

		// Get the index of the next available swapchain image:
		res = vkAcquireNextImageKHR(global.device, global.swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE,
									&global.currentSwapchainBufferIdx);
		// TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR
		// return codes
		assert(res == VK_SUCCESS);

		VkRenderPassBeginInfo rp_begin;
		rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rp_begin.pNext = NULL;
		rp_begin.renderPass = global.renderNode->renderPass;
		rp_begin.framebuffer = global.renderNode->framebuffers[global.currentSwapchainBufferIdx];
		rp_begin.renderArea.offset.x = 0;
		rp_begin.renderArea.offset.y = 0;
		rp_begin.renderArea.extent.width = global.width;
		rp_begin.renderArea.extent.height = global.height;
		rp_begin.clearValueCount = 2;
		rp_begin.pClearValues = clear_values;

		vkCmdBeginRenderPass(global.primaryCommandPool.currentBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, global.testPipeline);
		vkCmdBindDescriptorSets(global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, NUM_DESCRIPTOR_SETS,
								info.desc_set.data(), 0, NULL);

		const VkDeviceSize offsets[1] = {0};
		vkCmdBindVertexBuffers(global.primaryCommandPool.currentBuffer, 0, 1, &info.vertex_buffer.buf, offsets);

		init_viewports(global, info);
		init_scissors(global, info);

		vkCmdDraw(global.primaryCommandPool.currentBuffer, 12 * 3, 1, 0, 0);
		vkCmdEndRenderPass(global.primaryCommandPool.currentBuffer);

	}

	Game::Get()->jobsFrameEnd.ProcessAllCurrent();

	global.primaryCommandPool.EndCurrentBuffer();

	// Queue commands
	{
		const VkCommandBuffer cmd_bufs[] = {global.primaryCommandPool.currentBuffer};
#if 0
		VkFenceCreateInfo fenceInfo;
		VkFence drawFence;
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.pNext = NULL;
		fenceInfo.flags = 0;
		vkCreateFence(global.device, &fenceInfo, NULL, &drawFence);
#endif

		VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit_info[1] = {};
		submit_info[0].pNext = NULL;
		submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info[0].waitSemaphoreCount = 1;
		submit_info[0].pWaitSemaphores = &imageAcquiredSemaphore;
		submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
		submit_info[0].commandBufferCount = 1;
		submit_info[0].pCommandBuffers = cmd_bufs;
		submit_info[0].signalSemaphoreCount = 0;
		submit_info[0].pSignalSemaphores = NULL;
		
		// TEMP - Frame Time Logging (No VSync)
#if !OPENBBG_VULKAN_VSYNC
		GetTime(frameQueue);
#endif

		/* Queue the command buffer for execution */
		res = vkQueueSubmit(global.graphicsQueues[0], 1, submit_info, drawFence);
		assert(res == VK_SUCCESS);

		/* Now present the image in the window */

		VkPresentInfoKHR present;
		present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present.pNext = NULL;
		present.swapchainCount = 1;
		present.pSwapchains = &global.swapchain;
		present.pImageIndices = &global.currentSwapchainBufferIdx;
		present.pWaitSemaphores = NULL;
		present.waitSemaphoreCount = 0;
		present.pResults = NULL;

		do {
			res = vkWaitForFences(global.device, 1, &drawFence, VK_TRUE, FENCE_TIMEOUT);
		} while (res == VK_TIMEOUT);

		assert(res == VK_SUCCESS);
		res = vkQueuePresentKHR(global.presentQueue, &present);
		assert(res == VK_SUCCESS);

//		std::this_thread::sleep_for(std::chrono::seconds(1));
		/* VULKAN_KEY_END */
#if 0
		vkDestroySemaphore(global.device, imageAcquiredSemaphore, NULL);
		vkDestroyFence(global.device, drawFence, NULL);
#endif
	}

	global.primaryCommandPool.UpdateCurrentBuffer();

	// Show window after first frame is queued
	if (isFirstFrame && glfwGetWindowAttrib(window->glfwWindow, GLFW_VISIBLE) == GLFW_FALSE) {
		glfwShowWindow(window->glfwWindow);
		isFirstFrame = false;
	}
}

void Renderer_Vulkan::Destroy()
{
	if (isInitialized == false)
		return;

	vkDestroySemaphore(global.device, imageAcquiredSemaphore, NULL);
	vkDestroyFence(global.device, drawFence, NULL);

	delete graphicsPipeline;
	graphicsPipeline = nullptr;

	vkFreeDescriptorSets(global.device, info.desc_pool, (uint32_t)info.desc_set.size(), info.desc_set.data());

	// Descriptor Pool
	vkDestroyDescriptorPool(global.device, info.desc_pool, NULL);

	// Vertex Buffer
	vkDestroyBuffer(global.device, info.vertex_buffer.buf, NULL);
	vkFreeMemory(global.device, info.vertex_buffer.mem, NULL);

	// Uniform Buffer
	vkDestroyBuffer(global.device, info.uniform_data.buf, NULL);
	vkFreeMemory(global.device, info.uniform_data.mem, NULL);


	//----------------------

	// Cleanpu global instance
	global.Cleanup();

	isInitialized = false;
}

}
#endif