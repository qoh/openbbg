#include "stdafx.h"

// OpenBBG
#include <OpenBBG/UI/UI_Class_ColorCtrl.h>
#include <OpenBBG/UI/UI_Control.h>

namespace openbbg {

IMPLEMENT_SINGLETON(UI_Class_ColorCtrl);

UI_Class_ColorCtrl::UI_Class_ColorCtrl()
	: UI_Class()
	, graphicsPipeline { nullptr }
{
}

UI_Class_ColorCtrl::~UI_Class_ColorCtrl()
{
}

UI_Control *
UI_Class_ColorCtrl::Construct()
{
	auto ctrl = new UI_Control();
	ctrl->uiClass = this;
	return ctrl;
}

#if OPENBBG_WITH_VULKAN
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
UI_Class_ColorCtrl::Init(Renderer_Vulkan *r)
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
				0
			}, {
				2,
				1,
				VK_FORMAT_R32G32_SFLOAT,
				8
			}, {
				3,
				1,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				16
			}, {
				4,
				1,
				VK_FORMAT_R32G32B32A32_SFLOAT,
				32
			}, {
				5,
				1,
				VK_FORMAT_R32G32_SFLOAT,
				48
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

	entries.resize(1024 * 1);
	for (auto &entry : entries) {
		entry.position = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2048.f, static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1024.f };
		entry.extent = { 16.f, 16.f };
		entry.color = { static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 1.f };
		entry.scissor = { entry.position.x, entry.position.y, entry.extent.x, entry.extent.y };
		entry.hz = { -static_cast<float>(rand()) / static_cast<float>(RAND_MAX), 0.1f };
	}

	isInitialized = true;
}

void
UI_Class_ColorCtrl::Cleanup(Renderer_Vulkan *r)
{
	if (isInitialized == false)
		return;
	
	for (auto &entry : localDataMap) {
		auto &data = entry.second;

		// Indirect Command
		vkDestroyBuffer(r->global.device, data.indirectStagingBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.indirectStagingBufferMemory, nullptr);
		vkDestroyBuffer(r->global.device, data.indirectBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.indirectBufferMemory, nullptr);

		// Instance Buffer
		vkDestroyBuffer(r->global.device, data.localStagingBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.localStagingBufferMemory, nullptr);
		vkDestroyBuffer(r->global.device, data.localBufferObject, nullptr);
		vkFreeMemory(r->global.device, data.localBufferMemory, nullptr);
	}
	
	// Vertex Buffer
	vkDestroyBuffer(r->global.device, vertexBufferObject, nullptr);
	vkFreeMemory(r->global.device, vertexBufferMemory, nullptr);

	vkDestroyPipeline(r->global.device, pipeline, nullptr);

	delete graphicsPipeline;
	graphicsPipeline = nullptr;

	isInitialized = false;
}

void
UI_Class_ColorCtrl::Cleanup(Renderer_Vulkan *r, UI_Context *ctx)
{
}

void
UI_Class_ColorCtrl::Cleanup(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_ColorCtrl::Prepare(Renderer_Vulkan *r, UI_Context *ctx)
{
	if (isInitialized == false)
		Init(r);

	CreateLocalData(r, ctx);
}

void
UI_Class_ColorCtrl::Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_ColorCtrl::RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx)
{
	LocalData &data = localDataMap[ctx];

	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
	vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBufferObject, offsets);
	vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 1, 1, &data.localBufferObject, offsets);
	vkCmdDrawIndirect(r->global.primaryCommandPool.currentBuffer, data.indirectBufferObject, 0, 1, sizeof(VkDrawIndirectCommand));
}

void
UI_Class_ColorCtrl::RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_ColorCtrl::RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

inline
void
UI_Class_ColorCtrl::UploadLocalData(Renderer_Vulkan *r, LocalData &data)
{
	// Instance Data
	{
		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(r->global.device, data.localStagingBufferObject, &memReqs);

		uint8_t *pData;
		VkResult res = vkMapMemory(r->global.device, data.localStagingBufferMemory, 0, memReqs.size, 0, (void **)&pData);
		assert(res == VK_SUCCESS);

		memcpy(pData, entries.data(), entries.size() * sizeof(LocalDataEntry));

		vkUnmapMemory(r->global.device, data.localStagingBufferMemory);
	}
	
	// Indirect Command
	{
		data.indirectCommand.firstInstance = 0;
		data.indirectCommand.instanceCount = (uint32_t)entries.size();
		data.indirectCommand.firstVertex = 0;
		data.indirectCommand.vertexCount = 4 * 3;

		VkMemoryRequirements memReqs;
		vkGetBufferMemoryRequirements(r->global.device, data.indirectStagingBufferObject, &memReqs);

		uint8_t *pData;
		VkResult res = vkMapMemory(r->global.device, data.indirectStagingBufferMemory, 0, memReqs.size, 0, (void **)&pData);
		assert(res == VK_SUCCESS);

		memcpy(pData, &data.indirectCommand, sizeof(VkDrawIndirectCommand));

		vkUnmapMemory(r->global.device, data.indirectStagingBufferMemory);
	}

	//--------------------------------

	{
		VkBufferCopy copyRegion = {};
		copyRegion.size = entries.size() * sizeof(LocalDataEntry);
		vkCmdCopyBuffer(r->global.primaryCommandPool.currentBuffer, data.localStagingBufferObject, data.localBufferObject, 1, &copyRegion);
	}

	{
		VkBufferCopy copyRegion = {};
		copyRegion.size = sizeof(VkDrawIndirectCommand);
		vkCmdCopyBuffer(r->global.primaryCommandPool.currentBuffer, data.indirectStagingBufferObject, data.indirectBufferObject, 1, &copyRegion);
	}


}

inline
void
UI_Class_ColorCtrl::CreateLocalData(Renderer_Vulkan *r, UI_Context *ctx)
{
	if (localDataMap.find(ctx) != localDataMap.end())
		return;

//	VkResult res;

	LocalData &data = localDataMap[ctx];

	// Buffer Objects
	{
		assert(r->global.CreateBufferObject(
			sizeof(VkDrawIndirectCommand),
			VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			data.indirectBufferObject,
			data.indirectBufferMemory,
			nullptr));
		assert(r->global.CreateBufferObject(
			sizeof(VkDrawIndirectCommand),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			data.indirectStagingBufferObject,
			data.indirectStagingBufferMemory,
			nullptr));
	
		data.indirectBufferInfo = {
			data.indirectBufferObject,
			0,
			sizeof(VkDrawIndirectCommand)
		};
		data.indirectStagingBufferInfo = {
			data.indirectStagingBufferObject,
			0,
			sizeof(VkDrawIndirectCommand)
		};

		assert(r->global.CreateBufferObject(
			(uint32_t)(sizeof(LocalDataEntry) * entries.size()),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			data.localBufferObject,
			data.localBufferMemory,
			nullptr));
		assert(r->global.CreateBufferObject(
			(uint32_t)(sizeof(LocalDataEntry) * entries.size()),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			data.localStagingBufferObject,
			data.localStagingBufferMemory,
			nullptr));
	
		data.localBufferInfo = {
			data.localBufferObject,
			0,
			sizeof(LocalDataEntry) * entries.size()
		};
		data.localStagingBufferInfo = {
			data.localStagingBufferObject,
			0,
			sizeof(LocalDataEntry) * entries.size()
		};
	}

	UploadLocalData(r, data);
}

#endif

}
