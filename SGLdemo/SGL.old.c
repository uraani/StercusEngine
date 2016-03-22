static const float g_vertex_buffer_data[] = 
{
	-1.0f,-1.0f,-1.0f,  // -X side
	-1.0f,-1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,

	-1.0f,-1.0f,-1.0f,  // -Z side
	1.0f, 1.0f,-1.0f,
	1.0f,-1.0f,-1.0f,
	-1.0f,-1.0f,-1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,

	-1.0f,-1.0f,-1.0f,  // -Y side
	1.0f,-1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f,-1.0f,
	1.0f,-1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,

	-1.0f, 1.0f,-1.0f,  // +Y side
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f,-1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f,-1.0f,

	1.0f, 1.0f,-1.0f,  // +X side
	1.0f, 1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f,-1.0f,-1.0f,
	1.0f, 1.0f,-1.0f,

	-1.0f, 1.0f, 1.0f,  // +Z side
	-1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f,-1.0f, 1.0f,
	1.0f,-1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
};

static const float g_uv_buffer_data[] = {
	0.0f, 0.0f,  // -X side
	1.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,
	0.0f, 0.0f,

	1.0f, 0.0f,  // -Z side
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	1.0f, 1.0f,  // -Y side
	1.0f, 0.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	0.0f, 1.0f,

	1.0f, 1.0f,  // +Y side
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,

	1.0f, 1.0f,  // +X side
	0.0f, 1.0f,
	0.0f, 0.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,

	0.0f, 1.0f,  // +Z side
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f,
};
static void demo_flush_init_cmd(SGL_VkContext* vkContext) {
	VkResult U_ASSERT_ONLY err;

	if (vkContext->cmd == VK_NULL_HANDLE)
		return;

	err = vkEndCommandBuffer(vkContext->cmd);
	assert(!err);

	const VkCommandBuffer cmd_bufs[] = { vkContext->cmd };
	VkFence nullFence = VK_NULL_HANDLE;
	VkSubmitInfo submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 0,
		.pWaitSemaphores = NULL,
		.pWaitDstStageMask = NULL,
		.commandBufferCount = 1,
		.pCommandBuffers = cmd_bufs,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL };

	err = vkQueueSubmit(vkContext->queue, 1, &submit_info, nullFence);
	assert(!err);

	err = vkQueueWaitIdle(vkContext->queue);
	assert(!err);

	vkFreeCommandBuffers(vkContext->device, vkContext->cmd_pool, 1, cmd_bufs);
	vkContext->cmd = VK_NULL_HANDLE;
}
static void demo_set_image_layout(SGL_VkContext* vkContext, VkImage image,
	VkImageAspectFlags aspectMask,
	VkImageLayout old_image_layout,
	VkImageLayout new_image_layout) 
{
	VkResult U_ASSERT_ONLY err;

	if (vkContext->cmd == VK_NULL_HANDLE) {
		const VkCommandBufferAllocateInfo cmd = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = NULL,
			.commandPool = vkContext->cmd_pool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		err = vkAllocateCommandBuffers(vkContext->device, &cmd, &vkContext->cmd);
		assert(!err);

		VkCommandBufferInheritanceInfo cmd_buf_hinfo = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
			.pNext = NULL,
			.renderPass = VK_NULL_HANDLE,
			.subpass = 0,
			.framebuffer = VK_NULL_HANDLE,
			.occlusionQueryEnable = VK_FALSE,
			.queryFlags = 0,
			.pipelineStatistics = 0,
		};
		VkCommandBufferBeginInfo cmd_buf_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.pNext = NULL,
			.flags = 0,
			.pInheritanceInfo = &cmd_buf_hinfo,
		};
		err = vkBeginCommandBuffer(vkContext->cmd, &cmd_buf_info);
		assert(!err);
	}

	VkImageMemoryBarrier image_memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = 0,
		.dstAccessMask = 0,
		.oldLayout = old_image_layout,
		.newLayout = new_image_layout,
		.image = image,
		.subresourceRange = { aspectMask, 0, 1, 0, 1 } };

	if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		/* Make sure anything that was copying from this image has completed */
		image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	}

	if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		/* Make sure any Copy or CPU writes to image are flushed */
		image_memory_barrier.dstAccessMask =
			VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	}

	VkImageMemoryBarrier *pmemory_barrier = &image_memory_barrier;

	VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

	vkCmdPipelineBarrier(vkContext->cmd, src_stages, dest_stages, 0, 0, NULL, 0,
		NULL, 1, pmemory_barrier);
	U32 asd = 0;
}
static VkShaderModule
demo_prepare_shader_module(SGL_VkContext* vkContext, const void *code, size_t size) {
	VkShaderModule module;
	VkShaderModuleCreateInfo moduleCreateInfo;
	VkResult U_ASSERT_ONLY err;

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;

	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = code;
	moduleCreateInfo.flags = 0;
	err = vkCreateShaderModule(vkContext->device, &moduleCreateInfo, NULL, &module);
	assert(!err);

	return module;
}

char* demo_read_spv(const char *filename, size_t *psize) {
	size_t size;
	size_t U_ASSERT_ONLY retval;
	void *shader_code;

	SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
	//FILE *fp = fopen(filename, "rb");
	//if (!fp)
	//	return NULL;
	size = SDL_RWseek(rw, 0, RW_SEEK_END);
	//fseek(fp, 0L, SEEK_END);
	//size = ftell(fp);
	SDL_RWseek(rw, 0, RW_SEEK_SET);
	//fseek(fp, 0L, SEEK_SET);

	shader_code = malloc(size);
	retval = SDL_RWread(rw, shader_code, size, 1);
	//retval = fread(shader_code, size, 1, fp);
	SDL_assert(retval == 1);

	*psize = size;

	SDL_RWclose(rw);
	//fclose(fp);
	return shader_code;
}

static VkShaderModule demo_prepare_vs(SGL_VkContext* vkContext) {
	void *vertShaderCode;
	size_t size;

	vertShaderCode = demo_read_spv("cube-vert.spv", &size);

	vkContext->vert_shader_module =
		demo_prepare_shader_module(vkContext, vertShaderCode, size);

	free(vertShaderCode);

	return vkContext->vert_shader_module;
}
static VkShaderModule demo_prepare_fs(SGL_VkContext* vkContext) {
	void *fragShaderCode;
	size_t size;

	fragShaderCode = demo_read_spv("cube-frag.spv", &size);

	vkContext->frag_shader_module =
		demo_prepare_shader_module(vkContext, fragShaderCode, size);

	free(fragShaderCode);

	return vkContext->frag_shader_module;
}
static void demo_draw_build_cmd(SGL_VkContext* vkContext, VkCommandBuffer cmd_buf) {
	VkCommandBufferInheritanceInfo cmd_buf_hinfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		.pNext = NULL,
		.renderPass = VK_NULL_HANDLE,
		.subpass = 0,
		.framebuffer = VK_NULL_HANDLE,
		.occlusionQueryEnable = VK_FALSE,
		.queryFlags = 0,
		.pipelineStatistics = 0,
	};
	const VkCommandBufferBeginInfo cmd_buf_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = NULL,
		.flags = 0,
		.pInheritanceInfo = &cmd_buf_hinfo,
	};
	const VkClearValue clear_values[2] = {
		[0] = { .color.float32 = { 0.2f, 0.2f, 0.2f, 0.2f } },
		[1] = { .depthStencil = { 1.0f, 0 } },
	};
	const VkRenderPassBeginInfo rp_begin = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = vkContext->render_pass,
		.framebuffer = vkContext->framebuffers[vkContext->current_buffer],
		.renderArea.offset.x = 0,
		.renderArea.offset.y = 0,
		.renderArea.extent.width = vkContext->width,
		.renderArea.extent.height = vkContext->height,
		.clearValueCount = 2,
		.pClearValues = clear_values,
	};
	VkResult U_ASSERT_ONLY err;

	err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
	assert(!err);

	vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, vkContext->pipeline);
	vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
		vkContext->pipeline_layout, 0, 1, &vkContext->desc_set, 0,
		NULL);

	VkViewport viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.height = (float)vkContext->height;
	viewport.width = (float)vkContext->width;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = vkContext->width;
	scissor.extent.height = vkContext->height;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

	vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);
	vkCmdEndRenderPass(cmd_buf);

	VkImageMemoryBarrier prePresentBarrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } };

	prePresentBarrier.image = vkContext->buffers[vkContext->current_buffer].image;
	VkImageMemoryBarrier *pmemory_barrier = &prePresentBarrier;
	vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
		NULL, 1, pmemory_barrier);

	err = vkEndCommandBuffer(cmd_buf);
	assert(!err);
}
static void demo_prepare_buffers(SGL_VkContext* vkContext) {
	VkResult U_ASSERT_ONLY err;
	VkSwapchainKHR oldSwapchain = vkContext->swapchain;

	// Check the surface capabilities and formats
	VkSurfaceCapabilitiesKHR surfCapabilities;
	err = vkContext->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(
		vkContext->gpu, vkContext->surface, &surfCapabilities);
	assert(!err);

	uint32_t presentModeCount;
	err = vkContext->fpGetPhysicalDeviceSurfacePresentModesKHR(
		vkContext->gpu, vkContext->surface, &presentModeCount, NULL);
	assert(!err);
	VkPresentModeKHR *presentModes =
		(VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
	assert(presentModes);
	err = vkContext->fpGetPhysicalDeviceSurfacePresentModesKHR(
		vkContext->gpu, vkContext->surface, &presentModeCount, presentModes);
	assert(!err);

	VkExtent2D swapchainExtent;
	// width and height are either both -1, or both not -1.
	if (surfCapabilities.currentExtent.width == (uint32_t)-1) {
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = vkContext->width;
		swapchainExtent.height = vkContext->height;
	}
	else {
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCapabilities.currentExtent;
		vkContext->width = surfCapabilities.currentExtent.width;
		vkContext->height = surfCapabilities.currentExtent.height;
	}

	// If mailbox mode is available, use it, as is the lowest-latency non-
	// tearing mode.  If not, try IMMEDIATE which will usually be available,
	// and is fastest (though it tears).  If not, fall back to FIFO which is
	// always available.
	VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (size_t i = 0; i < presentModeCount; i++) {
		if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
			swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
		if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
			(presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
			swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		}
	}

	// Determine the number of VkImage's to use in the swap chain (we desire to
	// own only 1 image at a time, besides the images being displayed and
	// queued for display):
	uint32_t desiredNumberOfSwapchainImages =
		surfCapabilities.minImageCount + 1;
	if ((surfCapabilities.maxImageCount > 0) &&
		(desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount)) {
		// Application must settle for fewer images than desired:
		desiredNumberOfSwapchainImages = surfCapabilities.maxImageCount;
	}

	VkSurfaceTransformFlagsKHR preTransform;
	if (surfCapabilities.supportedTransforms &
		VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
		preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else {
		preTransform = surfCapabilities.currentTransform;
	}

	const VkSwapchainCreateInfoKHR swapchain = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.surface = vkContext->surface,
		.minImageCount = desiredNumberOfSwapchainImages,
		.imageFormat = vkContext->format,
		.imageColorSpace = vkContext->color_space,
		.imageExtent =
	{
		.width = swapchainExtent.width,.height = swapchainExtent.height,
	},
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.preTransform = preTransform,
		.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		.imageArrayLayers = 1,
		.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 0,
		.pQueueFamilyIndices = NULL,
		.presentMode = swapchainPresentMode,
		.oldSwapchain = oldSwapchain,
		.clipped = SGL_TRUE,
	};
	uint32_t i;

	err = vkContext->fpCreateSwapchainKHR(vkContext->device, &swapchain, NULL,
		&vkContext->swapchain);
	assert(!err);

	// If we just re-created an existing swapchain, we should destroy the old
	// swapchain at this point.
	// Note: destroying the swapchain also cleans up all its associated
	// presentable images once the platform is done with them.
	if (oldSwapchain != VK_NULL_HANDLE) {
		vkContext->fpDestroySwapchainKHR(vkContext->device, oldSwapchain, NULL);
	}

	err = vkContext->fpGetSwapchainImagesKHR(vkContext->device, vkContext->swapchain,
		&vkContext->swapchainImageCount, NULL);
	assert(!err);

	VkImage *swapchainImages =
		(VkImage *)malloc(vkContext->swapchainImageCount * sizeof(VkImage));
	assert(swapchainImages);
	err = vkContext->fpGetSwapchainImagesKHR(vkContext->device, vkContext->swapchain,
		&vkContext->swapchainImageCount,
		swapchainImages);
	assert(!err);

	vkContext->buffers = (SwapchainImage *)malloc(sizeof(SwapchainImage) *
		vkContext->swapchainImageCount);
	assert(vkContext->buffers);

	for (i = 0; i < vkContext->swapchainImageCount; i++) {
		VkImageViewCreateInfo color_image_view = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.format = vkContext->format,
			.components =
		{
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
			.subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1 },
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.flags = 0,
		};

		vkContext->buffers[i].image = swapchainImages[i];

		// Render loop will expect image to have been used before and in
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		// layout and will change to COLOR_ATTACHMENT_OPTIMAL, so init the image
		// to that state
		demo_set_image_layout(
			vkContext, vkContext->buffers[i].image, VK_IMAGE_ASPECT_COLOR_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

		color_image_view.image = vkContext->buffers[i].image;

		err = vkCreateImageView(vkContext->device, &color_image_view, NULL,
			&vkContext->buffers[i].view);
		assert(!err);
	}

	if (NULL != presentModes) {
		free(presentModes);
	}
}
static SGL_bool memory_type_from_properties(SGL_VkContext* vkContext, uint32_t typeBits,
	VkFlags requirements_mask,
	uint32_t *typeIndex) {
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < 32; i++) {
		if ((typeBits & 1) == 1) {
			// Type is available, does it match user properties?
			if ((vkContext->memory_properties.memoryTypes[i].propertyFlags &
				requirements_mask) == requirements_mask) {
				*typeIndex = i;
				return SGL_TRUE;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return SGL_FALSE;
}
/*SGL_bool loadTexture(const char *filename, uint8_t *rgba_data,
	VkSubresourceLayout *layout, int32_t *width, int32_t *height) {
	FILE *fPtr = fopen(filename, "rb");
	char header[256], *cPtr, *tmp;

	if (!fPtr)
		return SGL_FALSE;

	cPtr = fgets(header, 256, fPtr); // P6
	if (cPtr == NULL || strncmp(header, "P6\n", 3)) {
		fclose(fPtr);
		return SGL_FALSE;
	}

	do {
		cPtr = fgets(header, 256, fPtr);
		if (cPtr == NULL) {
			fclose(fPtr);
			return SGL_FALSE;
		}
	} while (!strncmp(header, "#", 1));

	sscanf(header, "%u %u", height, width);
	if (rgba_data == NULL) {
		fclose(fPtr);
		return SGL_TRUE;
	}
	tmp = fgets(header, 256, fPtr); // Format
	(void)tmp;
	if (cPtr == NULL || strncmp(header, "255\n", 3)) {
		fclose(fPtr);
		return SGL_FALSE;
	}

	for (int y = 0; y < *height; y++) {
		uint8_t *rowPtr = rgba_data;
		for (int x = 0; x < *width; x++) {
			size_t s = fread(rowPtr, 3, 1, fPtr);
			(void)s;
			rowPtr[3] = 255; 
			rowPtr += 4;
		}
		rgba_data += layout->rowPitch;
	}
	fclose(fPtr);
	return SGL_TRUE;
}*/
static void demo_prepare_texture_image(SGL_VkContext* vkContext, const char *filename,
struct texture_object *tex_obj,
	VkImageTiling tiling,
	VkImageUsageFlags usage,
	VkFlags required_props) 
{
	if(tex_obj->surf == NULL)
	{ 
		tex_obj->surf = SGL_DataLoadImage("chimppiVulkant0");
	}
	if (!tex_obj->surf)
	{
		SDL_Log("Failed to load textures\n");
		//fflush(stdout);
		exit(1);
	}
	const VkFormat tex_format = SGL_PixelFormatToVkFormat(tex_obj->surf->format->format);
	//const VkFormat tex_format = VK_FORMAT_R8G8B8A8_UNORM;
	//U32 tex_width;
	//U32 tex_height;
	VkResult U_ASSERT_ONLY err;
	SGL_bool U_ASSERT_ONLY pass;
	//if (!loadTexture(filename, NULL, NULL, &tex_width, &tex_height)) {
	//	printf("Failed to load textures\n");
	//	fflush(stdout);
	//	exit(1);
	//}
	tex_obj->tex_width = tex_obj->surf->w;
	tex_obj->tex_height = tex_obj->surf->h;

	const VkImageCreateInfo image_create_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = tex_format,
		.extent = { tex_obj->surf->w, tex_obj->surf->h, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = tiling,
		.usage = usage,
		.flags = 0,
	};

	VkMemoryRequirements mem_reqs;

	err =
		vkCreateImage(vkContext->device, &image_create_info, NULL, &tex_obj->image);
	assert(!err);

	vkGetImageMemoryRequirements(vkContext->device, tex_obj->image, &mem_reqs);

	tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	tex_obj->mem_alloc.pNext = NULL;
	tex_obj->mem_alloc.allocationSize = mem_reqs.size;
	tex_obj->mem_alloc.memoryTypeIndex = 0;

	pass = memory_type_from_properties(vkContext, mem_reqs.memoryTypeBits,
		required_props,
		&tex_obj->mem_alloc.memoryTypeIndex);
	assert(pass);

	/* allocate memory */
	err = vkAllocateMemory(vkContext->device, &tex_obj->mem_alloc, NULL,
		&(tex_obj->mem));
	assert(!err);

	/* bind memory */
	err = vkBindImageMemory(vkContext->device, tex_obj->image, tex_obj->mem, 0);
	assert(!err);

	if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		const VkImageSubresource subres = {
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.arrayLayer = 0,
		};
		VkSubresourceLayout layout;
		void *data;

		vkGetImageSubresourceLayout(vkContext->device, tex_obj->image, &subres,
			&layout);

		err = vkMapMemory(vkContext->device, tex_obj->mem, 0,
			tex_obj->mem_alloc.allocationSize, 0, &data);
		assert(!err);
		SDL_memcpy(data, tex_obj->surf->pixels, tex_obj->surf->format->BytesPerPixel * tex_obj->surf->w * tex_obj->surf->h);
		//if (!loadTexture(filename, data, &layout, &surf->w, &surf->h)) {
		//	fprintf(stderr, "Error loading texture: %s\n", filename);
		//}
		

		vkUnmapMemory(vkContext->device, tex_obj->mem);
	}

	tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	demo_set_image_layout(vkContext, tex_obj->image, VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED, tex_obj->imageLayout);
	/* setting the image layout does not reference the actual memory so no need
	* to add a mem ref */
}
static void demo_destroy_texture_image(SGL_VkContext* vkContext, struct texture_object *tex_objs)
{
	/* clean up staging resources */
	vkFreeMemory(vkContext->device, tex_objs->mem, NULL);
	vkDestroyImage(vkContext->device, tex_objs->image, NULL);
}

static void demo_prepare_textures(SGL_VkContext* vkContext) {
	const VkFormat tex_format = VK_FORMAT_B8G8R8A8_UNORM;
	VkFormatProperties props;
	uint32_t i;

	vkGetPhysicalDeviceFormatProperties(vkContext->gpu, tex_format, &props);

	for (i = 0; i < DEMO_TEXTURE_COUNT; i++) 
	{
		VkResult U_ASSERT_ONLY err;

		if ((props.linearTilingFeatures &
			VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) &&
			!vkContext->use_staging_buffer) {
			/* Device can texture using linear textures */
			demo_prepare_texture_image(vkContext, tex_files[i], &vkContext->textures[i],
				VK_IMAGE_TILING_LINEAR,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		}
		else if (props.optimalTilingFeatures &
			VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) {
			/* Must use staging buffer to copy linear texture to optimized */
			struct texture_object staging_texture;

			memset(&staging_texture, 0, sizeof(staging_texture));
			demo_prepare_texture_image(vkContext, tex_files[i], &staging_texture,
				VK_IMAGE_TILING_LINEAR,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			demo_prepare_texture_image(
				vkContext, tex_files[i], &vkContext->textures[i], VK_IMAGE_TILING_OPTIMAL,
				(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			demo_set_image_layout(vkContext, staging_texture.image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				staging_texture.imageLayout,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

			demo_set_image_layout(vkContext, vkContext->textures[i].image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				vkContext->textures[i].imageLayout,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkImageCopy copy_region = {
				.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
				.srcOffset = { 0, 0, 0 },
				.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
				.dstOffset = { 0, 0, 0 },
				.extent = { staging_texture.tex_width,
				staging_texture.tex_height, 1 },
			};
			vkCmdCopyImage(
				vkContext->cmd, staging_texture.image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vkContext->textures[i].image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

			demo_set_image_layout(vkContext, vkContext->textures[i].image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				vkContext->textures[i].imageLayout);

			demo_flush_init_cmd(vkContext);

			demo_destroy_texture_image(vkContext, &staging_texture);
		}
		else {
			/* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
			assert(!"No support for R8G8B8A8_UNORM as texture image format");
		}

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

		VkImageViewCreateInfo view = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.image = VK_NULL_HANDLE,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = tex_format,
			.components =
		{
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
		},
			.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
			.flags = 0,
		};

		/* create sampler */
		err = vkCreateSampler(vkContext->device, &sampler, NULL,
			&vkContext->textures[i].sampler);
		assert(!err);

		/* create image view */
		view.image = vkContext->textures[i].image;
		err = vkCreateImageView(vkContext->device, &view, NULL,
			&vkContext->textures[i].view);
		assert(!err);
	}
}
void demo_prepare_cube_data_buffer(SGL_VkContext* vkContext) {
	VkBufferCreateInfo buf_info;
	VkMemoryRequirements mem_reqs;
	U8 *pData;
	int i;
	SGL_Mat4 MVP, VP;
	VkResult U_ASSERT_ONLY err;
	SGL_bool U_ASSERT_ONLY pass;
	struct vktexcube_vs_uniform data;

	VP = SM_M4Multiply(&vkContext->view_matrix, &vkContext->projection_matrix);
	MVP = SM_M4Multiply(&vkContext->model_matrix, &VP);
	memcpy(data.mvp, &MVP, sizeof(MVP));

	for (i = 0; i < 12 * 3; i++) {
		data.position[i][0] = g_vertex_buffer_data[i * 3];
		data.position[i][1] = g_vertex_buffer_data[i * 3 + 1];
		data.position[i][2] = g_vertex_buffer_data[i * 3 + 2];
		data.position[i][3] = 1.0f;
		data.attr[i][0] = g_uv_buffer_data[2 * i];
		data.attr[i][1] = g_uv_buffer_data[2 * i + 1];
		data.attr[i][2] = 0;
		data.attr[i][3] = 0;
	}

	memset(&buf_info, 0, sizeof(buf_info));
	buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buf_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	buf_info.size = sizeof(data);
	err =
		vkCreateBuffer(vkContext->device, &buf_info, NULL, &vkContext->uniform_data.buf);
	assert(!err);

	vkGetBufferMemoryRequirements(vkContext->device, vkContext->uniform_data.buf,
		&mem_reqs);

	vkContext->uniform_data.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkContext->uniform_data.mem_alloc.pNext = NULL;
	vkContext->uniform_data.mem_alloc.allocationSize = mem_reqs.size;
	vkContext->uniform_data.mem_alloc.memoryTypeIndex = 0;

	pass = memory_type_from_properties(
		vkContext, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		&vkContext->uniform_data.mem_alloc.memoryTypeIndex);
	assert(pass);

	err = vkAllocateMemory(vkContext->device, &vkContext->uniform_data.mem_alloc, NULL,
		&(vkContext->uniform_data.mem));
	assert(!err);

	err = vkMapMemory(vkContext->device, vkContext->uniform_data.mem, 0,
		vkContext->uniform_data.mem_alloc.allocationSize, 0,
		(void **)&pData);
	assert(!err);

	memcpy(pData, &data, sizeof data);

	vkUnmapMemory(vkContext->device, vkContext->uniform_data.mem);

	err = vkBindBufferMemory(vkContext->device, vkContext->uniform_data.buf,
		vkContext->uniform_data.mem, 0);
	assert(!err);

	vkContext->uniform_data.buffer_info.buffer = vkContext->uniform_data.buf;
	vkContext->uniform_data.buffer_info.offset = 0;
	vkContext->uniform_data.buffer_info.range = sizeof(data);
}
static void demo_prepare_render_pass(SGL_VkContext* vkContext) {
	const VkAttachmentDescription attachments[2] = {
		[0] =
	{
		.format = vkContext->format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	},
		[1] =
	{
		.format = vkContext->depth.format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.finalLayout =
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	},
	};
	const VkAttachmentReference color_reference = {
		.attachment = 0,.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	};
	const VkAttachmentReference depth_reference = {
		.attachment = 1,
		.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};
	const VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.flags = 0,
		.inputAttachmentCount = 0,
		.pInputAttachments = NULL,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_reference,
		.pResolveAttachments = NULL,
		.pDepthStencilAttachment = &depth_reference,
		.preserveAttachmentCount = 0,
		.pPreserveAttachments = NULL,
	};
	const VkRenderPassCreateInfo rp_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.pNext = NULL,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 0,
		.pDependencies = NULL,
	};
	VkResult U_ASSERT_ONLY err;

	err = vkCreateRenderPass(vkContext->device, &rp_info, NULL, &vkContext->render_pass);
	assert(!err);
}
static void demo_prepare_pipeline(SGL_VkContext* vkContext) {
	VkGraphicsPipelineCreateInfo pipeline;
	VkPipelineCacheCreateInfo pipelineCache;
	VkPipelineVertexInputStateCreateInfo vi;
	VkPipelineInputAssemblyStateCreateInfo ia;
	VkPipelineRasterizationStateCreateInfo rs;
	VkPipelineColorBlendStateCreateInfo cb;
	VkPipelineDepthStencilStateCreateInfo ds;
	VkPipelineViewportStateCreateInfo vp;
	VkPipelineMultisampleStateCreateInfo ms;
	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo dynamicState;
	VkResult U_ASSERT_ONLY err;

	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
	memset(&dynamicState, 0, sizeof dynamicState);
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables;

	memset(&pipeline, 0, sizeof(pipeline));
	pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline.layout = vkContext->pipeline_layout;

	memset(&vi, 0, sizeof(vi));
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	memset(&ia, 0, sizeof(ia));
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	memset(&rs, 0, sizeof(rs));
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.cullMode = VK_CULL_MODE_BACK_BIT;
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rs.depthClampEnable = VK_FALSE;
	rs.rasterizerDiscardEnable = VK_FALSE;
	rs.depthBiasEnable = VK_FALSE;

	memset(&cb, 0, sizeof(cb));
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	VkPipelineColorBlendAttachmentState att_state[1];
	memset(att_state, 0, sizeof(att_state));
	att_state[0].colorWriteMask = 0xf;
	att_state[0].blendEnable = VK_FALSE;
	cb.attachmentCount = 1;
	cb.pAttachments = att_state;

	memset(&vp, 0, sizeof(vp));
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.viewportCount = 1;
	dynamicStateEnables[dynamicState.dynamicStateCount++] =
		VK_DYNAMIC_STATE_VIEWPORT;
	vp.scissorCount = 1;
	dynamicStateEnables[dynamicState.dynamicStateCount++] =
		VK_DYNAMIC_STATE_SCISSOR;

	memset(&ds, 0, sizeof(ds));
	ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	ds.depthTestEnable = VK_TRUE;
	ds.depthWriteEnable = VK_TRUE;
	ds.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	ds.depthBoundsTestEnable = VK_FALSE;
	ds.back.failOp = VK_STENCIL_OP_KEEP;
	ds.back.passOp = VK_STENCIL_OP_KEEP;
	ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
	ds.stencilTestEnable = VK_FALSE;
	ds.front = ds.back;

	memset(&ms, 0, sizeof(ms));
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.pSampleMask = NULL;
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	// Two stages: vs and fs
	pipeline.stageCount = 2;
	VkPipelineShaderStageCreateInfo shaderStages[2];
	memset(&shaderStages, 0, 2 * sizeof(VkPipelineShaderStageCreateInfo));

	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = demo_prepare_vs(vkContext);
	shaderStages[0].pName = "main";

	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = demo_prepare_fs(vkContext);
	shaderStages[1].pName = "main";

	memset(&pipelineCache, 0, sizeof(pipelineCache));
	pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	err = vkCreatePipelineCache(vkContext->device, &pipelineCache, NULL,
		&vkContext->pipelineCache);
	assert(!err);

	pipeline.pVertexInputState = &vi;
	pipeline.pInputAssemblyState = &ia;
	pipeline.pRasterizationState = &rs;
	pipeline.pColorBlendState = &cb;
	pipeline.pMultisampleState = &ms;
	pipeline.pViewportState = &vp;
	pipeline.pDepthStencilState = &ds;
	pipeline.pStages = shaderStages;
	pipeline.renderPass = vkContext->render_pass;
	pipeline.pDynamicState = &dynamicState;

	pipeline.renderPass = vkContext->render_pass;

	err = vkCreateGraphicsPipelines(vkContext->device, vkContext->pipelineCache, 1,
		&pipeline, NULL, &vkContext->pipeline);
	assert(!err);

	vkDestroyShaderModule(vkContext->device, vkContext->frag_shader_module, NULL);
	vkDestroyShaderModule(vkContext->device, vkContext->vert_shader_module, NULL);
}
static void demo_prepare_descriptor_layout(SGL_VkContext* vkContext) {
	const VkDescriptorSetLayoutBinding layout_bindings[2] = {
		[0] =
	{
		.binding = 0,
		.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.pImmutableSamplers = NULL,
	},
		[1] =
	{
		.binding = 1,
		.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = DEMO_TEXTURE_COUNT,
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.pImmutableSamplers = NULL,
	},
	};
	const VkDescriptorSetLayoutCreateInfo descriptor_layout = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.bindingCount = 2,
		.pBindings = layout_bindings,
	};
	VkResult U_ASSERT_ONLY err;

	err = vkCreateDescriptorSetLayout(vkContext->device, &descriptor_layout, NULL,
		&vkContext->desc_layout);
	assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.setLayoutCount = 1,
		.pSetLayouts = &vkContext->desc_layout,
	};

	err = vkCreatePipelineLayout(vkContext->device, &pPipelineLayoutCreateInfo, NULL,
		&vkContext->pipeline_layout);
	assert(!err);
}
static void demo_prepare_depth(SGL_VkContext* vkContext) 
{
	const VkFormat depth_format = VK_FORMAT_D16_UNORM;
	const VkImageCreateInfo image = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = depth_format,
		.extent = { vkContext->width, vkContext->height, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.flags = 0,
	};

	VkImageViewCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.image = VK_NULL_HANDLE,
		.format = depth_format,
		.subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
		.baseMipLevel = 0,
		.levelCount = 1,
		.baseArrayLayer = 0,
		.layerCount = 1 },
		.flags = 0,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
	};

	VkMemoryRequirements mem_reqs;
	VkResult U_ASSERT_ONLY err;
	SGL_bool U_ASSERT_ONLY pass;

	vkContext->depth.format = depth_format;

	/* create image */
	err = vkCreateImage(vkContext->device, &image, NULL, &vkContext->depth.image);
	assert(!err);

	vkGetImageMemoryRequirements(vkContext->device, vkContext->depth.image, &mem_reqs);
	assert(!err);

	vkContext->depth.mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkContext->depth.mem_alloc.pNext = NULL;
	vkContext->depth.mem_alloc.allocationSize = mem_reqs.size;
	vkContext->depth.mem_alloc.memoryTypeIndex = 0;

	pass = memory_type_from_properties(vkContext, mem_reqs.memoryTypeBits,
		0, /* No requirements */
		&vkContext->depth.mem_alloc.memoryTypeIndex);
	assert(pass);

	/* allocate memory */
	err = vkAllocateMemory(vkContext->device, &vkContext->depth.mem_alloc, NULL,
		&vkContext->depth.mem);
	assert(!err);

	/* bind memory */
	err =
		vkBindImageMemory(vkContext->device, vkContext->depth.image, vkContext->depth.mem, 0);
	assert(!err);

	demo_set_image_layout(vkContext, vkContext->depth.image, VK_IMAGE_ASPECT_DEPTH_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

	/* create image view */
	view.image = vkContext->depth.image;
	err = vkCreateImageView(vkContext->device, &view, NULL, &vkContext->depth.view);
	assert(!err);
}
static void demo_prepare_descriptor_pool(SGL_VkContext *vkContext) {
	const VkDescriptorPoolSize type_counts[2] = {
		[0] =
	{
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.descriptorCount = 1,
	},
		[1] =
	{
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.descriptorCount = DEMO_TEXTURE_COUNT,
	},
	};
	const VkDescriptorPoolCreateInfo descriptor_pool = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = NULL,
		.maxSets = 1,
		.poolSizeCount = 2,
		.pPoolSizes = type_counts,
	};
	VkResult U_ASSERT_ONLY err;

	err = vkCreateDescriptorPool(vkContext->device, &descriptor_pool, NULL,
		&vkContext->desc_pool);
	assert(!err);
}
static void demo_prepare_descriptor_set(SGL_VkContext *vkContext) {
	VkDescriptorImageInfo tex_descs[DEMO_TEXTURE_COUNT];
	VkWriteDescriptorSet writes[2];
	VkResult U_ASSERT_ONLY err;
	uint32_t i;

	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = vkContext->desc_pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &vkContext->desc_layout };
	err = vkAllocateDescriptorSets(vkContext->device, &alloc_info, &vkContext->desc_set);
	assert(!err);

	memset(&tex_descs, 0, sizeof(tex_descs));
	for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
		tex_descs[i].sampler = vkContext->textures[i].sampler;
		tex_descs[i].imageView = vkContext->textures[i].view;
		tex_descs[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	}

	memset(&writes, 0, sizeof(writes));

	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[0].dstSet = vkContext->desc_set;
	writes[0].descriptorCount = 1;
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writes[0].pBufferInfo = &vkContext->uniform_data.buffer_info;

	writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[1].dstSet = vkContext->desc_set;
	writes[1].dstBinding = 1;
	writes[1].descriptorCount = DEMO_TEXTURE_COUNT;
	writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writes[1].pImageInfo = tex_descs;

	vkUpdateDescriptorSets(vkContext->device, 2, writes, 0, NULL);
}

static void demo_prepare_framebuffers(SGL_VkContext *vkContext) {
	VkImageView attachments[2];
	attachments[1] = vkContext->depth.view;

	const VkFramebufferCreateInfo fb_info = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.renderPass = vkContext->render_pass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = vkContext->width,
		.height = vkContext->height,
		.layers = 1,
	};
	VkResult U_ASSERT_ONLY err;
	uint32_t i;

	vkContext->framebuffers = (VkFramebuffer *)malloc(vkContext->swapchainImageCount *
		sizeof(VkFramebuffer));
	assert(vkContext->framebuffers);

	for (i = 0; i < vkContext->swapchainImageCount; i++) {
		attachments[0] = vkContext->buffers[i].view;
		err = vkCreateFramebuffer(vkContext->device, &fb_info, NULL,
			&vkContext->framebuffers[i]);
		assert(!err);
	}
}
static void demo_prepare(SGL_VkContext *vkContext) 
{
	VkResult U_ASSERT_ONLY err;
	vkContext->cmd = VK_NULL_HANDLE;
	const VkCommandPoolCreateInfo cmd_pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.queueFamilyIndex = vkContext->graphics_queue_node_index,
		.flags = 0,
	};
	err = vkCreateCommandPool(vkContext->device, &cmd_pool_info, NULL,
		&vkContext->cmd_pool);
	assert(!err);

	const VkCommandBufferAllocateInfo cmd = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = vkContext->cmd_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	demo_prepare_buffers(vkContext);
	demo_prepare_depth(vkContext);
	demo_prepare_textures(vkContext);
	demo_prepare_cube_data_buffer(vkContext);

	demo_prepare_descriptor_layout(vkContext);
	demo_prepare_render_pass(vkContext);
	demo_prepare_pipeline(vkContext);

	for (uint32_t i = 0; i < vkContext->swapchainImageCount; i++) {
		err =
			vkAllocateCommandBuffers(vkContext->device, &cmd, &vkContext->buffers[i].cmd);
		assert(!err);
	}

	demo_prepare_descriptor_pool(vkContext);
	demo_prepare_descriptor_set(vkContext);

	demo_prepare_framebuffers(vkContext);

	for (uint32_t i = 0; i < vkContext->swapchainImageCount; i++) {
		vkContext->current_buffer = i;
		demo_draw_build_cmd(vkContext, vkContext->buffers[i].cmd);
	}

	/*
	* Prepare functions above may generate pipeline commands
	* that need to be flushed before beginning the render loop.
	*/
	demo_flush_init_cmd(vkContext);

	vkContext->current_buffer = 0;
	vkContext->prepared = SGL_TRUE;
}
static void demo_create_device(SGL_VkContext *vkContext)
{
	VkResult U_ASSERT_ONLY err;
	float queue_priorities[1] = { 0.0 };
	/*    VkStructureType             sType;
	const void*                 pNext;
	VkDeviceQueueCreateFlags    flags;
	uint32_t                    queueFamilyIndex;
	uint32_t                    queueCount;
	const float*                pQueuePriorities;*/
	const VkDeviceQueueCreateInfo queue =
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.queueFamilyIndex = vkContext->graphics_queue_node_index,
		.queueCount = 1,
		.pQueuePriorities = queue_priorities };

	VkDeviceCreateInfo device = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue,
		.enabledLayerCount = vkContext->enabled_layer_count,
		.ppEnabledLayerNames =
		(const char *const *)((vkContext->validate)
			? vkContext->device_validation_layers
			: NULL),
		.enabledExtensionCount = vkContext->enabled_extension_count,
		.ppEnabledExtensionNames = (const char *const *)vkContext->extension_names,
		.pEnabledFeatures =
		NULL, // If specific features are required, pass them in here
	};

	err = vkCreateDevice(vkContext->gpu, &device, NULL, &vkContext->device);
	assert(!err);
}
VKAPI_ATTR VkBool32 VKAPI_CALL
dbgFunc(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserData)
{
	//10 is the maximum characters that int32_t can contain
	//strlen("VK WARNING: [] Code  : ") + '\0' == 25
	const size_t titleLenght = strlen(pLayerPrefix) + 10 + strlen(pLayerPrefix) + 25;
	char *message = (char*)malloc(strlen(pMsg) + titleLenght);
	SDL_strlcpy(message, pMsg, strlen(pMsg));
	char* messageTitle = (char*)malloc(titleLenght);

	if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) 
	{
		SDL_snprintf(messageTitle, titleLenght, "VK ERROR: [%s] Code %d : ", pLayerPrefix, msgCode);
		SDL_snprintf(message, strlen(pMsg) + titleLenght, "%s%s", messageTitle, pMsg);
		SDL_Log(message);
	}
	else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT) 
	{
		// We know that we're submitting queues without fences, ignore this
		// warning
		//if (SDL_strstr(pMsg, "vkQueueSubmit parameter, VkFence fence, is null pointer")) 
		//{
		//	return SGL_FALSE;
		//}
		SDL_snprintf(messageTitle, titleLenght, "VK WARNING: [%s] Code %d : ", pLayerPrefix, msgCode);
		SDL_snprintf(message, strlen(pMsg) + titleLenght, "%s%s", messageTitle, pMsg);
		SDL_Log(message);
	}
	else 
	{
		return SGL_FALSE;
	}
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, messageTitle, message, NULL);
	free(message);
	free(messageTitle);
	/*
	* false indicates that layer should not bail-out of an
	* API call that had validation failures. This may mean that the
	* app dies inside the driver due to invalid parameter(s).
	* That's what would happen without validation layers, so we'll
	* keep that behavior here.
	*/
	return SGL_FALSE;
}
static VkBool32 demo_check_layers(uint32_t check_count, char **check_names, uint32_t layer_count, VkLayerProperties *layers)
{
	for (uint32_t i = 0; i < check_count; i++) {
		VkBool32 found = 0;
		for (uint32_t j = 0; j < layer_count; j++) {
			if (!SDL_strcmp(check_names[i], layers[j].layerName)) {
				found = 1;
				break;
			}
		}
		if (!found) {
			SDL_Log("Cannot find layer: %s\n", check_names[i]);
			return 0;
		}
	}
	return 1;
}
void SGL_Vk_CreateContext(SGL_Window* window)
{
	VkResult err;
	memset(&window->vkContext, 0, sizeof(SGL_VkContext));
	window->vkContext.frameCount = INT32_MAX;
	window->vkContext.validate = SGL_TRUE;
	window->vkContext.use_staging_buffer = SGL_FALSE;
	window->vkContext.use_break = SGL_FALSE;
	window->vkContext.pause = SGL_TRUE;
	char* extension_names[64];
	uint32_t instance_extension_count = 0;
	uint32_t instance_layer_count = 0;
	uint32_t device_validation_layer_count = 0;
	uint32_t enabled_extension_count = 0;
	uint32_t enabled_layer_count = 0;
	window->vkContext.width = window->windowSize.x;
	window->vkContext.height = window->windowSize.y;

	char* instance_validation_layers[] = {
		"VK_LAYER_GOOGLE_threading",      "VK_LAYER_LUNARG_mem_tracker",
		"VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_draw_state",
		"VK_LAYER_LUNARG_param_checker",  "VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
		"VK_LAYER_GOOGLE_unique_objects",
	};

	window->vkContext.device_validation_layers[0] = "VK_LAYER_GOOGLE_threading";
	window->vkContext.device_validation_layers[1] = "VK_LAYER_LUNARG_mem_tracker";
	window->vkContext.device_validation_layers[2] = "VK_LAYER_LUNARG_object_tracker";
	window->vkContext.device_validation_layers[3] = "VK_LAYER_LUNARG_draw_state";
	window->vkContext.device_validation_layers[4] = "VK_LAYER_LUNARG_param_checker";
	window->vkContext.device_validation_layers[5] = "VK_LAYER_LUNARG_swapchain";
	window->vkContext.device_validation_layers[6] = "VK_LAYER_LUNARG_device_limits";
	window->vkContext.device_validation_layers[7] = "VK_LAYER_LUNARG_image";
	window->vkContext.device_validation_layers[8] = "VK_LAYER_GOOGLE_unique_objects";
	device_validation_layer_count = 9;

	/* Look for validation layers */
	VkBool32 validation_found = 0;
	err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
	assert(!err);

	if (instance_layer_count > 0) {
		VkLayerProperties *instance_layers =
			SDL_malloc(sizeof(VkLayerProperties) * instance_layer_count);
		err = vkEnumerateInstanceLayerProperties(&instance_layer_count,
			instance_layers);
		assert(!err);
		if (window->vkContext.validate) {
			validation_found = demo_check_layers(
				SDL_arraysize(instance_validation_layers),
				instance_validation_layers, instance_layer_count,
				instance_layers);

			enabled_layer_count = SDL_arraysize(instance_validation_layers);
		}

		free(instance_layers);
	}

	if (window->vkContext.validate && !validation_found) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure",
			"vkEnumerateInstanceLayerProperties failed to find"
			"required validation layer.\n\n"
			"Please look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			);
	}

	/* Look for instance extensions */
	VkBool32 surfaceExtFound = 0;
	VkBool32 platformSurfaceExtFound = 0;
	SDL_memset(extension_names, 0, sizeof(extension_names));

	err = vkEnumerateInstanceExtensionProperties(
		NULL, &instance_extension_count, NULL);
	assert(!err);

	if (instance_extension_count > 0) 
	{
		VkExtensionProperties *instance_extensions = SDL_malloc(sizeof(VkExtensionProperties) * instance_extension_count);
		err = vkEnumerateInstanceExtensionProperties( NULL, &instance_extension_count, instance_extensions);

		assert(!err);

		for (uint32_t i = 0; i < instance_extension_count; i++) 
		{
			if (!SDL_strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
			{
				surfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
			}
#ifdef _WIN32
			if (!SDL_strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
			{
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#else  // _WIN32
			if (!SDL_strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) 
			{
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
			}
#endif // _WIN32
			if (!SDL_strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instance_extensions[i].extensionName)) 
			{
				if (window->vkContext.validate) {
					extension_names[enabled_extension_count++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
				}
			}
			assert(enabled_extension_count < 64);
		}
		free(instance_extensions);
	}

	if (!surfaceExtFound) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure",
			"vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			);
	}
	if (!platformSurfaceExtFound) 
	{
#ifdef _WIN32
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure",
			"vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			);
#else  // _WIN32
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure"
			"vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			);
#endif // _WIN32
	}
	const VkApplicationInfo app = 
	{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = SDL_GetWindowTitle(window->handle),
		.applicationVersion = 0,
		.pEngineName = "SGL",
		.engineVersion = 0,
		.apiVersion = VK_API_VERSION,
	};
	VkInstanceCreateInfo inst_info = 
	{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.pApplicationInfo = &app,
		.enabledLayerCount = enabled_layer_count,
		.ppEnabledLayerNames =
		(const char *const *)((window->vkContext.validate) ? instance_validation_layers
			: NULL),
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = (const char *const *)extension_names,
	};

	U32 gpu_count;
	err = vkCreateInstance(&inst_info, NULL, &window->vkContext.inst);
	if (err == VK_ERROR_INCOMPATIBLE_DRIVER) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure",
			"Cannot find a compatible Vulkan installable client driver "
			"(ICD).\n\nPlease look at the Getting Started guide for "
			"additional information.\n",
			window->handle
			);
	}
	else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"vkCreateInstance Failure",
			"Cannot find a specified extension library"
			".\nMake sure your layers path is set appropriately.\n",
			window->handle
			);
	}
	else if (err) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, 
			"vkCreateInstance Failure",
			"vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
			"installable client driver (ICD) installed?\nPlease look at "
			"the Getting Started guide for additional information.\n",
			window->handle
			);
	}
	SDL_Log("Vulkan instance created successfully\n");

	/* Make initial call to query gpu_count, then second call for gpu info*/
	err = vkEnumeratePhysicalDevices(window->vkContext.inst, &gpu_count, NULL);
	assert(!err && gpu_count > 0);

	if (gpu_count > 0) 
	{
		VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(window->vkContext.inst, &gpu_count, physical_devices);
		assert(!err);
		/* For cube demo we just grab the first physical device */
		window->vkContext.gpu = physical_devices[0];
		free(physical_devices);
	}
	else 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkEnumeratePhysicalDevices Failure",
			"vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
			"Do you have a compatible Vulkan installable client driver (ICD) "
			"installed?\nPlease look at the Getting Started guide for "
			"additional information.\n",
			window->handle
			);
	}
	SDL_Log("Vulkan found %i accessible GPUs\n", gpu_count);
	/* Look for validation layers */
	validation_found = 0;
	window->vkContext.enabled_layer_count = 0;
	uint32_t device_layer_count = 0;
	err =
		vkEnumerateDeviceLayerProperties(window->vkContext.gpu, &device_layer_count, NULL);
	assert(!err);

	if (device_layer_count > 0) {
		VkLayerProperties *device_layers =
			malloc(sizeof(VkLayerProperties) * device_layer_count);
		err = vkEnumerateDeviceLayerProperties(window->vkContext.gpu, &device_layer_count,
			device_layers);
		assert(!err);

		if (window->vkContext.validate) {
			validation_found = demo_check_layers(device_validation_layer_count,
				window->vkContext.device_validation_layers,
				device_layer_count,
				device_layers);
			window->vkContext.enabled_layer_count = device_validation_layer_count;
		}

		free(device_layers);
	}

	if (window->vkContext.validate && !validation_found) 
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateDevice Failure",
			"vkEnumerateDeviceLayerProperties failed to find"
			"a required validation layer.\n\n"
			"Please look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			);
	}

	/* Look for device extensions */
	uint32_t device_extension_count = 0;
	VkBool32 swapchainExtFound = 0;
	window->vkContext.enabled_extension_count = 0;
	memset(extension_names, 0, sizeof(extension_names));

	err = vkEnumerateDeviceExtensionProperties(window->vkContext.gpu, NULL,
		&device_extension_count, NULL);
	assert(!err);

	if (device_extension_count > 0) {
		VkExtensionProperties *device_extensions =
			malloc(sizeof(VkExtensionProperties) * device_extension_count);
		err = vkEnumerateDeviceExtensionProperties(
			window->vkContext.gpu, NULL, &device_extension_count, device_extensions);
		assert(!err);

		for (uint32_t i = 0; i < device_extension_count; i++) {
			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				device_extensions[i].extensionName)) {
				swapchainExtFound = 1;
				window->vkContext.extension_names[window->vkContext.enabled_extension_count++] =
					VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			}
			assert(window->vkContext.enabled_extension_count < 64);
		}

		free(device_extensions);
	}

	if (!swapchainExtFound) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"vkCreateInstance Failure",
			"vkEnumerateDeviceExtensionProperties failed to find "
			"the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			window->handle
			); 
	}

	if (window->vkContext.validate) {
		window->vkContext.CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(window->vkContext.inst, "vkCreateDebugReportCallbackEXT");
		window->vkContext.DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(window->vkContext.inst, "vkDestroyDebugReportCallbackEXT");
		if (!window->vkContext.CreateDebugReportCallback) 
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"vkGetProcAddr Failure",
				"GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n",
				window->handle
				);
		}
		if (!window->vkContext.DestroyDebugReportCallback) 
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"vkGetProcAddr Failure",
				"GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n",
				window->handle
				);
		}
		window->vkContext.DebugReportMessage = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(window->vkContext.inst, "vkDebugReportMessageEXT");
		if (!window->vkContext.DebugReportMessage) 
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"GetProcAddr: Unable to find vkDebugReportMessageEXT\n",
				"vkGetProcAddr Failure",
				window->handle
				); 
		}

		PFN_vkDebugReportCallbackEXT callback;

		if (!window->vkContext.use_break) 
		{
			callback = dbgFunc;
		}
		else {
			callback = dbgFunc;
			// TODO add a break callback defined locally since there is no
			// longer
			// one included in the loader
		}
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pNext = NULL;
		dbgCreateInfo.pfnCallback = callback;
		dbgCreateInfo.pUserData = NULL;
		dbgCreateInfo.flags =
			VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		err = window->vkContext.CreateDebugReportCallback(window->vkContext.inst, &dbgCreateInfo, NULL,
			&window->vkContext.msg_callback);
		switch (err) {
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"CreateDebugReportCallback: out of host memory\n",
				"CreateDebugReportCallback Failure",
				window->handle
				);
			break;
		default:
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
				"CreateDebugReportCallback Failure",
				"CreateDebugReportCallback: unknown failure\n",
				window->handle
				);
			break;
		}
	}
	vkGetPhysicalDeviceProperties(window->vkContext.gpu, &window->vkContext.gpu_props);

	/* Call with NULL data to get count */
	vkGetPhysicalDeviceQueueFamilyProperties(window->vkContext.gpu, &window->vkContext.queue_count,
		NULL);
	assert(window->vkContext.queue_count >= 1);

	window->vkContext.queue_props = (VkQueueFamilyProperties *)malloc(
		window->vkContext.queue_count * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(window->vkContext.gpu, &window->vkContext.queue_count,
		window->vkContext.queue_props);
	// Find a queue that supports gfx
	uint32_t gfx_queue_idx = 0;
	for (gfx_queue_idx = 0; gfx_queue_idx < window->vkContext.queue_count;
	gfx_queue_idx++) {
		if (window->vkContext.queue_props[gfx_queue_idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;
	}
	assert(gfx_queue_idx < window->vkContext.queue_count);
	// Query fine-grained feature support for this device.
	//  If app has specific feature requirements it should check supported
	//  features based on this query
	VkPhysicalDeviceFeatures physDevFeatures;
	vkGetPhysicalDeviceFeatures(window->vkContext.gpu, &physDevFeatures);

	GET_INSTANCE_PROC_ADDR(window->vkContext.inst, GetPhysicalDeviceSurfaceSupportKHR);
	GET_INSTANCE_PROC_ADDR(window->vkContext.inst, GetPhysicalDeviceSurfaceCapabilitiesKHR);
	GET_INSTANCE_PROC_ADDR(window->vkContext.inst, GetPhysicalDeviceSurfaceFormatsKHR);
	GET_INSTANCE_PROC_ADDR(window->vkContext.inst, GetPhysicalDeviceSurfacePresentModesKHR);
	GET_INSTANCE_PROC_ADDR(window->vkContext.inst, GetSwapchainImagesKHR);
	uint32_t i;
	// Create a WSI surface for the window:
#ifdef _WIN32
	SDL_SysWMinfo info;
	SDL_GetWindowWMInfo(window->handle, &info);
	TCHAR* className = NULL;
	GetClassName(info.info.win.window, className, 256);
	WNDCLASS wce;
	GetClassInfo(GetModuleHandle(NULL), className, &wce);
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.hinstance = wce.hInstance;
	createInfo.hwnd = info.info.win.window;
	window->vkContext.window = createInfo.hwnd;

	err = vkCreateWin32SurfaceKHR(window->vkContext.inst, &createInfo, NULL, &window->vkContext.surface);
	SDL_Log("Vulkan created Win32 surface for window\n");
#else  // _WIN32
	VkXcbSurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.connection = demo->connection;
	createInfo.window = demo->window;

	err = vkCreateXcbSurfaceKHR(demo->inst, &createInfo, NULL, &demo->surface);
#endif // _WIN32

	// Iterate over each queue to learn whether it supports presenting:
	VkBool32 *supportsPresent =
		(VkBool32 *)malloc(window->vkContext.queue_count * sizeof(VkBool32));
	for (i = 0; i < window->vkContext.queue_count; i++) {
		window->vkContext.fpGetPhysicalDeviceSurfaceSupportKHR(window->vkContext.gpu, i, window->vkContext.surface,
			&supportsPresent[i]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (i = 0; i < window->vkContext.queue_count; i++) {
		if ((window->vkContext.queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
			if (graphicsQueueNodeIndex == UINT32_MAX) {
				graphicsQueueNodeIndex = i;
			}

			if (supportsPresent[i] == VK_TRUE) {
				graphicsQueueNodeIndex = i;
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	if (presentQueueNodeIndex == UINT32_MAX) {
		// If didn't find a queue that supports both graphics and present, then
		// find a separate present queue.
		for (uint32_t i = 0; i < window->vkContext.queue_count; ++i) {
			if (supportsPresent[i] == VK_TRUE) {
				presentQueueNodeIndex = i;
				break;
			}
		}
	}
	free(supportsPresent);

	// Generate error if could not find both a graphics and a present queue
	if (graphicsQueueNodeIndex == UINT32_MAX ||
		presentQueueNodeIndex == UINT32_MAX) {
		ERR_EXIT("Could not find a graphics and a present queue\n",
			"Swapchain Initialization Failure");
	}

	// TODO: Add support for separate queues, including presentation,
	//       synchronization, and appropriate tracking for QueueSubmit.
	// NOTE: While it is possible for an application to use a separate graphics
	//       and a present queues, this demo program assumes it is only using
	//       one:
	if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
		ERR_EXIT("Could not find a common graphics and a present queue\n",
			"Swapchain Initialization Failure");
	}

	window->vkContext.graphics_queue_node_index = graphicsQueueNodeIndex;

	demo_create_device(&window->vkContext);
	
	GET_DEVICE_PROC_ADDR(window->vkContext.device, CreateSwapchainKHR);
	GET_DEVICE_PROC_ADDR(window->vkContext.device, DestroySwapchainKHR);
	GET_DEVICE_PROC_ADDR(window->vkContext.device, GetSwapchainImagesKHR);
	GET_DEVICE_PROC_ADDR(window->vkContext.device, AcquireNextImageKHR);
	GET_DEVICE_PROC_ADDR(window->vkContext.device, QueuePresentKHR);

	vkGetDeviceQueue(window->vkContext.device, window->vkContext.graphics_queue_node_index, 0,
		&window->vkContext.queue);

	// Get the list of VkFormat's that are supported:
	uint32_t formatCount;
	err = window->vkContext.fpGetPhysicalDeviceSurfaceFormatsKHR(window->vkContext.gpu, window->vkContext.surface,
		&formatCount, NULL);
	assert(!err);
	VkSurfaceFormatKHR *surfFormats =
		(VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
	err = window->vkContext.fpGetPhysicalDeviceSurfaceFormatsKHR(window->vkContext.gpu, window->vkContext.surface,
		&formatCount, surfFormats);
	assert(!err);
	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) 
	{
		window->vkContext.format = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else {
		assert(formatCount >= 1);
		window->vkContext.format = surfFormats[0].format;
	}
	window->vkContext.color_space = surfFormats[0].colorSpace;

	//demo->quit = SDL_FALSE;
	//demo->curFrame = 0;

	// Get Memory information and properties
	vkGetPhysicalDeviceMemoryProperties(window->vkContext.gpu, &window->vkContext.memory_properties);
	SDL_Log("Vulkan initialized successfully\n\n");
}

SGL_Window SGL_CreateWindow(const char* title, int x, int y, int w, int h, Uint32 SDLflags)
{
#if defined(_WINDOWS)
	SDL_DisplayMode targetMode, closestMode;
	targetMode.w = w;
	targetMode.h = h;
	targetMode.format = 0;  // don't care
	targetMode.refresh_rate = 0; // don't care
	targetMode.driverdata = 0; // initialize to 0
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SGL_Window window;
	window.handle = SDL_CreateWindow
		(
			title,
			x, y,
			closestMode.w, closestMode.h,
			SDLflags
			);
#elif defined(ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion > 3 ? GLMajorVersion : 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SGL_Window* window = SDL_malloc(sizeof(SGL_Window));
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode))
	{
		return NULL;
	}
	window->window = SDL_CreateWindow
		(
			title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			displayMode.w, displayMode.h,
			SDLflags
			);
	window->context = SDL_GL_CreateContext(window->handle);
#endif
	SGL_SetWindowIcon(&window, NULL);
	SDL_GetWindowSize(window.handle, &window.windowSize.x, &window.windowSize.y);
	window.windowHalfSizef.x = (float)window.windowSize.x*0.5f;
	window.windowHalfSizef.y = (float)window.windowSize.y*0.5f;
	return window;
}

/*
SGL_Window SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags)
{
#if defined(_WINDOWS)
	SDL_DisplayMode targetMode;
	if (SDL_GetCurrentDisplayMode(0, &targetMode))
	{
		SGL_Window window;
		window.glContext = NULL;
		window.handle = NULL;
		return window;
	}
	SDL_DisplayMode closestMode;
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SGL_Window window;
	window.handle = SDL_CreateWindow
		(
			title,
			x, y,
			w, h,
			SDLflags
		);
	window.glContext = SDL_GL_CreateContext(window.handle);
	glewExperimental = GL_TRUE;
	glewInit();
#elif defined(ANDROID)
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion > 3 ? GLMajorVersion : 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SGL_Window* window = SDL_malloc(sizeof(SGL_Window));
	SDL_DisplayMode displayMode;
	if (SDL_GetCurrentDisplayMode(0, &displayMode))
	{
		return NULL;
	}
	window->window = SDL_CreateWindow
		(
			title,
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			displayMode.w, displayMode.h,
			SDLflags
			);
	window->context = SDL_GL_CreateContext(window->window);
#endif
	SDL_Log("----------------------------------------------------------------\n");
	SDL_Log("Graphics Successfully Initialized For Window: %s\n", title);
	SDL_Log("OpenGL Info\n");
	SDL_Log("  Version: %s\n", glGetString(GL_VERSION));
	SDL_Log("   Vendor: %s\n", glGetString(GL_VENDOR));
	SDL_Log(" Renderer: %s\n", glGetString(GL_RENDERER));
	SDL_Log("  Shading: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	SDL_Log("----------------------------------------------------------------\n");
	SGL_SetWindowIcon(&window, NULL);
	SDL_GetWindowSize(window.handle, &window.rContext.windowSize.x, &window.rContext.windowSize.y);
	window.rContext.windowHalfSizef.x = (float)window.rContext.windowSize.x*0.5f;
	window.rContext.windowHalfSizef.y = (float)window.rContext.windowSize.y*0.5f;
	for (size_t i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		window.rContext.cameras[i].camType = 0;
	}
	return window;
}*/

void SGL_SetWindowIcon(const SGL_Window* window, void * pixels)
{
	SDL_Surface* surface;
	if (pixels == NULL)
	{
		unsigned char defaultPixels[] = { 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 52, 52, 75, 75, 75, 127, 127, 127, 194, 194, 194, 127, 127, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 75, 75, 131, 131, 131, 75, 75, 75, 127, 127, 127, 75, 75, 75, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 75, 75, 193, 193, 193, 25, 25, 25, 0, 0, 0, 0, 0, 0, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 27, 27, 0, 0, 0, 52, 52, 52, 131, 131, 131, 0, 0, 0, 52, 52, 52, 0, 0, 0, 77, 77, 77, 38, 38, 38, 17, 17, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 21, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 72, 72, 72, 0, 0, 0, 52, 52, 52, 168, 168, 168, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 38, 38, 77, 77, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 40, 40, 21, 21, 21, 0, 0, 0, 0, 0, 0, 99, 99, 99, 75, 75, 75, 194, 194, 194, 112, 112, 112, 75, 75, 75, 0, 0, 0, 0, 0, 0, 54, 54, 54, 131, 131, 131, 0, 0, 0, 0, 0, 0, 57, 57, 57, 0, 0, 0, 87, 87, 87, 87, 87, 87, 97, 97, 97, 50, 50, 50, 147, 147, 147, 75, 75, 75, 212, 212, 212, 75, 75, 75, 129, 129, 129, 0, 0, 0, 54, 54, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 110, 110, 14, 14, 14, 0, 0, 0, 44, 44, 44, 65, 65, 65, 50, 50, 50, 84, 84, 84, 161, 161, 161, 210, 210, 210, 166, 166, 166, 75, 75, 75, 38, 38, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 0, 0, 0, 75, 75, 75, 21, 21, 21, 0, 0, 0, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 0, 0, 0, 40, 40, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 39, 39, 39, 39, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 77, 77, 131, 131, 131, 131, 131, 131, 131, 131, 131, 77, 77, 77, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255, 255, 0, 255 };
		surface = SDL_CreateRGBSurfaceFrom(defaultPixels, 16, 16, 24, 3 * 16, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	}
	else
	{
		surface = SDL_CreateRGBSurfaceFrom(pixels, 16, 16, 24, 3 * 16, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	}
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
	SDL_SetWindowIcon(window->handle, surface);
	SDL_FreeSurface(surface);
}

void SGL_DestroyWindow(const SGL_Window* window)
{
	//SDL_GL_DeleteContext(window->glContext);
	SDL_DestroyWindow(window->handle);
}

void SGL_ConvertPNGToIconArray(const char * imagePath, const char * fileName)
{
	SDL_RWops* rwop = SDL_RWFromFile(imagePath, "rb");
	if (rwop)
	{
		SDL_Surface* surf = IMG_LoadPNG_RW(rwop);
		SDL_RWclose(rwop);
		SDL_RWops *rw = SDL_RWFromFile(fileName, "w");
		if (rw)
		{
			size_t len = surf->h*surf->w*surf->format->BytesPerPixel;
			const char start[] = "unsigned char icon[] = { ";
			SDL_RWwrite(rw, start, 1, SDL_strlen(start));
			size_t i;
			for (i = 0; i < len; i++)
			{
				char buffer[5];
				SDL_snprintf(buffer, 5, "%u", *(((unsigned char*)surf->pixels) + i));
				//sprintf(buffer, "%u", *(((unsigned char*)surf->pixels) + i));
				size_t l;
				for (l = 1; l < 4; l++)
				{
					if (buffer[l] == 0 && i + 1 < len)
					{
						buffer[l] = ',';
						buffer[l + 1] = ' ';
						SDL_RWwrite(rw, buffer, 1, l + 2);
						break;
					}
				}
			}
			char buffer[7];
			SDL_snprintf(buffer, 7,"%u", *(((unsigned char*)surf->pixels) + (len - 1)));
			size_t l;
			for (l = 1; l < 4; l++)
			{
				if (buffer[l] == 0)
				{
					buffer[l] = ' ';
					buffer[l + 1] = '}';
					buffer[l + 2] = ';';
					SDL_RWwrite(rw, buffer, 1, l + 3);
					break;
				}
			}
			SDL_RWclose(rw);
		}
	}
}
static void demo_resize(SGL_VkContext* vkContext) {
	uint32_t i;

	// Don't react to resize until after first initialization.
	if (!vkContext->prepared) {
		return;
	}
	// In order to properly resize the window, we must re-create the swapchain
	// AND redo the command buffers, etc.
	//
	// First, perform part of the demo_cleanup() function:
	vkContext->prepared = SGL_FALSE;

	for (i = 0; i < vkContext->swapchainImageCount; i++) {
		vkDestroyFramebuffer(vkContext->device, vkContext->framebuffers[i], NULL);
	}
	free(vkContext->framebuffers);
	vkDestroyDescriptorPool(vkContext->device, vkContext->desc_pool, NULL);

	vkDestroyPipeline(vkContext->device, vkContext->pipeline, NULL);
	vkDestroyPipelineCache(vkContext->device, vkContext->pipelineCache, NULL);
	vkDestroyRenderPass(vkContext->device, vkContext->render_pass, NULL);
	vkDestroyPipelineLayout(vkContext->device, vkContext->pipeline_layout, NULL);
	vkDestroyDescriptorSetLayout(vkContext->device, vkContext->desc_layout, NULL);

	for (i = 0; i < DEMO_TEXTURE_COUNT; i++) {
		vkDestroyImageView(vkContext->device, vkContext->textures[i].view, NULL);
		vkDestroyImage(vkContext->device, vkContext->textures[i].image, NULL);
		vkFreeMemory(vkContext->device, vkContext->textures[i].mem, NULL);
		vkDestroySampler(vkContext->device, vkContext->textures[i].sampler, NULL);
	}

	vkDestroyImageView(vkContext->device, vkContext->depth.view, NULL);
	vkDestroyImage(vkContext->device, vkContext->depth.image, NULL);
	vkFreeMemory(vkContext->device, vkContext->depth.mem, NULL);

	vkDestroyBuffer(vkContext->device, vkContext->uniform_data.buf, NULL);
	vkFreeMemory(vkContext->device, vkContext->uniform_data.mem, NULL);

	for (i = 0; i < vkContext->swapchainImageCount; i++) {
		vkDestroyImageView(vkContext->device, vkContext->buffers[i].view, NULL);
		vkFreeCommandBuffers(vkContext->device, vkContext->cmd_pool, 1,
			&vkContext->buffers[i].cmd);
	}
	vkDestroyCommandPool(vkContext->device, vkContext->cmd_pool, NULL);
	free(vkContext->buffers);

	// Second, re-perform the demo_prepare() function, which will re-create the
	// swapchain:
	demo_prepare(vkContext);
}
void demo_update_data_buffer(SGL_VkContext* vkContext) {
	SGL_Mat4 MVP, Model, VP;
	int matrixSize = sizeof(SGL_Mat4);
	U8* pData;
	VkResult U_ASSERT_ONLY err;

	VP = SM_M4Multiply(&vkContext->view_matrix, &vkContext->projection_matrix);
	//mat4x4_mul(VP, vkContext->projection_matrix, vkContext->view_matrix);

	// Rotate 22.5 degrees around the Y axis
	Model = vkContext->model_matrix;
	//mat4x4_dup(Model, vkContext->model_matrix);
	SGL_Vec4 up = { 0.0f,1.0f,0.0f,0.0f };
	vkContext->model_matrix = SM_Rotate(&Model, (float)degreesToRadians(vkContext->spin_angle), &up);
	//mat4x4_mul(MVP, VP, vkContext->model_matrix);
	MVP = SM_M4Multiply(&vkContext->model_matrix, &VP);

	err = vkMapMemory(vkContext->device, vkContext->uniform_data.mem, 0,
		vkContext->uniform_data.mem_alloc.allocationSize, 0,
		(void **)&pData);
	assert(!err);

	//memcpy(pData, (const void *)&MVP[0][0], matrixSize);
	memcpy(pData, (const void *)&MVP, matrixSize);

	vkUnmapMemory(vkContext->device, vkContext->uniform_data.mem);
}
static void demo_draw(SGL_VkContext* vkContext) {
	VkResult U_ASSERT_ONLY err;
	VkSemaphore presentCompleteSemaphore;
	VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
	};
	VkFence nullFence = VK_NULL_HANDLE;

	err = vkCreateSemaphore(vkContext->device, &presentCompleteSemaphoreCreateInfo,
		NULL, &presentCompleteSemaphore);
	assert(!err);

	// Get the index of the next available swapchain image:
	err = vkContext->fpAcquireNextImageKHR(vkContext->device, vkContext->swapchain, UINT64_MAX,
		presentCompleteSemaphore,
		(VkFence)0, // TODO: Show use of fence
		&vkContext->current_buffer);
	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		// demo->swapchain is out of date (e.g. the window was resized) and
		// must be recreated:
		demo_resize(vkContext);
		demo_draw(vkContext);
		vkDestroySemaphore(vkContext->device, presentCompleteSemaphore, NULL);
		return;
	}
	else if (err == VK_SUBOPTIMAL_KHR) {
		// demo->swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
	}
	else {
		assert(!err);
	}

	// Assume the command buffer has been run on current_buffer before so
	// we need to set the image layout back to COLOR_ATTACHMENT_OPTIMAL
	demo_set_image_layout(vkContext, vkContext->buffers[vkContext->current_buffer].image,
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	demo_flush_init_cmd(vkContext);

	// Wait for the present complete semaphore to be signaled to ensure
	// that the image won't be rendered to until the presentation
	// engine has fully released ownership to the application, and it is
	// okay to render to the image.

	// FIXME/TODO: DEAL WITH VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	VkPipelineStageFlags pipe_stage_flags =
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	VkSubmitInfo submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = NULL,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &presentCompleteSemaphore,
		.pWaitDstStageMask = &pipe_stage_flags,
		.commandBufferCount = 1,
		.pCommandBuffers =
		&vkContext->buffers[vkContext->current_buffer].cmd,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL };

	err = vkQueueSubmit(vkContext->queue, 1, &submit_info, nullFence);
	assert(!err);

	VkPresentInfoKHR present = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.swapchainCount = 1,
		.pSwapchains = &vkContext->swapchain,
		.pImageIndices = &vkContext->current_buffer,
	};

	// TBD/TODO: SHOULD THE "present" PARAMETER BE "const" IN THE HEADER?
	err = vkContext->fpQueuePresentKHR(vkContext->queue, &present);
	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		// demo->swapchain is out of date (e.g. the window was resized) and
		// must be recreated:
		demo_resize(vkContext);
	}
	else if (err == VK_SUBOPTIMAL_KHR) {
		// demo->swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
	}
	else {
		assert(!err);
	}

	err = vkQueueWaitIdle(vkContext->queue);
	assert(err == VK_SUCCESS);

	vkDestroySemaphore(vkContext->device, presentCompleteSemaphore, NULL);
}
static void demo_init_connection(struct demo *demo) {
#ifndef _WIN32
	const xcb_setup_t *setup;
	xcb_screen_iterator_t iter;
	int scr;

	demo->connection = xcb_connect(NULL, &scr);
	if (demo->connection == NULL) {
		printf("Cannot find a compatible Vulkan installable client driver "
			"(ICD).\nExiting ...\n");
		fflush(stdout);
		exit(1);
	}

	setup = xcb_get_setup(demo->connection);
	iter = xcb_setup_roots_iterator(setup);
	while (scr-- > 0)
		xcb_screen_next(&iter);

	demo->screen = iter.data;
#endif // _WIN32
}
static void demo_init(SGL_VkContext* vkContext)
{
	SGL_Vec4 eye = { 0.0f, 3.0f, 5.0f ,0.0f};
	SGL_Vec4 origin = { 0.0f, 0.0f, 0.0f, 0.0f };
	SGL_Vec4 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	vec3 eye0 = { 0.0f, 3.0f, 5.0f};
	vec3 origin0 = { 0.0f, 0.0f, 0.0f};
	vec3 up0 = { 0.0f, 1.0f, 0.0f};
//	mat4x4 testMat;
//	mat4x4 testMat0;
	vkContext->spin_angle = 0.01f;
	vkContext->spin_increment = 0.01f;
	//OK
	vkContext->projection_matrix = SM_Perspective((float)degreesToRadians(45.0f), (float)vkContext->width/(float)vkContext->height, 0.1f, 100.0f);
	//mat4x4_perspective(testMat, (float)degreesToRadians(45.0f), (float)vkContext->width / (float)vkContext->height, 0.1f, 100.0f);
	vkContext->view_matrix = SM_LookAt(&eye, &origin, &up);
	//mat4x4_look_at(testMat0, eye0, origin0, up0);
	vkContext->model_matrix = SM_IdentityMat4();
	//mat4x4_identity(vkContext->model_matrix);
}
void SGL_InitVulkanDemo(SGL_VkContext* vkContext)
{
	demo_init(vkContext);
	demo_prepare(vkContext);
	demo_draw(vkContext);
}
void SGL_UpdateVulkanDemo(SGL_VkContext * vkContext)
{
	if (!vkContext->prepared)
	{
		return;
	}
	vkDeviceWaitIdle(vkContext->device);
	demo_update_data_buffer(vkContext);
	demo_draw(vkContext);
	RedrawWindow(vkContext->window,NULL,NULL, RDW_INTERNALPAINT);
}
/*
void SGL_RunGLTest(const SGL_Window* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
	SDL_Delay(200);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->handle);
}*/
