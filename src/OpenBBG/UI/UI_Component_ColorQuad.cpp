#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Component_ColorQuad.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {
	
IMPLEMENT_SINGLETON(UI_Component_ColorQuad);

UI_Component_ColorQuad::UI_Component_ColorQuad()
	: UI_Component()
#if OPENBBG_WITH_VULKAN
	, graphicsPipeline { nullptr }
#endif
{
}

UI_Component_ColorQuad::~UI_Component_ColorQuad()
{
}

UI_ComponentInstance *
UI_Component_ColorQuad::Construct()
{
	UI_ComponentInstance *compInst = new UI_ComponentInstance_ColorQuad();
	compInst->component = this;
	return compInst;
}

void
UI_Component_ColorQuad::Deconstruct(UI_ComponentInstance *compInst)
{
	delete static_cast<UI_ComponentInstance_ColorQuad *>(compInst);
}

#if OPENBBG_WITH_VULKAN
bool
sortLocalData(UI_Component_ColorQuad::LocalDataEntry &a, UI_Component_ColorQuad::LocalDataEntry &b)
{
	return (a.color.a == 1.f && b.color.a == 1.f) ? (a.hz.x > b.hz.x) : ((a.color.a == 1.f) ? true : ((b.color.a == 1.f) ? false : (a.hz.x < b.hz.x)));
}

static const char *vertShaderText =
    "#version 400\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (std140, set = 0, binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec4 inVert;\n"
    "layout (location = 1) in vec2 inPos;\n"
    "layout (location = 2) in vec2 inExt;\n"
    "layout (location = 3) in vec4 inColor;\n"
    "layout (location = 4) in vec4 inScissor;\n"
    "layout (location = 5) in vec2 inHZ;\n"
    "layout (location = 0) out vec4 outColor;\n"
    "out gl_PerVertex { \n"
    "    vec4 gl_Position;\n"
	"    float gl_ClipDistance[4];\n"
    "};\n"
    "void main() {\n"
    "   outColor = inColor;\n"
	"   vec4 pos;\n"
	"   pos.x = inPos.x + inExt.x * inVert.x;\n"
	"   pos.y = inPos.y + inExt.y * inVert.y;\n"
	"   pos.z = inHZ.x;\n"
	"   pos.w = 1.0;\n"
	"   gl_ClipDistance[0] = pos.x - inScissor.x;\n"
	"   gl_ClipDistance[1] = inScissor.x + inScissor.z - pos.x;\n"
	"   gl_ClipDistance[2] = pos.y - inScissor.y;\n"
	"   gl_ClipDistance[3] = inScissor.y + inScissor.w - pos.y;\n"
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
};

static const Vertex g_vertexData[] = {
	{XYZ1(0, 0, 0)},
	{XYZ1(1, 0, 0)},
	{XYZ1(0, 1, 0)},
	{XYZ1(0, 1, 0)},
	{XYZ1(1, 0, 0)},
	{XYZ1(1, 1, 0)},
};

void
UI_Component_ColorQuad::Init(Renderer_Vulkan *r)
{
	if (isInitialized)
		return;

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
				sizeof(g_vertexData[0]),
				VK_VERTEX_INPUT_RATE_VERTEX
			},
			{
				1,
				sizeof(LocalDataEntry),
				VK_VERTEX_INPUT_RATE_INSTANCE
			}
		},
		// vector<VkVertexInputAttributeDescription>
		{
			// Vertex Data
			{
				0,
				0,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				0
			// Instance Data
			}, {
				1,
				1,
				VK_FORMAT_R32G32_SFLOAT,
				8
			}, {
				2,
				1,
				VK_FORMAT_R32G32_SFLOAT,
				16
			}, {
				3,
				1,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				24
			}, {
				4,
				1,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				40
			}, {
				5,
				1,
				VK_FORMAT_R32G32_SFLOAT,
				56
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
			// Enable Alpha Blending
			{
				VK_TRUE,
				VK_BLEND_FACTOR_SRC_ALPHA,
				VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				VK_BLEND_OP_ADD,
				VK_BLEND_FACTOR_ONE,
				VK_BLEND_FACTOR_ZERO,
				VK_BLEND_OP_ADD,
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
			}
		},
		// vector<VkDynamicState>
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		}
	);

	graphicsPipeline->Init(r->global.device);
	pipeline = graphicsPipeline->CreatePipeline(r->global.device, r->global.pipelineCache, r->global.renderNode, 0);
	
	// Vertex Buffer
	{
		VkMemoryRequirements memReqs;
		assert(r->global.CreateBufferObject(
			sizeof(g_vertexData),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBufferObject,
			vertexBufferMemory,
			&memReqs));
		vertexBufferInfo.range = memReqs.size;
		vertexBufferInfo.offset = 0;

		uint8_t *pData;
		assert(VK_SUCCESS == vkMapMemory(r->global.device, vertexBufferMemory, 0, memReqs.size, 0, (void **)&pData));

		memcpy(pData, g_vertexData, sizeof(g_vertexData));

		vkUnmapMemory(r->global.device, vertexBufferMemory);
	}

	isInitialized = true;
}

void
UI_Component_ColorQuad::Cleanup(Renderer_Vulkan *r)
{
	if (isInitialized == false)
		return;
	
	for (auto &entry : localDataMap) {
		auto &data = entry.second;

		// Instance Buffer
		vkDestroyBuffer(r->global.device, data.localStagingBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.localStagingBufferMemory, nullptr);
		vkDestroyBuffer(r->global.device, data.localBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.localBufferMemory, nullptr);
	}
	
	// Vertex Buffer
	vkDestroyBuffer(r->global.device, vertexBufferObject, nullptr);
	vkFreeMemory(r->global.device, vertexBufferMemory, nullptr);

	graphicsPipeline->Cleanup(r->global.device);
	delete graphicsPipeline;
	graphicsPipeline = nullptr;

	isInitialized = false;
}

void
UI_Component_ColorQuad::Cleanup(Renderer_Vulkan *r, UI_Context *ctx)
{
}

void
UI_Component_ColorQuad::Cleanup(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::Prepare(Renderer_Vulkan *r, UI_Context *ctx)
{
	if (isInitialized == false)
		Init(r);

	CreateLocalData(r, ctx);

	auto &data = localDataMap[ctx];
	if (data.isLocalBufferDirty) { // TEMP: Number of Elements or Z value change
		//----------------------------------------------------------------------
		// Result Attempt
		if (data.capacity < (uint32_t)data.entries.size()) {
			vkDestroyBuffer(r->global.device, data.localStagingBufferObject, nullptr);
			vkFreeMemory(r->global.device, data.localStagingBufferMemory, nullptr);
			vkDestroyBuffer(r->global.device, data.localBufferObject, nullptr);
			vkFreeMemory(r->global.device, data.localBufferMemory, nullptr);

			data.isInitialized = false;

			CreateLocalData(r, ctx);
		}

		sort(data.entries.begin(), data.entries.end(), sortLocalData);
		sortUpdateCallbackList.reserve(data.entries.size());
		uint32_t numEntries = (uint32_t)data.entries.size();
		for (uint32_t a = 0; a < numEntries; ++a) {
			auto &entry = data.entries[a];
			if (entry.compInst->instanceIdx != a || entry.compInst->isDirty) {
				sortUpdateCallbackList.push_back(entry.compInst);
				entry.compInst->instanceIdx = a;
				entry.compInst->isDirty = false;
			}
		}
		//----------------------------------------------------------------------

		if (sortUpdateCallbackList.empty() == false) {
			VkMemoryRequirements memReqs;
			vkGetBufferMemoryRequirements(r->global.device, data.localStagingBufferObject, &memReqs);

			uint8_t *pData;
			VkResult res = vkMapMemory(r->global.device, data.localStagingBufferMemory, 0, memReqs.size, 0, (void **)&pData);
			assert(res == VK_SUCCESS);

			uint32_t start;
			uint32_t last = numeric_limits<uint32_t>::max();
			for (auto entry : sortUpdateCallbackList) {
				if (last == numeric_limits<uint32_t>::max())
					start = entry->instanceIdx;
				else if ((entry->instanceIdx - last) > 1) {
					memcpy((LocalDataEntry *)pData + start, data.entries.data() + start, (last - start + 1) * sizeof(LocalDataEntry));
					start = entry->instanceIdx;
				}
				last = entry->instanceIdx;
			}

			if (last != numeric_limits<uint32_t>::max())
				memcpy((LocalDataEntry *)pData + start, data.entries.data() + start, (last - start + 1) * sizeof(LocalDataEntry));
			
			vkUnmapMemory(r->global.device, data.localStagingBufferMemory);

			sortUpdateCallbackList.clear();

/*			{
				VkBufferMemoryBarrier bufferMemoryBarrier = {};
				bufferMemoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
				bufferMemoryBarrier.pNext = nullptr;


				bufferMemoryBarrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
				bufferMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			}*/

			// TODO: Confirm if we want to do a dynamic copy
			{
#if 1
				VkCommandBuffer commandBuffer;

				VkCommandBufferAllocateInfo allocInfo = {};
				allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocInfo.commandPool = r->global.transientCommandPool.pool;
				allocInfo.commandBufferCount = 1;
				vkAllocateCommandBuffers(r->global.device, &allocInfo, &commandBuffer);

				VkCommandBufferBeginInfo beginInfo = {};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
				beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
				vkBeginCommandBuffer(commandBuffer, &beginInfo);

				VkBufferCopy copyRegion = {};
				copyRegion.size = data.entries.size() * sizeof(LocalDataEntry);
				vkCmdCopyBuffer(commandBuffer, data.localStagingBufferObject, data.localBufferObject, 1, &copyRegion);

				vkEndCommandBuffer(commandBuffer);

				VkSubmitInfo submitInfo = {};
				submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &commandBuffer;

				vkQueueSubmit(r->global.graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
				vkQueueWaitIdle(r->global.graphicsQueues[0]);

				vkFreeCommandBuffers(r->global.device, r->global.transientCommandPool.pool, 1, &commandBuffer);
#else
				VkBufferCopy copyRegion = {};
				copyRegion.size = data.entries.size() * sizeof(LocalDataEntry);
				vkCmdCopyBuffer(r->global.primaryCommandPool.currentBuffer, data.localStagingBufferObject, data.localBufferObject, 1, &copyRegion);
#endif
			}
		}

		data.isLocalBufferDirty = false;
	}
}

void
UI_Component_ColorQuad::Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx)
{
#if 1
	{
		LocalData &data = localDataMap[ctx];
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
		vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBufferObject, offsets);
		vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 1, 1, &data.localBufferObject, offsets);
		vkCmdDraw(r->global.primaryCommandPool.currentBuffer, 4 * 3, data.numOpaque, 0, 0);
	}
#endif
}

void
UI_Component_ColorQuad::RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, vector<UI_ComponentInstance *> &instances, uint32_t startInstance, uint32_t numInstances)
{
#if 1
	{
		if (s_lastComponentRendered != this) {
			LocalData &data = localDataMap[ctx];
			const VkDeviceSize offsets[1] = { 0 };
			vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
			vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBufferObject, offsets);
			vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 1, 1, &data.localBufferObject, offsets);
		}
		vkCmdDraw(r->global.primaryCommandPool.currentBuffer, 4 * 3, numInstances, 0, instances[startInstance]->instanceIdx);
	}
#endif
}

void
UI_Component_ColorQuad::RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::PopulateTransparentInstances(Renderer_Vulkan *r, UI_Context *ctx, vector<UI_ComponentInstance *> &instances)
{
	auto &data = localDataMap[ctx];
	auto &compInsts = componentInstances[ctx];
	uint32_t numInstances = (uint32_t)compInsts.size();
	for (uint32_t a = data.numOpaque; a < numInstances; ++a)
		instances.push_back(compInsts[a]);
}

inline
void
UI_Component_ColorQuad::UploadLocalData(Renderer_Vulkan *r, LocalData &data)
{
	// Instance Data
	{
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(r->global.device, data.localStagingBufferObject, &memReqs);

		uint8_t *pData;
		VkResult res = vkMapMemory(r->global.device, data.localStagingBufferMemory, 0, memReqs.size, 0, (void **)&pData);
		assert(res == VK_SUCCESS);

		memcpy(pData, data.entries.data(), data.entries.size() * sizeof(LocalDataEntry));

		vkUnmapMemory(r->global.device, data.localStagingBufferMemory);
	}

	{
#if 1
		VkCommandBuffer commandBuffer;

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = r->global.transientCommandPool.pool;
		allocInfo.commandBufferCount = 1;
		vkAllocateCommandBuffers(r->global.device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.size = data.entries.size() * sizeof(LocalDataEntry);
		vkCmdCopyBuffer(commandBuffer, data.localStagingBufferObject, data.localBufferObject, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(r->global.graphicsQueues[0], 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(r->global.graphicsQueues[0]);

		vkFreeCommandBuffers(r->global.device, r->global.transientCommandPool.pool, 1, &commandBuffer);
#else
		VkBufferCopy copyRegion = {};
		copyRegion.size = data.entries.size() * sizeof(LocalDataEntry);
		vkCmdCopyBuffer(r->global.primaryCommandPool.currentBuffer, data.localStagingBufferObject, data.localBufferObject, 1, &copyRegion);
#endif
	}
}

inline
void
UI_Component_ColorQuad::CreateLocalData(Renderer_Vulkan *r, UI_Context *ctx)
{
	auto search = localDataMap.find(ctx);
	if (search != localDataMap.end() && search->second.isInitialized)
		return;

	LocalData &data = localDataMap[ctx];

	// Store old capacity for logging purposes
	auto oldCapacity = data.capacity;

	if (data.capacity == 0)
		data.capacity = 1;
	uint32_t numEntries = (uint32_t)data.entries.size();
	while (data.capacity < numEntries)
		data.capacity <<= 1;
	LOG_DEBUG("Reallocating buffer: {} -> {} entries...", oldCapacity, data.capacity);

	// Buffer Objects
	{
		uint32_t numBytes = data.capacity * sizeof(LocalDataEntry);

		assert(r->global.CreateBufferObject(
			numBytes,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			data.localBufferObject,
			data.localBufferMemory,
			nullptr));
		assert(r->global.CreateBufferObject(
			numBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			data.localStagingBufferObject,
			data.localStagingBufferMemory,
			nullptr));
	
		data.localBufferInfo = {
			data.localBufferObject,
			0,
			numBytes
		};
		data.localStagingBufferInfo = {
			data.localStagingBufferObject,
			0,
			numBytes
		};
	}

	data.isInitialized = true;

	{
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(r->global.device, data.localStagingBufferObject, &memReqs);

		uint8_t *pData;
		VkResult res = vkMapMemory(r->global.device, data.localStagingBufferMemory, 0, memReqs.size, 0, (void **)&pData);
		assert(res == VK_SUCCESS);

		size_t used = data.entries.size() * sizeof(LocalDataEntry);
		memcpy(pData, data.entries.data(), used);

		// Zero out rest of data
#if 0
		size_t leftover = data.capacity * sizeof(LocalDataEntry) - used;
		memset(pData + used, 0, leftover);
#endif

		vkUnmapMemory(r->global.device, data.localStagingBufferMemory);
	}

//	UploadLocalData(r, data);
}
#endif


void
UI_Component_ColorQuad::OnAddToContext(UI_ComponentInstance *compInst, UI_Context *ctx)
{
#if OPENBBG_WITH_VULKAN
	auto localCompInst = static_cast<UI_ComponentInstance_ColorQuad *>(compInst);
	LocalData &data = localDataMap[ctx];
	compInst->instanceIdx = (uint32_t)data.entries.size();
	data.isLocalBufferDirty = true;
	data.entries.push_back(LocalDataEntry());
	auto &entry = data.entries.back();
	if (localCompInst->color.a == 1.f)
		++data.numOpaque;
	ctx->isTransparentInstancesDirty = true;
	entry.compInst = compInst;
	entry.position = compInst->relativePosition;
	entry.extent = compInst->extent;
	entry.color = localCompInst->color;
	entry.scissor = { entry.position, entry.extent };
	entry.hz.x = compInst->zActual;
#endif
}

void
UI_Component_ColorQuad::OnRemoveFromContext(UI_ComponentInstance *compInst, UI_Context *ctx)
{
#if OPENBBG_WITH_VULKAN
	auto localCompInst = static_cast<UI_ComponentInstance_ColorQuad *>(compInst);
	LocalData &data = localDataMap[ctx];
	data.entries.erase(data.entries.begin() + compInst->instanceIdx);
	uint32_t numEntries = (uint32_t)data.entries.size();
	for (auto a = compInst->instanceIdx; a < numEntries; ++a)
		--data.entries[a].compInst->instanceIdx;
	if (localCompInst->color.a == 1.f)
		--data.numOpaque;
	ctx->isTransparentInstancesDirty = true;
	data.isLocalBufferDirty = true;
#endif
}

void UI_Component_ColorQuad::OnMetricsUpdate(UI_ComponentInstance *compInst)
{
	compInst->isDirty = true;
	if (compInst->control->context != nullptr) {
		auto localCompInst = static_cast<UI_ComponentInstance_ColorQuad *>(compInst);
		auto &data = localDataMap[compInst->control->context];
		auto &entry = data.entries[compInst->instanceIdx];
		entry.position = compInst->relativePosition;
		entry.color = localCompInst->color;
		entry.scissor.x = entry.position.x;
		entry.scissor.y = entry.position.y;
		data.isLocalBufferDirty = true;
	}
}

}
