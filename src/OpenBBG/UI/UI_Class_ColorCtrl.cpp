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

static const Vertex g_vb_solid_face_colors_Data[] = {
    // green face
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(16, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 256, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 256, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(16, -1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(16, 256, -1), XYZ1(0.f, 1.f, 0.f)},
    // green face
    {XYZ1(-1, -1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(256, -1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, 16, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, 16, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(256, -1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(256, 16, -1), XYZ1(1.f, 1.f, 0.f)},
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

	graphicsPipeline->Init(r->global.device);
	pipeline = graphicsPipeline->CreatePipeline(r->global.device, r->global.pipelineCache, r->global.renderNode, 0);
	
	// Vertex Buffer
	{
		VkMemoryRequirements memReqs;
		assert(r->global.CreateBufferObject(
			sizeof(g_vb_solid_face_colors_Data),
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			vertexBufferObject,
			vertexBufferMemory,
			&memReqs));
		vertexBufferInfo.range = memReqs.size;
		vertexBufferInfo.offset = 0;

		uint8_t *pData;
		assert(VK_SUCCESS == vkMapMemory(r->global.device, vertexBufferMemory, 0, memReqs.size, 0, (void **)&pData));

		memcpy(pData, g_vb_solid_face_colors_Data, sizeof(g_vb_solid_face_colors_Data));

		vkUnmapMemory(r->global.device, vertexBufferMemory);
	}

	isInitialized = true;
}

void
UI_Class_ColorCtrl::Cleanup(Renderer_Vulkan *r)
{
	if (isInitialized == false)
		return;
	
	// Vertex Buffer
	vkDestroyBuffer(r->global.device, vertexBufferObject, nullptr);
	vkFreeMemory(r->global.device, vertexBufferMemory, nullptr);

	vkDestroyPipeline(r->global.device, pipeline, nullptr);

	delete graphicsPipeline;
	graphicsPipeline = nullptr;

	isInitialized = false;
}

void
UI_Class_ColorCtrl::Prepare(Renderer_Vulkan *r, UI_Context *ctx)
{
	if (isInitialized == false)
		Init(r);
}

void
UI_Class_ColorCtrl::Prepare(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_ColorCtrl::RenderOpaque(Renderer_Vulkan *r, UI_Context *ctx)
{
	vkCmdBindPipeline(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
	vkCmdBindDescriptorSets(r->global.primaryCommandPool.currentBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->pipeline_layout, 0, (uint32_t)r->global.descGlobalParamSets.size(), r->global.descGlobalParamSets.data(), 0, nullptr);
	const VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(r->global.primaryCommandPool.currentBuffer, 0, 1, &vertexBufferObject, offsets);
	vkCmdDraw(r->global.primaryCommandPool.currentBuffer, 4 * 3, 1, 0, 0);
}

void
UI_Class_ColorCtrl::RenderTransparent(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}

void
UI_Class_ColorCtrl::RenderOverlay(Renderer_Vulkan *r, UI_Context *ctx, UI_Control *ctrl)
{
}
#endif

}
