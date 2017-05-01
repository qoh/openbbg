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
sortInstances(UI_Component_ColorQuad::Instance &a, UI_Component_ColorQuad::Instance &b)
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
UI_Component_ColorQuad::Init()
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
				sizeof(Instance),
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

	auto r = Renderer_Vulkan::Get();
	graphicsPipeline->Init();
	pipeline = graphicsPipeline->CreatePipeline(r->global.device, r->global.pipelineCache, r->global.renderNode, 0);
	
	// Vertex Buffer
	{
		vertexBuffer.Init(sizeof(g_vertexData));
		uint8_t *pData;
		vertexBuffer.MapMemory((void **)&pData);
		memcpy(pData, g_vertexData, sizeof(g_vertexData));
		vertexBuffer.UnmapMemory();
	}

	isInitialized = true;
}

void
UI_Component_ColorQuad::Cleanup()
{
	if (isInitialized == false)
		return;
	
	for (auto &entry : instancedDataMap) {
		auto &data = entry.second;
		data.Cleanup();
	}
	
	vertexBuffer.Cleanup();
	
	auto r = Renderer_Vulkan::Get();
	graphicsPipeline->Cleanup();
	delete graphicsPipeline;
	graphicsPipeline = nullptr;

	isInitialized = false;
}

void
UI_Component_ColorQuad::Cleanup(UI_Context *ctx)
{
}

void
UI_Component_ColorQuad::Cleanup(UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::Prepare(UI_Context *ctx)
{
	if (isInitialized == false)
		Init();

	CreateInstancedData(ctx);

	auto &data = instancedDataMap[ctx];
	data.Prepare(this, sortInstances);
}

void
UI_Component_ColorQuad::Prepare(UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::RenderOpaque(UI_Context *ctx)
{
	auto r = Renderer_Vulkan::Get();
#if 1
	{
		InstancedData &data = instancedDataMap[ctx];
		const VkDeviceSize offsets[1] = { 0 };
		vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
		vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBuffer.bufferObject, offsets);
		vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 1, 1, &data.instanceBuffer.deviceBufferObject, offsets);
		vkCmdDraw(r->global.primaryCommandPool.currentBuffer, 4 * 3, data.numOpaque, 0, 0);
	}
#endif
}

void
UI_Component_ColorQuad::RenderTransparent(UI_Context *ctx, vector<UI_ComponentInstance *> &instances, uint32_t startInstance, uint32_t numInstances)
{
	auto r = Renderer_Vulkan::Get();
#if 1
	{
		if (s_lastComponentRendered != this) {
			InstancedData &data = instancedDataMap[ctx];
			const VkDeviceSize offsets[1] = { 0 };
			vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
			vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
			vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBuffer.bufferObject, offsets);
			vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 1, 1, &data.instanceBuffer.deviceBufferObject, offsets);
		}
		vkCmdDraw(r->global.primaryCommandPool.currentBuffer, 4 * 3, numInstances, 0, instances[startInstance]->instanceIdx);
	}
#endif
}

void
UI_Component_ColorQuad::RenderOverlay(UI_Context *ctx, UI_ComponentInstance *compInst)
{
}

void
UI_Component_ColorQuad::PopulateTransparentInstances(UI_Context *ctx, vector<UI_ComponentInstance *> &instances)
{
	auto &data = instancedDataMap[ctx];
	uint32_t numInstances = (uint32_t)data.instances.size();
	for (uint32_t a = data.numOpaque; a < numInstances; ++a)
		instances.push_back(data.instances[a].compInst);
}

inline
void
UI_Component_ColorQuad::CreateInstancedData(UI_Context *ctx)
{
	auto search = instancedDataMap.find(ctx);
	if (search != instancedDataMap.end() && search->second.isInitialized)
		return;

	InstancedData &data = instancedDataMap[ctx];
	data.Init();
}
#endif

void
UI_Component_ColorQuad::OnAddToContext(UI_ComponentInstance *compInst, UI_Context *ctx)
{
#if OPENBBG_WITH_VULKAN
	auto localCompInst = static_cast<UI_ComponentInstance_ColorQuad *>(compInst);
	InstancedData &data = instancedDataMap[ctx];
	compInst->instanceIdx = (uint32_t)data.instances.size();
	data.isLocalBufferDirty = true;
	data.instances.push_back(Instance());
	auto &entry = data.instances.back();
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
	InstancedData &data = instancedDataMap[ctx];
	data.instances.erase(data.instances.begin() + compInst->instanceIdx);
	uint32_t numEntries = (uint32_t)data.instances.size();
	for (auto a = compInst->instanceIdx; a < numEntries; ++a)
		--data.instances[a].compInst->instanceIdx;
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
		auto &data = instancedDataMap[compInst->control->context];
		auto &entry = data.instances[compInst->instanceIdx];
		entry.position = compInst->relativePosition;
		entry.color = localCompInst->color;
		entry.scissor.x = entry.position.x;
		entry.scissor.y = entry.position.y;
		data.isLocalBufferDirty = true;
	}
}

}
