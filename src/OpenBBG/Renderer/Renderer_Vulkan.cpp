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

Renderer_Vulkan::Renderer_Vulkan(Window *window)
	: window { window }
	, isInitialized { false }
	, isFirstFrame { true }
	, frameCPULog { 1000.f }
	, frameGPULog { 1000.f }
{
	Init();
}

Renderer_Vulkan::~Renderer_Vulkan()
{
	Destroy();
}

void Renderer_Vulkan::Init()
{
	if (isInitialized)
		return;

	const char *appName = "openbbg";

	// Init global instance
	global.width = window->framebufferSize.x;
	global.height = window->framebufferSize.y;
	global.Init(appName, window->glfwWindow);

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
	// TEMP - Frame Time Logging
	if (isFirstFrame == false) {
		GetTime(frameEnd);
		frameCPULog.Push(GetTimeDurationMS(frameStart, frameQueue));
		if (frameGPULog.Push(GetTimeDurationMS(frameQueue, frameEnd))) {
			float frameTime = frameCPULog.average + frameGPULog.average;
			LOG_DEBUG("|  fps: {:12.3f}  |  mspf: {:6.3f}  |  cpu: {:6.3f}  |  gpu: {:6.3f}  |", 1000.f / frameTime, frameTime, frameCPULog.average, frameGPULog.average);
		}
	}
	GetTime(frameStart);

	VkResult res;

	// Build Command Buffer
	global.primaryCommandPool.BeginCurrentBuffer();

	Game::Get()->jobsFrameStart.ProcessAllCurrent();

	if (g_masterContext != nullptr)
		g_masterContext->Prepare(this);

	{
		VkClearValue clear_values[2];
//		clear_values[0].color.float32[0] = fmod((float)glfwGetTime(), 1.f);
		clear_values[0].color.float32[0] = 0.2f;
		clear_values[0].color.float32[1] = 0.2f;
		clear_values[0].color.float32[2] = 0.2f;
		clear_values[0].color.float32[3] = 0.2f;
		clear_values[1].depthStencil.depth = 1.0f;
		clear_values[1].depthStencil.stencil = 0;

		// Get the index of the next available swapchain image:
		res = vkAcquireNextImageKHR(global.device, global.swapchain, UINT64_MAX, global.imageAcquiredSemaphore, VK_NULL_HANDLE, &global.currentSwapchainBufferIdx);
		// TODO: Deal with the VK_SUBOPTIMAL_KHR and VK_ERROR_OUT_OF_DATE_KHR return codes
		assert(res == VK_SUCCESS);

		// TODO: Encapsulate all of the following into the render graph generation

		VkRenderPassBeginInfo rp_begin;
		rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		rp_begin.pNext = nullptr;
		rp_begin.renderPass = global.renderNode->renderPass;
		rp_begin.framebuffer = global.renderNode->framebuffers[global.currentSwapchainBufferIdx];
		rp_begin.renderArea.offset.x = 0;
		rp_begin.renderArea.offset.y = 0;
		rp_begin.renderArea.extent.width = global.width;
		rp_begin.renderArea.extent.height = global.height;
		rp_begin.clearValueCount = 2;
		rp_begin.pClearValues = clear_values;

		vkCmdBeginRenderPass(global.primaryCommandPool.currentBuffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);
		
		global.SetViewport(0.f, 0.f, (float)global.width, (float)global.height);
		global.SetScissor(0, 0, global.width, global.height);

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
		const VkCommandBuffer cmd_bufs[] = { global.primaryCommandPool.currentBuffer };

		VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		VkSubmitInfo submit_info[1] = {};
		submit_info[0].pNext = nullptr;
		submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info[0].waitSemaphoreCount = 1;
		submit_info[0].pWaitSemaphores = &global.imageAcquiredSemaphore;
		submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
		submit_info[0].commandBufferCount = 1;
		submit_info[0].pCommandBuffers = cmd_bufs;
		submit_info[0].signalSemaphoreCount = 0;
		submit_info[0].pSignalSemaphores = nullptr;
		
		// TEMP - Frame Time Logging
		GetTime(frameQueue);

		/* Queue the command buffer for execution */
		res = vkQueueSubmit(global.graphicsQueues[0], 1, submit_info, global.drawFence);
		assert(res == VK_SUCCESS);

		/* Now present the image in the window */

		VkPresentInfoKHR present;
		present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present.pNext = nullptr;
		present.swapchainCount = 1;
		present.pSwapchains = &global.swapchain;
		present.pImageIndices = &global.currentSwapchainBufferIdx;
		present.pWaitSemaphores = nullptr;
		present.waitSemaphoreCount = 0;
		present.pResults = nullptr;

		do {
			res = vkWaitForFences(global.device, 1, &global.drawFence, VK_TRUE, FENCE_TIMEOUT);
		} while (res == VK_TIMEOUT);

		assert(res == VK_SUCCESS);
		res = vkQueuePresentKHR(global.presentQueue, &present);
		assert(res == VK_SUCCESS);
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
	
	VkResult res;

	// Present empty frame
	{
		// Build Command Buffer
		global.primaryCommandPool.BeginCurrentBuffer();

		Game::Get()->jobsFrameStart.ProcessAllCurrent();

		assert(VK_SUCCESS == vkAcquireNextImageKHR(global.device, global.swapchain, UINT64_MAX, global.imageAcquiredSemaphore, VK_NULL_HANDLE, &global.currentSwapchainBufferIdx));

		Game::Get()->jobsFrameEnd.ProcessAllCurrent();

		global.primaryCommandPool.EndCurrentBuffer();

		// Queue commands
		{
			const VkCommandBuffer cmd_bufs[] = { global.primaryCommandPool.currentBuffer };

			VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo submit_info[1] = {};
			submit_info[0].pNext = nullptr;
			submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info[0].waitSemaphoreCount = 1;
			submit_info[0].pWaitSemaphores = &global.imageAcquiredSemaphore;
			submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
			submit_info[0].commandBufferCount = 1;
			submit_info[0].pCommandBuffers = cmd_bufs;
			submit_info[0].signalSemaphoreCount = 0;
			submit_info[0].pSignalSemaphores = nullptr;

			/* Queue the command buffer for execution */
			res = vkQueueSubmit(global.graphicsQueues[0], 1, submit_info, global.drawFence);
			assert(res == VK_SUCCESS);

			/* Now present the image in the window */

			VkPresentInfoKHR present;
			present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present.pNext = nullptr;
			present.swapchainCount = 1;
			present.pSwapchains = &global.swapchain;
			present.pImageIndices = &global.currentSwapchainBufferIdx;
			present.pWaitSemaphores = nullptr;
			present.waitSemaphoreCount = 0;
			present.pResults = nullptr;

			do {
				res = vkWaitForFences(global.device, 1, &global.drawFence, VK_TRUE, FENCE_TIMEOUT);
			} while (res == VK_TIMEOUT);

			assert(res == VK_SUCCESS);
			res = vkQueuePresentKHR(global.presentQueue, &present);
			assert(res == VK_SUCCESS);
		}
	}




	// Build Command Buffer
	global.primaryCommandPool.BeginCurrentBuffer();

	if (g_masterContext != nullptr)
		g_masterContext->Cleanup(this);
	UI_Component::CleanupAll(this);

	global.primaryCommandPool.EndCurrentBuffer();

	// Queue commands
	{
		const VkCommandBuffer cmd_bufs[] = { global.primaryCommandPool.currentBuffer };

		VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		VkSubmitInfo submit_info[1] = {};
		submit_info[0].pNext = nullptr;
		submit_info[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info[0].pWaitDstStageMask = &pipe_stage_flags;
		submit_info[0].commandBufferCount = 1;
		submit_info[0].pCommandBuffers = cmd_bufs;

		/* Queue the command buffer for execution */
		res = vkQueueSubmit(global.graphicsQueues[0], 1, submit_info, global.drawFence);
		assert(res == VK_SUCCESS);

		do {
			res = vkWaitForFences(global.device, 1, &global.drawFence, VK_TRUE, FENCE_TIMEOUT);
		} while (res == VK_TIMEOUT);
	}

	// Cleanup global instance
	global.Cleanup();

	isInitialized = false;
}

}
#endif