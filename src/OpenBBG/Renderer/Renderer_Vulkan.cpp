#include "stdafx.h"
#if OPENBBG_WITH_VULKAN

// OpenBBG
#include <OpenBBG/Renderer/Renderer_Vulkan.h>
#include <OpenBBG/Renderer/Utility_Vulkan.h>
#include <OpenBBG/Game.h>
#include <OpenBBG/Window.h>
#include <OpenBBG/Log.h>
#include <OpenBBG/Renderer/RenderNode.h>
#include <OpenBBG/UI/UI_Context.h>

// REF: https://vulkan.lunarg.com/doc/sdk/1.0.42.1/windows/tutorial/html/index.html

// TODO: Figure out a way to instantly present to window without waiting for the first frame to queue

namespace openbbg {

deque<vk::GraphicsPipeline *> vk::GraphicsPipeline::s_pipelines;

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
void Renderer_Vulkan::Init()
{
	if (isInitialized)
		return;

	const char *appName = "openbbg";

	// Init global instance
	global.width = window->framebufferSize.x;
	global.height = window->framebufferSize.y;
	global.Init(appName, window->glfwWindow);

	//-----------------------------




	//--------------------------

	init_vertex_buffer(global, info, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data),
					   sizeof(g_vb_solid_face_colors_Data[0]), false);
	init_descriptor_pool(global, info, false);
	
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
	
	global.UpdateMVP();
	global.UploadGlobalParamsBuffer();
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

	if (g_masterContext != nullptr)
		g_masterContext->Prepare(this);

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

		// TODO: Encapsulate all of the following into the render graph generation

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

		init_viewports(global, info);
		init_scissors(global, info);

		// TODO: Render current UI context
		if (g_masterContext != nullptr)
			g_masterContext->Render(this);


		//-----------------------------------------------

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

	// Descriptor Pool
	vkDestroyDescriptorPool(global.device, info.desc_pool, NULL);

	// Vertex Buffer
	vkDestroyBuffer(global.device, info.vertex_buffer.buf, NULL);
	vkFreeMemory(global.device, info.vertex_buffer.mem, NULL);

	//----------------------

	// Cleanpu global instance
	global.Cleanup();

	isInitialized = false;
}

}
#endif