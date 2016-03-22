#pragma once
#include "SDL.h"
#include "SGL_camera.h"
#include "SGL_types.h"
#include "SGL_math.h"
#include "linmath.h"
#include <assert.h>
#include <vulkan/vulkan.h>
#include <vulkan/vk_sdk_platform.h>
#if defined(NDEBUG) && defined(__GNUC__)
#define U_ASSERT_ONLY __attribute__((unused))
#else
#define U_ASSERT_ONLY
#endif

#define DEMO_TEXTURE_COUNT 1

#ifdef _WIN32
#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        MessageBox(NULL, err_msg, err_class, MB_OK);                           \
        exit(1);                                                               \
    } while (0)

#else // _WIN32

#define ERR_EXIT(err_msg, err_class)                                           \
    do {                                                                       \
        printf(err_msg);                                                       \
        fflush(stdout);                                                        \
        exit(1);                                                               \
    } while (0)
#endif // _WIN32

#define GET_INSTANCE_PROC_ADDR(inst, entrypoint)                               \
    {                                                                          \
       window->vkContext.fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)vkGetInstanceProcAddr(inst, "vk" #entrypoint); \
        if (window->vkContext.fp##entrypoint == NULL) {                                    \
            ERR_EXIT("vkGetInstanceProcAddr failed to find vk" #entrypoint,    \
                     "vkGetInstanceProcAddr Failure");                         \
        }                                                                      \
    }

static PFN_vkGetDeviceProcAddr g_gdpa = NULL;

#define GET_DEVICE_PROC_ADDR(dev, entrypoint)                                  \
    {                                                                          \
        if (!g_gdpa)                                                           \
            g_gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(           \
                window->vkContext.inst, "vkGetDeviceProcAddr");                            \
        window->vkContext.fp##entrypoint =                                                 \
            (PFN_vk##entrypoint)g_gdpa(dev, "vk" #entrypoint);                 \
        if (window->vkContext.fp##entrypoint == NULL) {                                    \
            ERR_EXIT("vkGetDeviceProcAddr failed to find vk" #entrypoint,      \
                     "vkGetDeviceProcAddr Failure");                           \
        }                                                                      \
    }
typedef struct _SGL_Window SGL_Window;
struct vkcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float color[12 * 3][4];
};

struct vktexcube_vs_uniform 
{
	// Must start with MVP
	float mvp[4][4];
	float position[12 * 3][4];
	float attr[12 * 3][4];
};
struct texture_object
{
	VkSampler sampler;

	VkImage image;
	VkImageLayout imageLayout;

	VkMemoryAllocateInfo mem_alloc;
	VkDeviceMemory mem;
	VkImageView view;
	I32 tex_width, tex_height;
	SDL_Surface*  surf;
};
typedef struct _SGL_Texture
{
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkMemoryAllocateInfo mem_alloc;
	VkDeviceMemory mem;
	VkImageView view;
	VkFormat format;
	U32 w, h;
	U32 mipLevels;
	SDL_Surface* surf;
} SGL_Texture;
/*struct texture_object 
{
	VkSampler sampler;
	VkImage image;
	VkImageLayout imageLayout;
	VkMemoryAllocateInfo mem_alloc;
	VkDeviceMemory mem;
	VkImageView view;
	U32 width, height;
};*/
typedef struct _SwapchainImage
{
	VkImage image;
	VkCommandBuffer cmd;
	VkImageView view;
} SwapchainImage;
typedef struct _DepthStencil
{
	VkImage image;
	VkDeviceMemory mem;
	VkImageView view;
} DepthStencil;
typedef struct _SGL_VertexBuffer
{
	VkBuffer buf;
	VkDeviceMemory mem;
	VkVertexInputBindingDescription* bindingDescriptions;
	VkVertexInputAttributeDescription* attributeDescriptions;
	U32 bindingCount;
	U32 attributeCount;
	VkPipelineVertexInputStateCreateInfo inputState;
} SGL_VertexBuffer;
typedef struct _SGL_IndexBuffer
{
	VkBuffer buf;
	VkDeviceMemory mem;
	U32 count;
} SGL_IndexBuffer;
typedef struct _SGL_UBO
{
	SGL_Mat4 projection;
	SGL_Mat4 model;
	float lodBias;
} SGL_UBO;
typedef struct _SGL_UniformBuffer
{
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDescriptorBufferInfo descriptor;
	U32 allocSize;
} SGL_UniformBuffer;
//#undef NDEBUG
typedef struct _SGL_VkSwapChain
{
	//pointer types
	VkInstance inst;
	VkSurfaceKHR surf;
	VkDevice device;
	VkQueue queue;
	VkPhysicalDevice gpu;
	VkCommandPool cmdPool;
	VkSwapchainKHR swapchain;
	VkRenderPass renderPass;
	VkPipeline mainPipeline;
	VkPipelineCache pipelineCache;
	VkCommandBuffer setupCmdBuffer;
	VkCommandBuffer postPresentCmdBuffer;
	VkCommandBuffer prePresentCmdBuffer;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkShaderModule* shaders;
	VkImage* images;
	VkImage swapchainImages[2];
	VkCommandBuffer swapchainCmdBuffers[2];
	VkImageView swapchainViews[2];
	VkFramebuffer* framebuffers;

	//function pointer types
	PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;
#ifndef NDEBUG
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
	VkDebugReportCallbackEXT msgCallback;
#endif
	//large structs
	VkPhysicalDeviceProperties gpuProps;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	DepthStencil depthStencil;
	SGL_Texture tex;
	SGL_VertexBuffer vertexBuffer;
	SGL_IndexBuffer indexBuffer;
	SGL_UniformBuffer uniformBuffer;
	SGL_Mat4 projection_matrix;
	SGL_Mat4 view_matrix;
	SGL_Mat4 model_matrix;
	float spin_angle;
	float spin_increment;
	SGL_UBO ubo;
	SGL_Camera cameras[SGL_CAMERA_COUNT];
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
	VkFormat colorFormat;
	VkFormat depthFormat;
	VkColorSpaceKHR colorSpace;
	//example code shit
	SGL_Vec3 rotation;
	F32 zoom;
	SGL_Vec2 mousePos;


	U32 prepared;
	U32 currentBuffer;
	U32 queueNodeIndex;
	U32 shaderCount;
	U32 maxShaderCount;
	U32 imageCount;
	U32 maxImageCount;
	U32 bufferCount;
	U32 maxBufferCount;
	U32 swapchainImageCount;
}SGL_VkSwapChain;
typedef struct _SGL_VkContext
{
#ifdef _WIN32
	HINSTANCE connection;        // hInstance - Windows Instance
	HWND window;                 // hWnd - window handle
#else                            // _WIN32
	xcb_connection_t *connection;
	xcb_screen_t *screen;
	xcb_window_t window;
	xcb_intern_atom_reply_t *atom_wm_delete_window;
#endif                           // _WIN32
	VkSurfaceKHR surface;
	SGL_bool prepared;
	SGL_bool use_staging_buffer;

	VkInstance inst;
	VkPhysicalDevice gpu;
	VkDevice device;
	VkQueue queue;
	U32 graphics_queue_node_index;
	VkPhysicalDeviceProperties gpu_props;
	VkQueueFamilyProperties *queue_props;
	VkPhysicalDeviceMemoryProperties memory_properties;

	U32 enabled_extension_count;
	U32 enabled_layer_count;
	char *extension_names[64];
	char *device_validation_layers[64];

	I32 width, height;
	VkFormat format;
	VkColorSpaceKHR color_space;

	PFN_vkGetPhysicalDeviceSurfaceSupportKHR
		fpGetPhysicalDeviceSurfaceSupportKHR;
	PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR
		fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
	PFN_vkGetPhysicalDeviceSurfaceFormatsKHR
		fpGetPhysicalDeviceSurfaceFormatsKHR;
	PFN_vkGetPhysicalDeviceSurfacePresentModesKHR
		fpGetPhysicalDeviceSurfacePresentModesKHR;
	PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
	PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
	PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
	PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
	PFN_vkQueuePresentKHR fpQueuePresentKHR;
	uint32_t swapchainImageCount;
	VkSwapchainKHR swapchain;
	SwapchainImage *buffers;
	VkCommandBuffer postPresentCmdBuffer;
	VkCommandBuffer prePresentCmdBuffer;
	VkCommandBuffer cmd; // Buffer for initialization commands
	VkCommandPool cmd_pool;

	struct {
		VkFormat format;

		VkImage image;
		VkMemoryAllocateInfo mem_alloc;
		VkDeviceMemory mem;
		VkImageView view;
	} depth;

	struct texture_object textures[DEMO_TEXTURE_COUNT];

	struct {
		VkBuffer buf;
		VkMemoryAllocateInfo mem_alloc;
		VkDeviceMemory mem;
		VkDescriptorBufferInfo buffer_info;
	} uniform_data;

	VkPipelineLayout pipeline_layout;
	VkDescriptorSetLayout desc_layout;
	VkPipelineCache pipelineCache;
	VkRenderPass render_pass;
	VkPipeline pipeline;

	SGL_Mat4 projection_matrix;
	SGL_Mat4 view_matrix;
	SGL_Mat4 model_matrix;

	float spin_angle;
	float spin_increment;
	SGL_bool pause;

	VkShaderModule vert_shader_module;
	VkShaderModule frag_shader_module;

	VkDescriptorPool desc_pool;
	VkDescriptorSet desc_set;

	VkFramebuffer *framebuffers;

	I32 curFrame;
	I32 frameCount;
	SGL_bool validate;
	SGL_bool use_break;

	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback;
	VkDebugReportCallbackEXT msg_callback;
	PFN_vkDebugReportMessageEXT DebugReportMessage;

	U32 current_buffer;
	U32 queue_count;
} SGL_VkContext;
inline VkFormat SGL_PixelFormatToVkFormat(const U32 pixelFormat)
{
	//this function became quite useless once i found out that vulkan very picky with pixel formats
	switch (pixelFormat)
	{
	/*case SDL_PIXELFORMAT_RGBA4444:
	{
		return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_BGRA4444:
	{
		return VK_FORMAT_B4G4R4A4_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_ARGB1555:
	{
		return VK_FORMAT_A1R5G5B5_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_BGRA5551:
	{
		return VK_FORMAT_B5G5R5A1_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_RGB565:
	{
		return VK_FORMAT_R5G5B5A1_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_BGR565:
	{
		return VK_FORMAT_B5G6R5_UNORM_PACK16;
	}
	case SDL_PIXELFORMAT_RGBA8888:
	{
		return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
	}*/
	case SDL_PIXELFORMAT_ARGB8888:
	{
		return VK_FORMAT_B8G8R8A8_UNORM;
	}
/*	case SDL_PIXELFORMAT_ABGR8888:
	{
		return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
	}
	case SDL_PIXELFORMAT_BGRA8888:
	{
		return VK_FORMAT_B8G8R8A8_UNORM;
	}
	case SDL_PIXELFORMAT_ARGB2101010:
	{
		return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
	}*/
	default:
	{
		return VK_FORMAT_UNDEFINED;
	}
	}
}
U32 SGL_InitVulkan(SGL_Window* window);
U32 SGL_PrepareVulkan(SGL_VkSwapChain* swapChain);
U32 SGL_UpdateUniforms(SGL_VkSwapChain* swapchain);
U32 SGL_Draw(SGL_VkSwapChain* swapchain);
inline void SGL_Render(SGL_VkSwapChain* swapChain)
{
	if (!swapChain->prepared)
		return;
	// Wait for work to finish before updating MVP.
	vkDeviceWaitIdle(swapChain->device);
	SGL_UpdateUniforms(swapChain);

	SGL_Draw(swapChain);

	// Wait for work to finish before updating MVP.
	vkDeviceWaitIdle(swapChain->device);
}