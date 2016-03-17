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
	VkDeviceMemory deviceMem;
	VkImageView view;
	VkFormat format;
	U32 w, h;
	U32 mipLevels;
	SDL_Surface* surf;
} SGL_Texture;
typedef struct _SwapchainBuffers
{
	VkImage image;
	VkCommandBuffer cmd;
	VkImageView view;
} SwapchainBuffers;
typedef struct _SwapChainBuffer 
{
	VkImage image;
	VkImageView view;
} SwapChainBuffer;
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
	VkSwapchainKHR swapChain;
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
	SwapChainBuffer* buffers;
	VkCommandBuffer* drawCmdBuffers;
	VkFramebuffer* frameBuffers;

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
	U32 imageCount;
	U32 maxImageCount;
	U32 bufferCount;
	U32 maxBufferCount;
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
	SwapchainBuffers *buffers;
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
U32 SGL_PrepareVertices(SGL_VkSwapChain* swapChain);
U32 SGL_PrepareUniformBuffers(SGL_VkSwapChain* swapChain);
U32 SGL_PreparePipelines(SGL_VkSwapChain* swapChain);
U32 SGL_SetupDescriptorSetLayout(SGL_VkSwapChain* swapChain);
U32 SGL_SetupDescriptorPool(SGL_VkSwapChain* swapChain);
U32 SGL_SetupDescriptorSet(SGL_VkSwapChain* swapChain);
U32 SGL_BuildCommandBuffers(SGL_VkSwapChain* swapChain);
U32 SGL_PrepareTexture(SGL_VkSwapChain * swapChain, SGL_Texture* tex, VkImageTiling tiling, VkImageUsageFlags usage, VkFlags requiredProps);
U32 SGL_PrepareTextureToOptimalTiling(SGL_VkSwapChain * swapChain, SGL_Texture* tex);
U32 SGL_PrepareTextureTest(SGL_VkSwapChain * swapChain, SGL_Texture* tex);
inline void SGL_Draw(SGL_VkSwapChain* swapChain)
{
	VkResult err;
	VkSemaphore presentCompleteSemaphore;
	VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
	presentCompleteSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	presentCompleteSemaphoreCreateInfo.pNext = VK_NULL_HANDLE;
	presentCompleteSemaphoreCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	err = vkCreateSemaphore(swapChain->device, &presentCompleteSemaphoreCreateInfo, VK_NULL_HANDLE, &presentCompleteSemaphore);
	SDL_assert(!err);

	// Get next image in the swap chain (back/front buffer)
	err = swapChain->fpAcquireNextImageKHR(swapChain->device, swapChain->swapChain, UINT64_MAX, presentCompleteSemaphore, (VkFence)VK_NULL_HANDLE, &swapChain->currentBuffer);
	assert(!err);
	{
		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &presentCompleteSemaphore;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &swapChain->drawCmdBuffers[swapChain->currentBuffer];
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = VK_NULL_HANDLE;

		// Submit draw command buffer
		err = vkQueueSubmit(swapChain->queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
		{
			VkPresentInfoKHR presentInfo;
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pNext = NULL;
			presentInfo.waitSemaphoreCount = 0;
			presentInfo.pWaitSemaphores = VK_NULL_HANDLE;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = &swapChain->swapChain;
			presentInfo.pImageIndices = &swapChain->currentBuffer;
			presentInfo.pResults = VK_NULL_HANDLE;

			err = swapChain->fpQueuePresentKHR(swapChain->queue, &presentInfo);
			assert(!err);
		}
		vkDestroySemaphore(swapChain->device, presentCompleteSemaphore, VK_NULL_HANDLE);
	}
	{
		// alla tämä funktio
		VkCommandBufferBeginInfo cmdBufInfo;
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.pNext = VK_NULL_HANDLE;
		cmdBufInfo.flags = 0;
		cmdBufInfo.pInheritanceInfo = VK_NULL_HANDLE;

		err = vkBeginCommandBuffer(swapChain->postPresentCmdBuffer, &cmdBufInfo);
		assert(!err);

		VkImageMemoryBarrier postPresentBarrier;
		postPresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		postPresentBarrier.pNext = VK_NULL_HANDLE;
		postPresentBarrier.srcAccessMask = 0;
		postPresentBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		postPresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		postPresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		postPresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		postPresentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		postPresentBarrier.subresourceRange.baseMipLevel = 0;
		postPresentBarrier.subresourceRange.levelCount = 1;
		postPresentBarrier.subresourceRange.baseArrayLayer = 0;
		postPresentBarrier.subresourceRange.layerCount = 1;
		postPresentBarrier.image = swapChain->buffers[swapChain->currentBuffer].image;

		vkCmdPipelineBarrier(
			swapChain->postPresentCmdBuffer,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, VK_NULL_HANDLE, // No memory barriers,
			0, VK_NULL_HANDLE, // No buffer barriers,
			1, &postPresentBarrier);

		err = vkEndCommandBuffer(swapChain->postPresentCmdBuffer);
		assert(!err);


		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = VK_NULL_HANDLE;
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = VK_NULL_HANDLE;
		submitInfo.pWaitDstStageMask = VK_NULL_HANDLE;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &swapChain->postPresentCmdBuffer;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = VK_NULL_HANDLE;

		err = vkQueueSubmit(swapChain->queue, 1, &submitInfo, VK_NULL_HANDLE);
		assert(!err);
	}

	err = vkQueueWaitIdle(swapChain->queue);
	assert(!err);
}
inline void SGL_Render(SGL_VkSwapChain* swapChain)
{
	if (!swapChain->prepared)
	{
		return;
	}
	vkDeviceWaitIdle(swapChain->device);
	SGL_Draw(swapChain);
	vkDeviceWaitIdle(swapChain->device);
}