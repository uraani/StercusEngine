void SetImageLayout(const VkCommandBuffer cmdBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.oldLayout = oldImageLayout,
		.newLayout = newImageLayout,
		.image = image,
		.subresourceRange.aspectMask = aspectMask,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.layerCount = 1
	};
	// Source layouts (old)

	// Undefined layout
	// Only allowed as initial layout!
	// Make sure any writes to the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
	}

	// Old layout is color attachment
	// Make sure any writes to the color buffer have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	// Old layout is depth/stencil attachment
	// Make sure any writes to the depth/stencil buffer have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	// Old layout is transfer source
	// Make sure any reads from the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	// Old layout is shader read (sampler, input attachment)
	// Make sure any shader reads from the image have been finished
	if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}

	// Target layouts (new)

	// New layout is transfer destination (copy, blit)
	// Make sure any copyies to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	}

	// New layout is transfer source (copy, blit)
	// Make sure any reads from and writes to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
	{
		imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	// New layout is color attachment
	// Make sure any writes to the color buffer hav been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	// New layout is depth attachment
	// Make sure any writes to depth/stencil buffer have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	// New layout is shader read (sampler, input attachment)
	// Make sure any writes to the image have been finished
	if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		if (oldImageLayout != VK_IMAGE_LAYOUT_UNDEFINED)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	}


	// Put barrier on top
	VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(cmdBuffer, srcStageFlags, destStageFlags, 0, 0, VK_NULL_HANDLE, 0, VK_NULL_HANDLE, 1, &imageMemoryBarrier);
	U32 asd = 0;
}
inline void CreateCommandPool(SGL_VkSwapChain * swapChain)
{
	VkCommandPoolCreateInfo cmdPoolInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = swapChain->queueNodeIndex,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	};
	VkResult vkRes = vkCreateCommandPool(swapChain->device, &cmdPoolInfo, NULL, &swapChain->cmdPool);
	SDL_assert(!vkRes);
}
inline void CreateSetupCommandBuffer(SGL_VkSwapChain * swapChain)
{
	if (swapChain->setupCmdBuffer != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(swapChain->device, swapChain->cmdPool, 1, &swapChain->setupCmdBuffer);
		swapChain->setupCmdBuffer = VK_NULL_HANDLE; // todo : check if still necessary
	}
	VkCommandBufferAllocateInfo cmdBufferAllocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = swapChain->cmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};
	VkResult vkRes = vkAllocateCommandBuffers(swapChain->device, &cmdBufferAllocInfo, &swapChain->setupCmdBuffer);
	assert(!vkRes);

	// todo : Command buffer is also started here, better put somewhere else
	// todo : Check if necessaray at all...
	VkCommandBufferBeginInfo cmdBufInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
	};
	// todo : check null handles, flags?

	vkRes = vkBeginCommandBuffer(swapChain->setupCmdBuffer, &cmdBufInfo);
	assert(!vkRes);
}
inline void CreateSwapChain(SGL_VkSwapChain* swapChain)
{
	VkResult err;
	VkSwapchainKHR oldSwapchain = swapChain->swapChain;

	// Get physical device surface properties and formats
	VkSurfaceCapabilitiesKHR surfCaps;
	err = swapChain->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(swapChain->gpu, swapChain->surf, &surfCaps);
	if (err != VK_SUCCESS)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "fpGetPhysicalDeviceSurfaceCapabilitiesKHR Failure",
			"Couldnt find surface capabilities\n\n"
			"Please uninstall your life.\n", NULL);
	}
	assert(!err);

	// Get available present modes
	uint32_t presentModeCount;
	err = swapChain->fpGetPhysicalDeviceSurfacePresentModesKHR(swapChain->gpu, swapChain->surf, &presentModeCount, NULL);
	assert(!err);
	assert(presentModeCount > 0);

	VkPresentModeKHR* presentModes = SDL_malloc(presentModeCount*sizeof(VkPresentModeKHR));

	err = swapChain->fpGetPhysicalDeviceSurfacePresentModesKHR(swapChain->gpu, swapChain->surf, &presentModeCount, presentModes);
	assert(!err);

	VkExtent2D swapchainExtent = { 
		.width = swapChain->windowSize.x,
		.height = swapChain->windowSize.y
	};
	// the special value (0xFFFFFFFF,0xFFFFFFFF) indicating that the surface size will be determined by the extent of a swapchain targeting the surface.
	if (surfCaps.currentExtent.width != 0xFFFFFFFF)
	{
		swapchainExtent = surfCaps.currentExtent;
		swapChain->windowSize.x = surfCaps.currentExtent.width;
		swapChain->windowSize.y = surfCaps.currentExtent.height;
	}

	// Prefer mailbox mode if present, it's the lowest latency non-tearing present  mode
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (size_t i = 0; i < presentModeCount; i++)
	{
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
		{
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of images
	U32 desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
	if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
	{
		desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		preTransform = surfCaps.currentTransform;
	}

	VkSwapchainCreateInfoKHR swapchainCI = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.surface = swapChain->surf,
		.minImageCount = desiredNumberOfSwapchainImages,
		.imageFormat = swapChain->colorFormat,
		.imageColorSpace = swapChain->colorSpace,
		.imageExtent = { swapchainExtent.width, swapchainExtent.height },
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform,
		.imageArrayLayers = 1,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = VK_NULL_HANDLE,
		.presentMode = swapchainPresentMode,
		.oldSwapchain = oldSwapchain,
		.clipped = VK_TRUE,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
	};

	err = swapChain->fpCreateSwapchainKHR(swapChain->device, &swapchainCI, VK_NULL_HANDLE, &swapChain->swapChain);
	assert(!err);

	// If an existing sawp chain is re-created, destroy the old swap chain
	// This also cleans up all the presentable images
	if (oldSwapchain != VK_NULL_HANDLE)
	{
		swapChain->fpDestroySwapchainKHR(swapChain->device, oldSwapchain, VK_NULL_HANDLE);
	}

	err = swapChain->fpGetSwapchainImagesKHR(swapChain->device, swapChain->swapChain, &swapChain->imageCount, NULL);
	assert(!err);

	// Get the swap chain images
	swapChain->images = (VkImage*)SDL_realloc(swapChain->images, swapChain->imageCount*sizeof(VkImage));
	err = swapChain->fpGetSwapchainImagesKHR(swapChain->device, swapChain->swapChain, &swapChain->imageCount, swapChain->images);
	assert(!err);

	// Get the swap chain buffers containing the image and imageview
	swapChain->buffers = (SwapChainBuffer*)SDL_realloc(swapChain->buffers, swapChain->imageCount*sizeof(SwapChainBuffer));
	for (uint32_t i = 0; i < swapChain->imageCount; i++)
	{
		VkImageViewCreateInfo colorAttachmentView = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.format = swapChain->colorFormat,
			.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.flags = 0
		};

		swapChain->buffers[i].image = swapChain->images[i];

		// Transform images from initial (undefined) to present layout
		SetImageLayout(swapChain->setupCmdBuffer, swapChain->buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		colorAttachmentView.image = swapChain->buffers[i].image;

		err = vkCreateImageView(swapChain->device, &colorAttachmentView, VK_NULL_HANDLE, &swapChain->buffers[i].view);
		assert(!err);
	}
}
inline void CreateCommandBuffers(SGL_VkSwapChain* swapChain)
{
	// Create one command buffer per frame buffer 
	// in the swap chain
	// Command buffers store a reference to the 
	// frame buffer inside their render pass info
	// so for static usage withouth having to rebuild 
	// them each frame, we use one per frame buffer
	swapChain->drawCmdBuffers = (VkCommandBuffer*)SDL_realloc(swapChain->drawCmdBuffers, swapChain->imageCount*sizeof(VkCommandBuffer));

	VkCommandBufferAllocateInfo cmdBufferAllocInfo = 
	{
	.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
	.commandPool = swapChain->cmdPool,
	.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
	.commandBufferCount = swapChain->imageCount,
	};
	VkResult vkRes = vkAllocateCommandBuffers(swapChain->device, &cmdBufferAllocInfo, swapChain->drawCmdBuffers);
	assert(!vkRes);

	// Command buffers for submitting present barriers
	cmdBufferAllocInfo.commandBufferCount = 1;
	// Pre present
	vkRes = vkAllocateCommandBuffers(swapChain->device, &cmdBufferAllocInfo, &swapChain->prePresentCmdBuffer);
	assert(!vkRes);
	// Post present
	vkRes = vkAllocateCommandBuffers(swapChain->device, &cmdBufferAllocInfo, &swapChain->postPresentCmdBuffer);
	assert(!vkRes);
}
inline VkBool32 GetMemoryType(const VkPhysicalDeviceMemoryProperties* memProps, U32 typeBits, VkFlags properties, U32 * typeIndex)
{
	for (U32 i = 0; i < 32; i++)
	{
		if ((typeBits & 1) == 1)
		{
			if ((memProps->memoryTypes[i].propertyFlags & properties) == properties)
			{
				*typeIndex = i;
				return VK_TRUE;
			}
		}
		typeBits >>= 1;
	}
	return VK_FALSE;
}
//TODO: IN 2D rendering engine theres no need for depth buffer
inline void SetupDepthStencil(SGL_VkSwapChain* swapChain)
{
	VkImageCreateInfo image = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = swapChain->depthFormat,
		.extent = { swapChain->windowSize.x, swapChain->windowSize.y, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		.flags = 0
	};
	VkMemoryAllocateInfo mem_alloc = 
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = 0,
		.memoryTypeIndex = 0,
	};

	VkImageViewCreateInfo depthStencilView = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = swapChain->depthFormat,
		.flags = 0,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
	};

	VkMemoryRequirements memReqs;
	VkResult err;

	err = vkCreateImage(swapChain->device, &image, VK_NULL_HANDLE, &swapChain->depthStencil.image);
	assert(!err);
	vkGetImageMemoryRequirements(swapChain->device, swapChain->depthStencil.image, &memReqs);
	mem_alloc.allocationSize = memReqs.size;
	GetMemoryType(&swapChain->memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mem_alloc.memoryTypeIndex);
	err = vkAllocateMemory(swapChain->device, &mem_alloc, VK_NULL_HANDLE, &swapChain->depthStencil.mem);
	assert(!err);

	err = vkBindImageMemory(swapChain->device, swapChain->depthStencil.image, swapChain->depthStencil.mem, 0);
	assert(!err);
	SetImageLayout(swapChain->setupCmdBuffer, swapChain->depthStencil.image, depthStencilView.subresourceRange.aspectMask, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	depthStencilView.image = swapChain->depthStencil.image;
	err = vkCreateImageView(swapChain->device, &depthStencilView, VK_NULL_HANDLE, &swapChain->depthStencil.view);
	assert(!err);
}
inline void SetupRenderPass(SGL_VkSwapChain* swapChain)
{
	VkAttachmentDescription attachments[2];
	attachments[0].format = swapChain->colorFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	attachments[1].format = swapChain->depthFormat;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	const VkAttachmentReference colorReference = 
	{
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	const VkAttachmentReference depthReference =
	{
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = 
	{
		.flags = 0,
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.inputAttachmentCount = 0,
		.pInputAttachments = VK_NULL_HANDLE,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorReference,
		.pResolveAttachments = VK_NULL_HANDLE,
		.pDepthStencilAttachment = &depthReference,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = VK_NULL_HANDLE
	};

	VkRenderPassCreateInfo renderPassInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 0,
		.pDependencies = VK_NULL_HANDLE
	};

	VkResult err;
	err = vkCreateRenderPass(swapChain->device, &renderPassInfo, VK_NULL_HANDLE, &swapChain->renderPass);
	SDL_assert(!err);
}
inline void CreatePipelineCache(SGL_VkSwapChain * swapChain)
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
	};
	VkResult err = vkCreatePipelineCache(swapChain->device, &pipelineCacheCreateInfo, VK_NULL_HANDLE, &swapChain->pipelineCache);
	SDL_assert(!err);
}
inline void SetupFrameBuffer(SGL_VkSwapChain* swapChain)
{
	VkImageView attachments[2];

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = swapChain->depthStencil.view;

	VkFramebufferCreateInfo frameBufferCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.renderPass = swapChain->renderPass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = swapChain->windowSize.x,
		.height = swapChain->windowSize.y,
		.layers = 1
	};

	// Create frame buffers for every swap chain image
	swapChain->frameBuffers = (VkFramebuffer*)SDL_realloc(swapChain->frameBuffers, swapChain->imageCount*sizeof(VkFramebuffer));
	for (uint32_t i = 0; i < swapChain->imageCount; i++)
	{
		attachments[0] = swapChain->buffers[i].view;
		VkResult err = vkCreateFramebuffer(swapChain->device, &frameBufferCreateInfo, VK_NULL_HANDLE, &swapChain->frameBuffers[i]);
		assert(!err);
	}
}
inline void FlushSetupCommandBuffer(SGL_VkSwapChain* swapChain)
{
	VkResult err;

	if (swapChain->setupCmdBuffer == VK_NULL_HANDLE)
		return;

	err = vkEndCommandBuffer(swapChain->setupCmdBuffer);
	assert(!err);

	VkSubmitInfo submitInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &swapChain->setupCmdBuffer
	};

	err = vkQueueSubmit(swapChain->queue, 1, &submitInfo, VK_NULL_HANDLE);
	assert(!err);

	err = vkQueueWaitIdle(swapChain->queue);
	assert(!err);

	vkFreeCommandBuffers(swapChain->device, swapChain->cmdPool, 1, &swapChain->setupCmdBuffer);
	swapChain->setupCmdBuffer = VK_NULL_HANDLE; // todo : check if still necessary
}
U32 SGL_PrepareVulkan(SGL_VkSwapChain * swapChain)
{
	CreateCommandPool(swapChain);
	CreateSetupCommandBuffer(swapChain);
	CreateSwapChain(swapChain);
	CreateCommandBuffers(swapChain);
	SetupDepthStencil(swapChain);
	SetupRenderPass(swapChain);
	CreatePipelineCache(swapChain);
	SetupFrameBuffer(swapChain);
	FlushSetupCommandBuffer(swapChain);
	CreateSetupCommandBuffer(swapChain);
	return SGL_TRUE;
}
inline VkBool32 CreateBuffer(const SGL_VkSwapChain* swapChain, VkBufferUsageFlags usage, VkDeviceSize size, void * data, VkBuffer *buffer, VkDeviceMemory *memory)
{
	VkMemoryRequirements memReqs;
	VkMemoryAllocateInfo memAlloc = 
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.allocationSize = 0,
		.memoryTypeIndex = 0
	};
	VkBufferCreateInfo bufferCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.usage = usage,
		.size = size,
		.flags = 0
	};

	VkResult err = vkCreateBuffer(swapChain->device, &bufferCreateInfo, VK_NULL_HANDLE, buffer);
	assert(!err);
	vkGetBufferMemoryRequirements(swapChain->device, *buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	if (!GetMemoryType(&swapChain->memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex))
	{
		SDL_Log("shreckd");
	}
	err = vkAllocateMemory(swapChain->device, &memAlloc, VK_NULL_HANDLE, memory);
	assert(!err);
	if (data != VK_NULL_HANDLE)
	{
		void *mapped;
		err = vkMapMemory(swapChain->device, *memory, 0, size, 0, &mapped);
		assert(!err);
		SDL_memcpy(mapped, data, size);
		vkUnmapMemory(swapChain->device, *memory);
	}
	err = vkBindBufferMemory(swapChain->device, *buffer, *memory, 0);
	assert(!err);
	return VK_TRUE;
}
U32 SGL_PrepareVertices(SGL_VkSwapChain * swapChain)
{
	// Setup vertices for a single uv-mapped quad
	SGL_Vec2 vertexBuffer[] =
	{
		{  1.0f,  1.0f },{ 1.0f, 1.0f },
		{ -1.0f,  1.0f },{ 0.0f, 1.0f },
		{ -1.0f, -1.0f },{ 0.0f, 0.0f },
		{  1.0f, -1.0f },{ 1.0f, 0.0f }
	};
	// Setup indices
	U32 indexBuffer[] = { 0,1,2, 2,3,0 };
	swapChain->indexBuffer.count = SDL_arraysize(indexBuffer);
	CreateBuffer(swapChain, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, SDL_arraysize(vertexBuffer) * sizeof(SGL_Vec2), vertexBuffer, &swapChain->vertexBuffer.buf, &swapChain->vertexBuffer.mem);
	CreateBuffer(swapChain, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, swapChain->indexBuffer.count * sizeof(U32), indexBuffer, &swapChain->indexBuffer.buf, &swapChain->indexBuffer.mem);
	// Binding description
	swapChain->vertexBuffer.bindingCount = 1;
	swapChain->vertexBuffer.bindingDescriptions = SDL_malloc(sizeof(VkVertexInputBindingDescription));
	swapChain->vertexBuffer.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID,
	swapChain->vertexBuffer.bindingDescriptions[0].stride = sizeof(SGL_Vec2) * 2,
	swapChain->vertexBuffer.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
	// Attribute descriptions
	// Describes memory layout and shader positions
	swapChain->vertexBuffer.attributeCount = 2;
	swapChain->vertexBuffer.attributeDescriptions = SDL_malloc(sizeof(VkVertexInputAttributeDescription)*2);
	// Location 0 : Position
	swapChain->vertexBuffer.attributeDescriptions[0].location = 0;
	swapChain->vertexBuffer.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	swapChain->vertexBuffer.attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	swapChain->vertexBuffer.attributeDescriptions[0].offset = 0;
	// Location 1 : Texture coordinates
	swapChain->vertexBuffer.attributeDescriptions[1].location = 1;
	swapChain->vertexBuffer.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
	swapChain->vertexBuffer.attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	swapChain->vertexBuffer.attributeDescriptions[1].offset = sizeof(SGL_Vec2);

	swapChain->vertexBuffer.inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	swapChain->vertexBuffer.inputState.pNext = VK_NULL_HANDLE;

	swapChain->vertexBuffer.inputState.vertexBindingDescriptionCount = swapChain->vertexBuffer.bindingCount;
	swapChain->vertexBuffer.inputState.pVertexBindingDescriptions = swapChain->vertexBuffer.bindingDescriptions;
	swapChain->vertexBuffer.inputState.vertexAttributeDescriptionCount = swapChain->vertexBuffer.attributeCount;
	swapChain->vertexBuffer.inputState.pVertexAttributeDescriptions = swapChain->vertexBuffer.attributeDescriptions;
	return SGL_TRUE;
}

U32 SGL_PrepareUniformBuffers(SGL_VkSwapChain * swapChain)
{
	swapChain->rotation.x = 45.0f;
	swapChain->rotation.y = 0.0f;
	swapChain->rotation.z = 0.0f;
	swapChain->zoom = -2.5f;
	swapChain->ubo.projection = SM_Perspective(degreesToRadians(60.0f), (float)swapChain->windowSize.x / (float)swapChain->windowSize.y, 0.1f, 256.0f);
	{
		SGL_Vec4 translationV = { 0.0f,0.0f,swapChain->zoom ,1.0f };
		SGL_Mat4 identityM = SM_IdentityMat4();
		SGL_Mat4 viewMatrix = SM_Translate(&identityM, &translationV);
		swapChain->ubo.model = SM_M4Multiply(&viewMatrix, &identityM);
	}
	{
		SGL_Vec4 right = { 1.0f,0.0f,0.0f,1.0f };
		SGL_Vec4 up = { 0.0f,1.0f,0.0f,1.0f };
		SGL_Vec4 forward = { 0.0f,0.0f,1.0f,1.0f };
		swapChain->ubo.model = SM_Rotate(&swapChain->ubo.model, degreesToRadians(swapChain->rotation.x), &right);
		swapChain->ubo.model = SM_Rotate(&swapChain->ubo.model, degreesToRadians(swapChain->rotation.y), &up);
		swapChain->ubo.model = SM_Rotate(&swapChain->ubo.model, degreesToRadians(swapChain->rotation.z), &forward);
	}

	// Vertex shader uniform buffer block
	CreateBuffer(swapChain, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(SGL_UBO), &swapChain->ubo, &swapChain->uniformBuffer.buffer, &swapChain->uniformBuffer.memory);
	swapChain->uniformBuffer.descriptor.buffer = swapChain->uniformBuffer.buffer;
	swapChain->uniformBuffer.descriptor.offset = 0;
	swapChain->uniformBuffer.descriptor.range = sizeof(SGL_UBO);
	return SGL_TRUE;
}
VkPipelineShaderStageCreateInfo LoadShader(SGL_VkSwapChain* swapChain, const SGL_Shader* shader, VkShaderStageFlagBits stageBits)
{

	VkResult err;
	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.codeSize = shader->size,
		.pCode = (U32*)shader->data
	};
	err = vkCreateShaderModule(swapChain->device, &moduleCreateInfo, VK_NULL_HANDLE, &shaderModule);
	assert(!err);
	VkPipelineShaderStageCreateInfo shaderStage = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = stageBits,
		.module = shaderModule,
		.pName = "main" // TODO : make param
	};
	swapChain->shaderCount++;
	swapChain->shaders = SDL_realloc(swapChain->shaders, sizeof(VkShaderModule)*swapChain->shaderCount);
	swapChain->shaders[swapChain->shaderCount] = shaderModule;
	return shaderStage;
}
U32 SGL_PreparePipelines(SGL_VkSwapChain * swapChain)
{
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.flags = 0,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
	VkPipelineRasterizationStateCreateInfo rasterizationState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.flags = 0,
		.depthClampEnable = VK_TRUE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_NONE,
		.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE
	};
	VkPipelineColorBlendAttachmentState blendAttachmentState =
	{
		.colorWriteMask = 0xf,
		.blendEnable = VK_FALSE
	};
	VkPipelineColorBlendStateCreateInfo colorBlendState =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.pNext = NULL,
		.attachmentCount = 1,
		.pAttachments = &blendAttachmentState
	};
	//not 100% sure about this one
	VkPipelineDepthStencilStateCreateInfo depthStencilState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
		.back.compareOp = VK_COMPARE_OP_ALWAYS
	};
	VkPipelineViewportStateCreateInfo viewportState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.flags = 0,
		.viewportCount = 1,
		.scissorCount = 1
	};

	VkPipelineMultisampleStateCreateInfo multisampleState = 
	{
	.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
	.flags = 0,
	.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
	};
	VkDynamicState dynamicStateEnables[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = SDL_arraysize(dynamicStateEnables),
		.pDynamicStates = dynamicStateEnables,
	};
	// Load shaders
	VkPipelineShaderStageCreateInfo shaderStages[2];
	SGL_Shader vert = SGL_DataLoadShader("texShaderVert");
	SGL_Shader frag = SGL_DataLoadShader("texShaderFrag");
	shaderStages[0] = LoadShader(swapChain, &vert, VK_SHADER_STAGE_VERTEX_BIT);
	shaderStages[1] = LoadShader(swapChain, &frag, VK_SHADER_STAGE_FRAGMENT_BIT);

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.flags = 0,
		.stageCount = 2,
		.renderPass = swapChain->renderPass,
		.pVertexInputState = &swapChain->vertexBuffer.inputState,
		.pInputAssemblyState = &inputAssemblyState,
		.pViewportState = &viewportState,
		.pRasterizationState = &rasterizationState,
		.pMultisampleState = &multisampleState,
		.pDepthStencilState = &depthStencilState,
		.pColorBlendState = &colorBlendState,
		.pDynamicState = &dynamicState,
		.layout = swapChain->pipelineLayout,
		.pStages = shaderStages
	};

	VkResult err = vkCreateGraphicsPipelines(swapChain->device, swapChain->pipelineCache, 1, &pipelineCreateInfo, VK_NULL_HANDLE, &swapChain->mainPipeline);
	assert(!err);
	return SGL_TRUE;
}

U32 SGL_SetupDescriptorSetLayout(SGL_VkSwapChain * swapChain)
{
	VkDescriptorSetLayoutBinding setLayoutBindings[] =
	{
		// Binding 0 : Vertex shader uniform buffer
		{
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.binding = 0,
			// Default value in all examples
			.descriptorCount = 1
		},
		// Binding 1 : Fragment shader image sampler
		{
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.binding = 1,
			.descriptorCount = 1
		}
	};
	VkDescriptorSetLayoutCreateInfo descriptorLayout =
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.pBindings = setLayoutBindings,
		.bindingCount = SDL_arraysize(setLayoutBindings)
	};
	VkResult err = vkCreateDescriptorSetLayout(swapChain->device, &descriptorLayout, VK_NULL_HANDLE, &swapChain->descriptorSetLayout);
	assert(!err);

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.setLayoutCount = 1,
		.pSetLayouts = &swapChain->descriptorSetLayout
	};
	err = vkCreatePipelineLayout(swapChain->device, &pPipelineLayoutCreateInfo, VK_NULL_HANDLE, &swapChain->pipelineLayout);
	assert(!err);
	return SGL_TRUE;
}

U32 SGL_SetupDescriptorPool(SGL_VkSwapChain * swapChain)
{
	// Example uses one ubo and one image sampler
	VkDescriptorPoolSize poolSizes[2] =
	{
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1
		},
		{
			.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.descriptorCount = 1
		}
	};

	VkDescriptorPoolCreateInfo descriptorPoolInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.poolSizeCount = SDL_arraysize(poolSizes),
		.pPoolSizes = poolSizes,
		.maxSets = 2,
	};
	VkResult vkRes = vkCreateDescriptorPool(swapChain->device, &descriptorPoolInfo, VK_NULL_HANDLE, &swapChain->descriptorPool);
	assert(!vkRes);
	return SGL_TRUE;
}

U32 SGL_SetupDescriptorSet(SGL_VkSwapChain * swapChain)
{
	VkDescriptorSetAllocateInfo allocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.descriptorPool = swapChain->descriptorPool,
		.pSetLayouts = &swapChain->descriptorSetLayout,
		.descriptorSetCount = 1
	};
	VkResult vkRes = vkAllocateDescriptorSets(swapChain->device, &allocInfo, &swapChain->descriptorSet);
	assert(!vkRes);
	// Image descriptor for the color map texture
	VkDescriptorImageInfo texDescriptor = 
	{
		.sampler = swapChain->tex.sampler,
		.imageView = swapChain->tex.view,
		.imageLayout = VK_IMAGE_LAYOUT_GENERAL
	};
	VkWriteDescriptorSet writeDescriptorSets[] =
	{
		// Binding 0 : Vertex shader uniform buffer
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = VK_NULL_HANDLE,
			.dstSet = swapChain->descriptorSet,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.dstBinding = 0,
			.pBufferInfo = &swapChain->uniformBuffer.descriptor,
			// Default value in all examples
			.descriptorCount = 1
		},
		// Binding 1 : Fragment shader texture sampler
		{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.pNext = VK_NULL_HANDLE,
			.dstSet = swapChain->descriptorSet,
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.dstBinding = 1,
			.pImageInfo = &texDescriptor,
			// Default value in all examples
			.descriptorCount = 1
		}
	};
	vkUpdateDescriptorSets(swapChain->device, SDL_arraysize(writeDescriptorSets), writeDescriptorSets, 0, VK_NULL_HANDLE);
	return SGL_TRUE;
}

U32 SGL_BuildCommandBuffers(SGL_VkSwapChain * swapChain)
{
	VkCommandBufferBeginInfo cmdBufInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = VK_NULL_HANDLE
	};

	VkClearValue clearValues[2] = 
	{
		{
			.color = { 0.025f, 0.025f, 0.025f, 1.0f }
		},
		{
			.depthStencil = { 1.0f, 0.0f }
		}
	};

	VkRenderPassBeginInfo renderPassBeginInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = VK_NULL_HANDLE,
		.renderPass = swapChain->renderPass,
		.renderArea.offset.x = 0,
		.renderArea.offset.y = 0,
		.renderArea.extent.width = swapChain->windowSize.x,
		.renderArea.extent.height = swapChain->windowSize.y,
		.clearValueCount = 2,
		.pClearValues = clearValues
	};

	VkResult err;

	for (U32 i = 0; i < swapChain->imageCount; ++i)
	{
		// Set target frame buffer
		renderPassBeginInfo.framebuffer = swapChain->frameBuffers[i];

		err = vkBeginCommandBuffer(swapChain->drawCmdBuffers[i], &cmdBufInfo);
		assert(!err);

		vkCmdBeginRenderPass(swapChain->drawCmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		VkViewport viewport = 
		{
			.width = (float)swapChain->windowSize.x,
			.height = (float)swapChain->windowSize.y,
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(swapChain->drawCmdBuffers[i], 0, 1, &viewport);

		VkRect2D scissor = 
		{
			swapChain->windowSize.x,
			swapChain->windowSize.y,
			0,
			0
		};
		vkCmdSetScissor(swapChain->drawCmdBuffers[i], 0, 1, &scissor);

		vkCmdBindDescriptorSets(swapChain->drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, swapChain->pipelineLayout, 0, 1, &swapChain->descriptorSet, 0, NULL);
		vkCmdBindPipeline(swapChain->drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, swapChain->mainPipeline);

		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(swapChain->drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, &swapChain->vertexBuffer.buf, offsets);
		vkCmdBindIndexBuffer(swapChain->drawCmdBuffers[i], swapChain->indexBuffer.buf, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(swapChain->drawCmdBuffers[i], swapChain->indexBuffer.count, 1, 0, 0, 0);

		vkCmdEndRenderPass(swapChain->drawCmdBuffers[i]);

		VkImageMemoryBarrier prePresentBarrier =
		{
			// Some default values
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.pNext = NULL,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = 0,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
			//And image
			.image = swapChain->buffers[i].image
		};
		vkCmdPipelineBarrier(
			swapChain->drawCmdBuffers[i],
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0,
			0, VK_NULL_HANDLE,
			0, VK_NULL_HANDLE,
			1, &prePresentBarrier);

		err = vkEndCommandBuffer(swapChain->drawCmdBuffers[i]);
		assert(!err);
	}
	swapChain->prepared = SGL_TRUE;
	return SGL_TRUE;
}
U32 SGL_PrepareTexture(SGL_VkSwapChain * swapChain, SGL_Texture* tex, VkImageTiling tiling, VkImageUsageFlags usage, VkFlags requiredProps)
{
	VkResult err;
	SDL_assert(tex->surf);
	const VkImageCreateInfo image_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = tex->format,
		.extent = { tex->w, tex->h, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = tiling,
		.usage = usage,
		.flags = 0,
		.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED
	};

	VkMemoryRequirements memReqs;

	err =
		vkCreateImage(swapChain->device, &image_create_info, NULL, &tex->image);
	assert(!err);

	vkGetImageMemoryRequirements(swapChain->device, tex->image, &memReqs);
	VkMemoryAllocateInfo memAllocInfo =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.allocationSize = memReqs.size,
		.memoryTypeIndex = 0
	};
	SDL_assert(GetMemoryType(&swapChain->memoryProperties, memReqs.memoryTypeBits, requiredProps, &memAllocInfo.memoryTypeIndex));

	/* allocate memory */
	err = vkAllocateMemory(swapChain->device, &memAllocInfo, NULL,
		&tex->deviceMem);
	SDL_assert(!err);

	/* bind memory */
	err = vkBindImageMemory(swapChain->device, tex->image, tex->deviceMem, 0);
	SDL_assert(!err);

	if (requiredProps & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
	{
		const VkImageSubresource subres = 
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.arrayLayer = 0,
		};
		VkSubresourceLayout layout;
		void *data;

		vkGetImageSubresourceLayout(swapChain->device, tex->image, &subres,
			&layout);

		err = vkMapMemory(swapChain->device, tex->deviceMem, 0,
			memAllocInfo.allocationSize, 0, &data);
		SDL_assert(!err);

		SDL_memcpy(data, tex->surf->pixels, tex->w*tex->h*tex->surf->format->BytesPerPixel);
		//if (!loadTexture(filename, data, &layout, &tex_width, &tex_height)) {
		//	fprintf(stderr, "Error loading texture: %s\n", filename);
		//}

		vkUnmapMemory(swapChain->device, tex->deviceMem);
	}

	tex->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	SetImageLayout(swapChain->setupCmdBuffer, tex->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, tex->imageLayout);

	return SGL_TRUE;
}
U32 SGL_PrepareTextureToOptimalTiling(SGL_VkSwapChain * swapChain, SGL_Texture* tex)
{
	SGL_Texture stagingTexture =
	{
		.format = tex->format,
		.w = tex->w,
		.h = tex->h,
		.mipLevels = 1,
		.surf = tex->surf
	};

	SGL_PrepareTexture(swapChain, &stagingTexture, VK_IMAGE_TILING_LINEAR, VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	SGL_PrepareTexture(swapChain, tex, VK_IMAGE_TILING_OPTIMAL, (VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	SetImageLayout(swapChain->setupCmdBuffer, stagingTexture.image, VK_IMAGE_ASPECT_COLOR_BIT, stagingTexture.imageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	SetImageLayout(swapChain->setupCmdBuffer, tex->image, VK_IMAGE_ASPECT_COLOR_BIT, tex->imageLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkImageCopy copyRegion = 
	{
		.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		.srcOffset = { 0, 0, 0 },
		.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		.dstOffset = { 0, 0, 0 },
		.extent = { stagingTexture.w, stagingTexture.h, 1 },
	};

	vkCmdCopyImage(swapChain->setupCmdBuffer, stagingTexture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, tex->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
	SetImageLayout(swapChain->setupCmdBuffer, tex->image, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, tex->imageLayout);
	FlushSetupCommandBuffer(swapChain);
	CreateSetupCommandBuffer(swapChain);
	const VkSamplerCreateInfo sampler = 
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = NULL,
		.magFilter = VK_FILTER_NEAREST,
		.minFilter = VK_FILTER_NEAREST,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		.mipLodBias = 0.0f,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = 1,
		.compareOp = VK_COMPARE_OP_NEVER,
		.minLod = 0.0f,
		.maxLod = 0.0f,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
		.unnormalizedCoordinates = VK_FALSE,
	};

	VkImageViewCreateInfo view = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.image = VK_NULL_HANDLE,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = tex->format,
		.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
		.flags = 0,
	};
	VkResult err;
	/* create sampler */
	err = vkCreateSampler(swapChain->device, &sampler, NULL, &tex->sampler);
	assert(!err);

	/* create image view */
	view.image = tex->image;
	err = vkCreateImageView(swapChain->device, &view, NULL, &tex->view);
	assert(!err);
	return SGL_TRUE;
}

U32 SGL_PrepareTextureTest(SGL_VkSwapChain * swapChain, SGL_Texture * tex)
{
	VkFormatProperties formatProperties;
	VkResult err;

	assert(tex);
	// Get device properites for the requested texture format
	vkGetPhysicalDeviceFormatProperties(swapChain->gpu, tex->format, &formatProperties);

	// Only use linear tiling if forced
	//*

	VkImageCreateInfo imageCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = tex->format,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_LINEAR,
		.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.flags = 0,
		.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
		.extent = { tex->w, tex->h, 1 }
	};
	VkMemoryAllocateInfo memAllocInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.allocationSize = 0,
		.memoryTypeIndex = 0
	};
	VkMemoryRequirements memReqs;
	{
		// Load all available mip levels into linear textures
		// and copy to optimal tiling target
		typedef struct _MipLevel 
		{
			VkImage image;
			VkDeviceMemory memory;
		} MipLevel;
		MipLevel* mipLevels = SDL_malloc(tex->mipLevels*sizeof(MipLevel));
		// Copy mip levels
		for (uint32_t level = 0; level < tex->mipLevels; ++level)
		{
			imageCreateInfo.extent.width = tex[level].w;
			imageCreateInfo.extent.height = tex[level].h;
			imageCreateInfo.extent.depth = 1;

			err = vkCreateImage(swapChain->device, &imageCreateInfo, VK_NULL_HANDLE, &mipLevels[level].image);
			assert(!err);

			vkGetImageMemoryRequirements(swapChain->device, mipLevels[level].image, &memReqs);
			memAllocInfo.allocationSize = memReqs.size;
			GetMemoryType(&swapChain->memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
			err = vkAllocateMemory(swapChain->device, &memAllocInfo, VK_NULL_HANDLE, &mipLevels[level].memory);
			assert(!err);
			err = vkBindImageMemory(swapChain->device, mipLevels[level].image, mipLevels[level].memory, 0);
			assert(!err);

			VkImageSubresource subRes = 
			{
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			};

			VkSubresourceLayout subResLayout;
			void *data;

			vkGetImageSubresourceLayout(swapChain->device, mipLevels[level].image, &subRes, &subResLayout);
			assert(!err);
			err = vkMapMemory(swapChain->device, mipLevels[level].memory, 0, memReqs.size, 0, &data);
			assert(!err);
			memcpy(data, tex[level].surf->pixels, tex[level].surf->format->BytesPerPixel * tex->w * tex->h);
			vkUnmapMemory(swapChain->device, mipLevels[level].memory);

			// Image barrier for linear image (base)
			// Linear image will be used as a source for the copy
			SetImageLayout(
				swapChain->setupCmdBuffer,
				mipLevels[level].image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_PREINITIALIZED,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
		}

		// Setup texture as blit target with optimal tiling
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.mipLevels = tex->mipLevels;
		imageCreateInfo.extent.width = tex->w;
		imageCreateInfo.extent.height = tex->h;
		imageCreateInfo.extent.depth = 1;

		err = vkCreateImage(swapChain->device, &imageCreateInfo, VK_NULL_HANDLE, &tex->image);
		assert(!err);

		vkGetImageMemoryRequirements(swapChain->device, tex->image, &memReqs);

		memAllocInfo.allocationSize = memReqs.size;

		GetMemoryType(&swapChain->memoryProperties, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memAllocInfo.memoryTypeIndex);
		err = vkAllocateMemory(swapChain->device, &memAllocInfo, VK_NULL_HANDLE, &tex->deviceMem);
		assert(!err);
		err = vkBindImageMemory(swapChain->device, tex->image, tex->deviceMem, 0);
		assert(!err);

		// Image barrier for optimal image (target)
		// Optimal image will be used as destination for the copy
		SetImageLayout(
			swapChain->setupCmdBuffer,
			tex->image,
			VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Copy mip levels one by one
		for (uint32_t level = 0; level < tex->mipLevels; ++level)
		{
			// Copy region for image blit
			VkImageCopy copyRegion = 
			{
				.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.srcSubresource.baseArrayLayer = 0,
				.srcSubresource.mipLevel = 0,
				.srcSubresource.layerCount = 1,
				.srcOffset = { 0, 0, 0 },
			};

			copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.dstSubresource.baseArrayLayer = 0;
			// Set mip level to copy the linear image to
			copyRegion.dstSubresource.mipLevel = level;
			copyRegion.dstSubresource.layerCount = 1;
			copyRegion.dstOffset.x = 0;
			copyRegion.dstOffset.y = 0;
			copyRegion.dstOffset.z = 0;

			copyRegion.extent.width = tex[level].w;
			copyRegion.extent.height = tex[level].h;
			copyRegion.extent.depth = 1;

			// Put image copy into command buffer
			vkCmdCopyImage(
				swapChain->setupCmdBuffer,
				mipLevels[level].image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				tex->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &copyRegion);

			// Change texture image layout to shader read after the copy
			tex->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			SetImageLayout(
				swapChain->setupCmdBuffer,
				tex->image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				tex->imageLayout);
		}

		FlushSetupCommandBuffer(swapChain);
		CreateSetupCommandBuffer(swapChain);

		// Clean up linear images 
		// No longer required after mip levels
		// have been transformed over to optimal tiling
		for (size_t i = 0; i < tex->mipLevels; i++)
		{
			vkDestroyImage(swapChain->device, mipLevels[i].image, VK_NULL_HANDLE);
			vkFreeMemory(swapChain->device, mipLevels[i].memory, VK_NULL_HANDLE);
		}
	}
	// Create sampler
	// In Vulkan textures are accessed by samplers
	// This separates all the sampling information from the 
	// texture data
	// This means you could have multiple sampler objects
	// for the same texture with different settings
	// Similar to the samplers available with OpenGL 3.3
	VkSamplerCreateInfo sampler = 
	{
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = VK_NULL_HANDLE,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.mipLodBias = 0.0f,
		.compareOp = VK_COMPARE_OP_NEVER,
		.minLod = 0.0f,
		// Max level-of-detail should match mip level count
		.maxLod = tex->mipLevels,
		// Enable anisotropic filtering
		.maxAnisotropy = 8,
		.anisotropyEnable = VK_TRUE,
		.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
	};
	err = vkCreateSampler(swapChain->device, &sampler, VK_NULL_HANDLE, &tex->sampler);
	assert(!err);

	// Create image view
	// Textures are not directly accessed by the shaders and
	// are abstracted by image views containing additional
	// information and sub resource ranges
	VkImageViewCreateInfo view = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.image = tex->image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = tex->format,
		.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		// Linear tiling usually won't support mip maps
		// Only set mip map count if optimal tiling is used
		.subresourceRange.levelCount = tex->mipLevels,
	};
	err = vkCreateImageView(swapChain->device, &view, VK_NULL_HANDLE, &tex->view);
	assert(!err);
	//*/
	return SGL_TRUE;
}
