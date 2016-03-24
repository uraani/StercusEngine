//#include "SGL_vk.h"
#include "SGL_window.h"
#include "SGL_data.h"

//TODO: CHECK IF ALL VULKAN POINTERS ARE NON CONST
#define VERTEX_BUFFER_BIND_ID 0
VKAPI_ATTR VkBool32 VKAPI_CALL dbgFuncNew(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType, uint64_t srcObject, size_t location, int32_t msgCode, const char *pLayerPrefix, const char *pMsg, void *pUserData)
{
	//strlen("4294967295") == 10 (max characters int32_t can have)
	//strlen("VK WARNING: [] Code  : ") + 1 == 25
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
static VkBool32 demo_check_layers(uint32_t check_count, char **check_names,
	uint32_t layer_count,
	VkLayerProperties *layers) {
	for (uint32_t i = 0; i < check_count; i++) {
		VkBool32 found = 0;
		for (uint32_t j = 0; j < layer_count; j++) {
			if (!strcmp(check_names[i], layers[j].layerName)) {
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
static VkBool32 demo_init_vk(SGL_VkSwapChain *demo) 
{
	VkResult err;
	char *extension_names[64];
	uint32_t instance_extension_count = 0;
	uint32_t instance_layer_count = 0;
	uint32_t device_validation_layer_count = 0;
	uint32_t enabled_extension_count = 0;
	uint32_t enabled_layer_count = 0;

	char *instance_validation_layers[] = {
		"VK_LAYER_GOOGLE_threading",     "VK_LAYER_LUNARG_param_checker",
		"VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_object_tracker",
		"VK_LAYER_LUNARG_image",         "VK_LAYER_LUNARG_mem_tracker",
		"VK_LAYER_LUNARG_draw_state",    "VK_LAYER_LUNARG_swapchain",
		"VK_LAYER_GOOGLE_unique_objects"
	};

	demo->device_validation_layers[0] = "VK_LAYER_GOOGLE_threading";
	demo->device_validation_layers[1] = "VK_LAYER_LUNARG_param_checker";
	demo->device_validation_layers[2] = "VK_LAYER_LUNARG_device_limits";
	demo->device_validation_layers[3] = "VK_LAYER_LUNARG_object_tracker";
	demo->device_validation_layers[4] = "VK_LAYER_LUNARG_image";
	demo->device_validation_layers[5] = "VK_LAYER_LUNARG_mem_tracker";
	demo->device_validation_layers[6] = "VK_LAYER_LUNARG_draw_state";
	demo->device_validation_layers[7] = "VK_LAYER_LUNARG_swapchain";
	demo->device_validation_layers[8] = "VK_LAYER_GOOGLE_unique_objects";
	device_validation_layer_count = 9;

	/* Look for validation layers */
	VkBool32 validation_found = 0;
	err = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
	SDL_assert(!err);

	if (instance_layer_count > 0) {
		VkLayerProperties *instance_layers =
			malloc(sizeof(VkLayerProperties) * instance_layer_count);
		err = vkEnumerateInstanceLayerProperties(&instance_layer_count,
			instance_layers);
		SDL_assert(!err);

		if (demo->validate) {
			validation_found = demo_check_layers(
				SDL_arraysize(instance_validation_layers),
				instance_validation_layers, instance_layer_count,
				instance_layers);

			enabled_layer_count = SDL_arraysize(instance_validation_layers);
		}

		free(instance_layers);
	}

	if (demo->validate && !validation_found) {
		SDL_Log("vkEnumerateInstanceLayerProperties failed to find"
			"required validation layer.\n\n"
			"Please look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
	}

	/* Look for instance extensions */
	VkBool32 surfaceExtFound = 0;
	VkBool32 platformSurfaceExtFound = 0;
	memset(extension_names, 0, sizeof(extension_names));

	err = vkEnumerateInstanceExtensionProperties(
		NULL, &instance_extension_count, NULL);
	SDL_assert(!err);

	if (instance_extension_count > 0) {
		VkExtensionProperties *instance_extensions =
			malloc(sizeof(VkExtensionProperties) * instance_extension_count);
		err = vkEnumerateInstanceExtensionProperties(
			NULL, &instance_extension_count, instance_extensions);
		SDL_assert(!err);
		for (uint32_t i = 0; i < instance_extension_count; i++) {
			if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME,
				instance_extensions[i].extensionName)) {
				surfaceExtFound = 1;
				extension_names[enabled_extension_count++] =
					VK_KHR_SURFACE_EXTENSION_NAME;
			}
#ifdef _WIN32
			if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
				instance_extensions[i].extensionName)) {
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] =
					VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
#else  // _WIN32
			if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME,
				instance_extensions[i].extensionName)) {
				platformSurfaceExtFound = 1;
				extension_names[enabled_extension_count++] =
					VK_KHR_XCB_SURFACE_EXTENSION_NAME;
			}
#endif // _WIN32
			if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
				instance_extensions[i].extensionName)) {
				if (demo->validate) {
					extension_names[enabled_extension_count++] =
						VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
				}
			}
			SDL_assert(enabled_extension_count < 64);
		}

		free(instance_extensions);
	}

	if (!surfaceExtFound) {
		SDL_Log("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
	}
	if (!platformSurfaceExtFound) {
#ifdef _WIN32
		SDL_Log("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_WIN32_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
#else  // _WIN32
		ERR_EXIT("vkEnumerateInstanceExtensionProperties failed to find "
			"the " VK_KHR_XCB_SURFACE_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
#endif // _WIN32
	}
	const VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pNext = NULL,
		.pApplicationName = "SGL",
		.applicationVersion = 0,
		.pEngineName = "SGL_ENGINE",
		.engineVersion = 0,
		.apiVersion = VK_MAKE_VERSION(1, 0, 0),
	};
	VkInstanceCreateInfo inst_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = NULL,
		.pApplicationInfo = &app,
		.enabledLayerCount = enabled_layer_count,
		.ppEnabledLayerNames =
		(const char *const *)((demo->validate) ? instance_validation_layers
			: NULL),
		.enabledExtensionCount = enabled_extension_count,
		.ppEnabledExtensionNames = (const char *const *)extension_names,
	};

	uint32_t gpu_count;

	err = vkCreateInstance(&inst_info, NULL, &demo->inst);
	if (err == VK_ERROR_INCOMPATIBLE_DRIVER) {
		SDL_Log("Cannot find a compatible Vulkan installable client driver "
			"(ICD).\n\nPlease look at the Getting Started guide for "
			"additional information.\n",
			"vkCreateInstance Failure");
	}
	else if (err == VK_ERROR_EXTENSION_NOT_PRESENT) {
		SDL_Log("Cannot find a specified extension library"
			".\nMake sure your layers path is set appropriately.\n",
			"vkCreateInstance Failure");
	}
	else if (err) {
		SDL_Log("vkCreateInstance failed.\n\nDo you have a compatible Vulkan "
			"installable client driver (ICD) installed?\nPlease look at "
			"the Getting Started guide for additional information.\n",
			"vkCreateInstance Failure");
	}

	/* Make initial call to query gpu_count, then second call for gpu info*/
	err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, NULL);
	SDL_assert(!err && gpu_count > 0);

	if (gpu_count > 0) {
		VkPhysicalDevice *physical_devices = malloc(sizeof(VkPhysicalDevice) * gpu_count);
		err = vkEnumeratePhysicalDevices(demo->inst, &gpu_count, physical_devices);
		SDL_assert(!err);
		/* For cube demo we just grab the first physical device */
		demo->gpu = physical_devices[0];
		free(physical_devices);
	}
	else {
		SDL_Log("vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
			"Do you have a compatible Vulkan installable client driver (ICD) "
			"installed?\nPlease look at the Getting Started guide for "
			"additional information.\n",
			"vkEnumeratePhysicalDevices Failure");
	}

	/* Look for validation layers */
	validation_found = 0;
	demo->enabled_layer_count = 0;
	uint32_t device_layer_count = 0;
	err =
		vkEnumerateDeviceLayerProperties(demo->gpu, &device_layer_count, NULL);
	SDL_assert(!err);

	if (device_layer_count > 0) {
		VkLayerProperties *device_layers =
			malloc(sizeof(VkLayerProperties) * device_layer_count);
		err = vkEnumerateDeviceLayerProperties(demo->gpu, &device_layer_count,
			device_layers);
		SDL_assert(!err);

		if (demo->validate) {
			validation_found = demo_check_layers(device_validation_layer_count,
				demo->device_validation_layers,
				device_layer_count,
				device_layers);
			demo->enabled_layer_count = device_validation_layer_count;
		}

		free(device_layers);
	}

	if (demo->validate && !validation_found) {
		SDL_Log("vkEnumerateDeviceLayerProperties failed to find"
			"a required validation layer.\n\n"
			"Please look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateDevice Failure");
	}

	/* Look for device extensions */
	uint32_t device_extension_count = 0;
	VkBool32 swapchainExtFound = 0;
	demo->enabled_extension_count = 0;
	memset(extension_names, 0, sizeof(extension_names));

	err = vkEnumerateDeviceExtensionProperties(demo->gpu, NULL,
		&device_extension_count, NULL);
	SDL_assert(!err);

	if (device_extension_count > 0) {
		VkExtensionProperties *device_extensions =
			malloc(sizeof(VkExtensionProperties) * device_extension_count);
		err = vkEnumerateDeviceExtensionProperties(
			demo->gpu, NULL, &device_extension_count, device_extensions);
		SDL_assert(!err);

		for (uint32_t i = 0; i < device_extension_count; i++) {
			if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				device_extensions[i].extensionName)) {
				swapchainExtFound = 1;
				demo->extension_names[demo->enabled_extension_count++] =
					VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			}
			SDL_assert(demo->enabled_extension_count < 64);
		}

		free(device_extensions);
	}

	if (!swapchainExtFound) {
		SDL_Log("vkEnumerateDeviceExtensionProperties failed to find "
			"the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
			" extension.\n\nDo you have a compatible "
			"Vulkan installable client driver (ICD) installed?\nPlease "
			"look at the Getting Started guide for additional "
			"information.\n",
			"vkCreateInstance Failure");
	}

	if (demo->validate) {
		demo->CreateDebugReportCallback =
			(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
				demo->inst, "vkCreateDebugReportCallbackEXT");
		demo->DestroyDebugReportCallback =
			(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
				demo->inst, "vkDestroyDebugReportCallbackEXT");
		if (!demo->CreateDebugReportCallback) {
			SDL_Log(
				"GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n",
				"vkGetProcAddr Failure");
		}
		if (!demo->DestroyDebugReportCallback) {
			SDL_Log(
				"GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n",
				"vkGetProcAddr Failure");
		}
		demo->DebugReportMessage =
			(PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(
				demo->inst, "vkDebugReportMessageEXT");
		if (!demo->DebugReportMessage) {
			SDL_Log("GetProcAddr: Unable to find vkDebugReportMessageEXT\n",
				"vkGetProcAddr Failure");
		}

		PFN_vkDebugReportCallbackEXT callback;
		callback = dbgFuncNew;
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pNext = NULL;
		dbgCreateInfo.pfnCallback = callback;
		dbgCreateInfo.pUserData = NULL;
		dbgCreateInfo.flags =
			VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		err = demo->CreateDebugReportCallback(demo->inst, &dbgCreateInfo, NULL,
			&demo->msg_callback);
		switch (err) {
		case VK_SUCCESS:
			break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			SDL_Log("CreateDebugReportCallback: out of host memory\n",
				"CreateDebugReportCallback Failure");
			break;
		default:
			SDL_Log("CreateDebugReportCallback: unknown failure\n",
				"CreateDebugReportCallback Failure");
			break;
		}
	}
	vkGetPhysicalDeviceProperties(demo->gpu, &demo->gpuProps);

	/* Call with NULL data to get count */
	vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_count,
		NULL);
	SDL_assert(demo->queue_count >= 1);

	demo->queue_props = (VkQueueFamilyProperties *)malloc(
		demo->queue_count * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties(demo->gpu, &demo->queue_count,
		demo->queue_props);
	// Find a queue that supports gfx
	uint32_t gfx_queue_idx = 0;
	for (gfx_queue_idx = 0; gfx_queue_idx < demo->queue_count;
	gfx_queue_idx++) {
		if (demo->queue_props[gfx_queue_idx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;
	}
	SDL_assert(gfx_queue_idx < demo->queue_count);
	// Query fine-grained feature support for this device.
	//  If app has specific feature requirements it should check supported
	//  features based on this query
	VkPhysicalDeviceFeatures physDevFeatures;
	vkGetPhysicalDeviceFeatures(demo->gpu, &physDevFeatures);

	demo->fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(demo->inst, "vkGetPhysicalDeviceSurfaceSupportKHR");
	if (demo->fpGetPhysicalDeviceSurfaceSupportKHR == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
		return VK_FALSE;
	}
	demo->fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(demo->inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
	if (demo->fpGetPhysicalDeviceSurfaceCapabilitiesKHR == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
		return VK_FALSE;
	}
	demo->fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(demo->inst, "vkGetPhysicalDeviceSurfaceFormatsKHR");
	if (demo->fpGetPhysicalDeviceSurfaceFormatsKHR == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
		return VK_FALSE;
	}
	demo->fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(demo->inst, "vkGetPhysicalDeviceSurfacePresentModesKHR");
	if (demo->fpGetPhysicalDeviceSurfacePresentModesKHR == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
		return VK_FALSE;
	}
	demo->fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)vkGetInstanceProcAddr(demo->inst, "vkGetSwapchainImagesKHR");
	if (demo->fpGetSwapchainImagesKHR == NULL)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
		return VK_FALSE;
	}
	return VK_TRUE;
}
static void demo_create_device(SGL_VkSwapChain *demo) 
{
	VkResult err;
	float queue_priorities[1] = { 0.0 };
	const VkDeviceQueueCreateInfo queue = 
	{
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.pNext = NULL,
		.queueFamilyIndex = demo->queueNodeIndex,
		.queueCount = 1,
		.pQueuePriorities = queue_priorities };

	VkDeviceCreateInfo device = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = NULL,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &queue,
		.enabledLayerCount = demo->enabled_layer_count,
		.ppEnabledLayerNames =
		(const char *const *)((demo->validate)
			? demo->device_validation_layers
			: NULL),
		.enabledExtensionCount = demo->enabled_extension_count,
		.ppEnabledExtensionNames = (const char *const *)demo->extension_names,
		.pEnabledFeatures =
		NULL, // If specific features are required, pass them in here
	};

	err = vkCreateDevice(demo->gpu, &device, NULL, &demo->device);
	SDL_assert(!err);
}
static VkBool32 demo_init_vk_swapchain(SGL_VkSwapChain *demo) 
{
	VkResult err;
	uint32_t i;

	// Create a WSI surface for the window:
#ifdef _WIN32
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(demo->window, &info);
	VkWin32SurfaceCreateInfoKHR createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.hinstance = GetModuleHandle(NULL);
	createInfo.hwnd = info.info.win.window;

	err =
		vkCreateWin32SurfaceKHR(demo->inst, &createInfo, NULL, &demo->surf);

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
		(VkBool32 *)malloc(demo->queue_count * sizeof(VkBool32));
	for (i = 0; i < demo->queue_count; i++) {
		demo->fpGetPhysicalDeviceSurfaceSupportKHR(demo->gpu, i, demo->surf,
			&supportsPresent[i]);
	}

	// Search for a graphics and a present queue in the array of queue
	// families, try to find one that supports both
	uint32_t graphicsQueueNodeIndex = UINT32_MAX;
	uint32_t presentQueueNodeIndex = UINT32_MAX;
	for (i = 0; i < demo->queue_count; i++) {
		if ((demo->queue_props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
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
		for (uint32_t i = 0; i < demo->queue_count; ++i) {
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
		SDL_Log("Could not find a graphics and a present queue\n",
			"Swapchain Initialization Failure");
	}

	// TODO: Add support for separate queues, including presentation,
	//       synchronization, and appropriate tracking for QueueSubmit.
	// NOTE: While it is possible for an application to use a separate graphics
	//       and a present queues, this demo program assumes it is only using
	//       one:
	if (graphicsQueueNodeIndex != presentQueueNodeIndex) {
		SDL_Log("Could not find a common graphics and a present queue\n",
			"Swapchain Initialization Failure");
	}

	demo->queueNodeIndex = graphicsQueueNodeIndex;

	demo_create_device(demo);

	{
		PFN_vkGetDeviceProcAddr gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(demo->inst, "vkGetDeviceProcAddr");

		demo->fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)gdpa(demo->device, "vkCreateSwapchainKHR");
		if (demo->fpCreateSwapchainKHR == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
			return VK_FALSE;
		}
		demo->fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)gdpa(demo->device, "vkDestroySwapchainKHR");
		if (demo->fpDestroySwapchainKHR == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
			return VK_FALSE;
		}
		demo->fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)gdpa(demo->device, "vkGetSwapchainImagesKHR");
		if (demo->fpGetSwapchainImagesKHR == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
			return VK_FALSE;
		}
		demo->fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)gdpa(demo->device, "vkAcquireNextImageKHR");
		if (demo->fpAcquireNextImageKHR == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
			return VK_FALSE;
		}
		demo->fpQueuePresentKHR = (PFN_vkQueuePresentKHR)gdpa(demo->device, "vkQueuePresentKHR");
		if (demo->fpQueuePresentKHR == NULL)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", NULL);
			return VK_FALSE;
		}
	}
	vkGetDeviceQueue(demo->device, demo->queueNodeIndex, 0,
		&demo->queue);

	// Get the list of VkFormat's that are supported:
	uint32_t formatCount;
	err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surf,
		&formatCount, NULL);
	SDL_assert(!err);
	VkSurfaceFormatKHR *surfFormats =
		(VkSurfaceFormatKHR *)malloc(formatCount * sizeof(VkSurfaceFormatKHR));
	err = demo->fpGetPhysicalDeviceSurfaceFormatsKHR(demo->gpu, demo->surf,
		&formatCount, surfFormats);
	SDL_assert(!err);
	// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
	// the surface has no preferred format.  Otherwise, at least one
	// supported format will be returned.
	if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED) 
	{
		demo->colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else {
		SDL_assert(formatCount >= 1);
		demo->colorFormat = surfFormats[0].format;
	}
	demo->colorSpace = surfFormats[0].colorSpace;
	{
		VkFormat depthFormats[] =
		{
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};
		const size_t depthCount = SDL_arraysize(depthFormats);
		for (size_t i = 0; i < depthCount; i++)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(demo->gpu, depthFormats[i], &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				demo->depthFormat = depthFormats[i];
				break;
			}
		}
		SDL_assert(demo->depthFormat != VK_FORMAT_UNDEFINED);
	}
	// Get Memory information and properties
	vkGetPhysicalDeviceMemoryProperties(demo->gpu, &demo->memoryProperties);
	return VK_TRUE;
}
U32 SGL_InitVulkan(SGL_Window * window)
{
	memset(&window->vkSwapChain, 0, sizeof(SGL_VkSwapChain));
	window->vkSwapChain.validate = SGL_TRUE;
	window->vkSwapChain.window = window->handle;
	demo_init_vk(&window->vkSwapChain);
	demo_init_vk_swapchain(&window->vkSwapChain);
	/*
	VkResult err;
	// Init vkSwapChain struct
	for (U32 i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		window->vkSwapChain.cameras[i].camType = 0;
	}
	//Creates instance, physicaldevice, device, surface and check for available validation layers and extensions
	{
		char* extensionNames[64];
		U32 instanceExtensionCount = 0;
		U32 enabledExtensionCount = 0;
		// Look for instance validation layers  (error is thrown if even one is missing)
		// This is only done in debug mode since its a debug feature imo
#ifndef NDEBUG
		char* instanceValidationLayers[] = 
		{
			"VK_LAYER_GOOGLE_threading",     "VK_LAYER_LUNARG_param_checker",
			"VK_LAYER_LUNARG_device_limits", "VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_image",         "VK_LAYER_LUNARG_mem_tracker",
			"VK_LAYER_LUNARG_draw_state",    "VK_LAYER_LUNARG_swapchain",
			"VK_LAYER_GOOGLE_unique_objects"
		};
		U32 foundInstanceLayerCount = 0;
		U32 deviceValidationLayerCount = 0;
		U32 enabledLayerCount = 0;
		U32 instanceLayerCount = SDL_arraysize(instanceValidationLayers);
		deviceValidationLayerCount = 9;

		err = vkEnumerateInstanceLayerProperties(&foundInstanceLayerCount, NULL);
		SDL_assert(!err);

		if (foundInstanceLayerCount > 0)
		{
			VkBool32 validationFound = 0;
			VkLayerProperties* instanceLayers = SDL_malloc(sizeof(VkLayerProperties) * foundInstanceLayerCount);
			err = vkEnumerateInstanceLayerProperties(&foundInstanceLayerCount, instanceLayers);
			SDL_assert(!err);
			for (U32 i = 0; i < instanceLayerCount; i++)
			{
				validationFound = 0;
				for (U32 j = 0; j < foundInstanceLayerCount; j++)
				{
					if (!SDL_strcmp(instanceValidationLayers[i], instanceLayers[j].layerName))
					{
						validationFound = 1;
						break;
					}
				}
				if (!validationFound)
				{
					SDL_Log("Cannot find layer: %s\n", instanceValidationLayers[i]);
					enabledLayerCount = 0;
					break;
				}
			}
			if (validationFound == 1)
			{
				enabledLayerCount = instanceLayerCount;
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkCreateInstance Failure",
					"vkEnumerateInstanceLayerProperties failed to find required validation layer.\n\n"
					"Please look at the Getting Started guide for additional information.\n", window->handle
					);
					return SGL_FALSE;
			}
			SDL_free(instanceLayers);
		}
#endif // !NDEBUG

		// Look for instance extensions 
		{
			VkBool32 surfaceExtFound = 0;
			VkBool32 platformSurfaceExtFound = 0;
			SDL_memset(extensionNames, 0, sizeof(extensionNames));

			err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
			SDL_assert(!err);

			if (instanceExtensionCount > 0)
			{
				VkExtensionProperties *instanceExtensions = SDL_malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
				err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions);

				SDL_assert(!err);

				for (uint32_t i = 0; i < instanceExtensionCount; i++)
				{
					if (!SDL_strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
					{
						surfaceExtFound = 1;
						extensionNames[enabledExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
					}
		#ifdef _WIN32
					if (!SDL_strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
					{
						platformSurfaceExtFound = 1;
						extensionNames[enabledExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
					}
		#else  // _WIN32
					if (!SDL_strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
					{
						platformSurfaceExtFound = 1;
						extensionNames[enabledExtensionCount++] = VK_KHR_XCB_SURFACE_EXTENSION_NAME;
					}
		#endif // _WIN32
		#ifndef NDEBUG
					if (!SDL_strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName))
					{
							extensionNames[enabledExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
					}
		#endif
					SDL_assert(instanceExtensionCount < 64);
				}
				SDL_free(instanceExtensions);
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
				return SGL_FALSE;
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
				return SGL_FALSE;
			}
		}

		U32 gpuCount;
		// Create vkInstance
		{
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
	#ifndef NDEBUG
				.enabledLayerCount = enabledLayerCount,
				.ppEnabledLayerNames = (const char *const *)instanceValidationLayers,
	#else
				.enabledLayerCount = 0,
				.ppEnabledLayerNames = NULL,
	#endif
				.enabledExtensionCount = enabledExtensionCount,
				.ppEnabledExtensionNames = (const char *const *)extensionNames,
			};

			err = vkCreateInstance(&inst_info, NULL, &window->vkSwapChain.inst);
			if (err == VK_ERROR_INCOMPATIBLE_DRIVER)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkCreateInstance Failure",
					"Cannot find a compatible Vulkan installable client driver (ICD).\n\n"
					"Please look at the Getting Started guide for additional information.\n", window->handle
					);
				return SGL_FALSE;
			}
			else if (err == VK_ERROR_EXTENSION_NOT_PRESENT)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
					"vkCreateInstance Failure",
					"Cannot find a specified extension library.\n"
					"Make sure your layers path is set appropriately.\n", window->handle
					);
				return SGL_FALSE;
			}
			else if (err)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"vkCreateInstance Failure",
					"vkCreateInstance failed.\n\n"
					"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
					"Please look at the Getting Started guide for additional information.\n", window->handle
					);
				return SGL_FALSE;
			}
			SDL_Log("Vulkan instance created successfully\n");
		}
		// Find vkPhysicalDevice
		{
			// Make initial call to query gpu_count, then second call for gpu info
			err = vkEnumeratePhysicalDevices(window->vkSwapChain.inst, &gpuCount, NULL);
			SDL_assert(!err && gpuCount > 0);

			if (gpuCount > 0)
			{
				VkPhysicalDevice *physicalDevices = SDL_malloc(sizeof(VkPhysicalDevice) * gpuCount);
				err = vkEnumeratePhysicalDevices(window->vkSwapChain.inst, &gpuCount, physicalDevices);
				SDL_assert(!err);
				VkPhysicalDeviceProperties* devProp = SDL_calloc(1,sizeof(VkPhysicalDeviceProperties));
				SDL_Log("Listing devices that support vulkan:\n\n");
				//these match with VkPhysicalDeviceType enum names
				const char* deviceTypes[] =
				{
					"VK_PHYSICAL_DEVICE_TYPE_OTHER",
					"VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU",
					"VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU",
					"VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU",
					"VK_PHYSICAL_DEVICE_TYPE_CPU"
				};
				for (U32 i = 0; i < gpuCount; i++)
				{
					vkGetPhysicalDeviceProperties(physicalDevices[i], &devProp[i]);
					SDL_Log("Physical Device: %i\n", i);
					SDL_Log("Driver Version:  %d\n", devProp[i].driverVersion);
					SDL_Log("Device Name:     %s\n", devProp[i].deviceName);
					SDL_Log("Device Type:     %s\n", deviceTypes[devProp[i].deviceType]);
					SDL_Log("API Version:     %d.%d.%d\n\n",
						//vulkan apiversion is encoded to integer
						//31-22 Major version
						//21-12 Minor version
						//11-0 Patch version
						(devProp[i].apiVersion >> 22) & 0x3FF,
						(devProp[i].apiVersion >> 12) & 0x3FF,
						(devProp[i].apiVersion & 0x3FF));
				}
				//in future find the discrete gpu and use it over other types of devices
				//devProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				//I shouldnt do this.
				SDL_Log("Aimlessly selecting first available device\n");
				window->vkSwapChain.gpu = physicalDevices[0];
				free(physicalDevices);
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkEnumeratePhysicalDevices Failure",
					"vkEnumeratePhysicalDevices reported zero accessible devices.\n\n"
					"Do you have a compatible Vulkan installable client driver (ICD) installed?\n"
					"Please look at the Getting Started guide for additional information.\n",window->handle);
				return SGL_FALSE;
			}
		}
		// Look for device validation layers
#ifndef NDEBUG
		SDL_Log("Checking layers supported by the gpu\n");
		uint32_t deviceLayerCount = 0;
		err =
			vkEnumerateDeviceLayerProperties(window->vkSwapChain.gpu, &deviceLayerCount, NULL);
		SDL_assert(!err);

		if (deviceLayerCount > 0)
		{
			VkBool32 validationFound = 0;
			VkLayerProperties *deviceLayers = malloc(sizeof(VkLayerProperties) * deviceLayerCount);
			err = vkEnumerateDeviceLayerProperties(window->vkSwapChain.gpu, &deviceLayerCount, deviceLayers);
			SDL_assert(!err);
			for (U32 i = 0; i < instanceLayerCount; i++)
			{
				validationFound = 0;
				for (U32 j = 0; j < deviceLayerCount; j++)
				{
					if (!SDL_strcmp(instanceValidationLayers[i], deviceLayers[j].layerName))
					{
						validationFound = 1;
						break;
					}
				}
				if (!validationFound)
				{
					SDL_Log("Cannot find layer: %s\n", instanceValidationLayers[i]);
					enabledLayerCount = 0;
					break;
				}
			}
			if (validationFound == 1)
			{
				enabledLayerCount = instanceLayerCount;
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkCreateDevice Failure",
					"vkEnumerateDeviceLayerProperties failed to find a required validation layer.\n\n"
					"Please look at the Getting Started guide for additional information.\n", window->handle
					);
				return SGL_FALSE;
			}
			SDL_free(deviceLayers);
		}
#endif // !NDEBUG
		// Look for device extensions
		{

			uint32_t deviceExtensionCount = 0;
			VkBool32 swapchainExtFound = 0;
			memset(extensionNames, 0, sizeof(extensionNames));
			enabledExtensionCount = 0;

			err = vkEnumerateDeviceExtensionProperties(window->vkSwapChain.gpu, NULL, &deviceExtensionCount, NULL);
			SDL_assert(!err);

			if (deviceExtensionCount > 0)
			{
				VkExtensionProperties *deviceExtensions = SDL_malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
				err = vkEnumerateDeviceExtensionProperties(window->vkSwapChain.gpu, NULL, &deviceExtensionCount, deviceExtensions);
				SDL_assert(!err);
				for (uint32_t i = 0; i < deviceExtensionCount; i++)
				{
					if (!SDL_strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName))
					{
						swapchainExtFound = 1;
						extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
					}
					SDL_assert(enabledExtensionCount < 64);
				}

				SDL_free(deviceExtensions);
			}

			if (!swapchainExtFound)
			{
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
		}
		// Create DebugCallback
#ifndef NDEBUG
		window->vkSwapChain.CreateDebugReportCallback = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkCreateDebugReportCallbackEXT");
		window->vkSwapChain.DestroyDebugReportCallback = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkDestroyDebugReportCallbackEXT");
		if (!window->vkSwapChain.CreateDebugReportCallback)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetProcAddr Failure", "GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n", window->handle);
		}
		if (!window->vkSwapChain.DestroyDebugReportCallback)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetProcAddr Failure", "GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n", window->handle);
		}
		PFN_vkDebugReportMessageEXT DebugReportMessage = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkDebugReportMessageEXT");
		if (!DebugReportMessage)
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "GetProcAddr: Unable to find vkDebugReportMessageEXT\n", "vkGetProcAddr Failure", window->handle);
		}
		PFN_vkDebugReportCallbackEXT callback = dbgFuncNew;
		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
		dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		dbgCreateInfo.pNext = NULL;
		dbgCreateInfo.pfnCallback = callback;
		dbgCreateInfo.pUserData = NULL;
		dbgCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		err = window->vkSwapChain.CreateDebugReportCallback(window->vkSwapChain.inst, &dbgCreateInfo, NULL, &window->vkSwapChain.msgCallback);
		switch (err)
		{
		case VK_SUCCESS:
		{
			break;
		}
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		{
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CreateDebugReportCallback: out of host memory\n", "CreateDebugReportCallback Failure", window->handle);
			break;
		}
		default:
		{

			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "CreateDebugReportCallback Failure", "CreateDebugReportCallback: unknown failure\n", window->handle);
			break;
		}
		}
#endif
		U32 queueCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(window->vkSwapChain.gpu, &queueCount, NULL);
		VkQueueFamilyProperties* queueFamilyProps = (VkQueueFamilyProperties *)malloc(queueCount * sizeof(VkQueueFamilyProperties));
		// Find a queue that supports gfx
		{
			// Call with NULL data to get count
			SDL_assert(queueCount >= 1);
			vkGetPhysicalDeviceQueueFamilyProperties(window->vkSwapChain.gpu, &queueCount, queueFamilyProps);
			U32 gfxQueueIndex = 0;
			for (gfxQueueIndex = 0; gfxQueueIndex < queueCount; gfxQueueIndex++)
			{
				if (queueFamilyProps[gfxQueueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					break;
				}
			}
			SDL_assert(gfxQueueIndex < queueCount);
			// Query fine-grained feature support for this device.
			//  If app has specific feature requirements it should check supported
			//  features based on this query
			VkPhysicalDeviceFeatures physDevFeatures;
			vkGetPhysicalDeviceFeatures(window->vkSwapChain.gpu, &physDevFeatures);

			window->vkSwapChain.fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkGetPhysicalDeviceSurfaceSupportKHR");
			if (window->vkSwapChain.fpGetPhysicalDeviceSurfaceSupportKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
			if (window->vkSwapChain.fpGetPhysicalDeviceSurfaceCapabilitiesKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkGetPhysicalDeviceSurfaceFormatsKHR");
			if (window->vkSwapChain.fpGetPhysicalDeviceSurfaceFormatsKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkGetPhysicalDeviceSurfacePresentModesKHR");
			if (window->vkSwapChain.fpGetPhysicalDeviceSurfacePresentModesKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
		}
		// Create a WSI surface for the window:
#ifdef _WIN32
		SDL_SysWMinfo info;
		SDL_VERSION(&info.version);
		SDL_GetWindowWMInfo(window->handle, &info);
		VkWin32SurfaceCreateInfoKHR createInfo;
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.hinstance = GetModuleHandle(NULL);
		createInfo.hwnd = info.info.win.window;
		//window->vkContext.window = createInfo.hwnd;

		err = vkCreateWin32SurfaceKHR(window->vkSwapChain.inst, &createInfo, NULL, &window->vkSwapChain.surf);
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
		U32 graphicsQueueNodeIndex = UINT32_MAX;
		// Search for a graphics and a present queue in the array of queue
		{
			VkBool32* supportsPresent = (VkBool32 *)SDL_malloc(queueCount * sizeof(VkBool32));
			// Iterate over each queue to learn whether it supports presenting:
			U32 i;
			{
				for (i = 0; i < queueCount; i++)
				{
					window->vkSwapChain.fpGetPhysicalDeviceSurfaceSupportKHR(window->vkSwapChain.gpu, i, window->vkSwapChain.surf, &supportsPresent[i]);
				}
			}
			// families, try to find one that supports both
			U32 presentQueueNodeIndex = UINT32_MAX;
			for (i = 0; i < queueCount; i++)
			{
				if ((queueFamilyProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
				{
					if (graphicsQueueNodeIndex == UINT32_MAX) 
					{
						graphicsQueueNodeIndex = i;
					}

					if (supportsPresent[i] == VK_TRUE) 
					{
						graphicsQueueNodeIndex = i;
						presentQueueNodeIndex = i;
						break;
					}
				}
			}
			if (presentQueueNodeIndex == UINT32_MAX) 
			{
				// If didn't find a queue that supports both graphics and present, then
				// find a separate present queue.
				for (i = 0; i < queueCount; ++i)
				{
					if (supportsPresent[i] == VK_TRUE) 
					{
						presentQueueNodeIndex = i;
						break;
					}
				}
			}
			SDL_free(queueFamilyProps);
			SDL_free(supportsPresent);

			// Generate error if could not find both a graphics and a present queue
			if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) 
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Swapchain Initialization Failure", "Could not find a graphics and a present queue\n", window->handle);
				return SGL_FALSE;
			}

			// TODO: Add support for separate queues, including presentation,
			//       synchronization, and appropriate tracking for QueueSubmit.
			// NOTE: While it is possible for an application to use a separate graphics
			//       and a present queues, this demo program assumes it is only using
			//       one:
			if (graphicsQueueNodeIndex != presentQueueNodeIndex)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Swapchain Initialization Failure", "Could not find a common graphics and a present queue\n", window->handle);
				return SGL_FALSE;
			}
		}
		float queue_priorities[1] = { 0.0 };
		// Create vkDevice
		{
			const VkDeviceQueueCreateInfo queue =
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.pNext = NULL,
				.queueFamilyIndex = graphicsQueueNodeIndex,
				.queueCount = 1,
				.pQueuePriorities = queue_priorities
			};

			VkDeviceCreateInfo device =
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.pNext = NULL,
				.queueCreateInfoCount = 1,
				.pQueueCreateInfos = &queue,
	#ifndef NDEBUG
				.enabledLayerCount = enabledLayerCount,
				.ppEnabledLayerNames = (const char *const *)instanceValidationLayers,
	#else
				.enabledLayerCount = 0,
				.ppEnabledLayerNames = NULL,
	#endif
				.enabledExtensionCount = enabledExtensionCount,
				.ppEnabledExtensionNames = (const char *const *)extensionNames,
				.pEnabledFeatures =
				NULL, // If specific features are required, pass them in here
			};

			err = vkCreateDevice(window->vkSwapChain.gpu, &device, NULL, &window->vkSwapChain.device);
			SDL_assert(!err);
		}
		// Setup device function pointers
		{
			PFN_vkGetDeviceProcAddr gdpa = (PFN_vkGetDeviceProcAddr)vkGetInstanceProcAddr(window->vkSwapChain.inst, "vkGetDeviceProcAddr");

			window->vkSwapChain.fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)gdpa(window->vkSwapChain.device, "vkCreateSwapchainKHR");
			if (window->vkSwapChain.fpCreateSwapchainKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)gdpa(window->vkSwapChain.device, "vkDestroySwapchainKHR");
			if (window->vkSwapChain.fpDestroySwapchainKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)gdpa(window->vkSwapChain.device, "vkGetSwapchainImagesKHR");
			if (window->vkSwapChain.fpGetSwapchainImagesKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)gdpa(window->vkSwapChain.device, "vkAcquireNextImageKHR");
			if (window->vkSwapChain.fpAcquireNextImageKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
			window->vkSwapChain.fpQueuePresentKHR = (PFN_vkQueuePresentKHR)gdpa(window->vkSwapChain.device, "vkQueuePresentKHR");
			if (window->vkSwapChain.fpQueuePresentKHR == NULL)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"vkGetDeviceProcAddr Failure", "vkGetDeviceProcAddr failed to find vk", window->handle);
				return SGL_FALSE;
			}
		}
		// Create vkQueue
		vkGetDeviceQueue(window->vkSwapChain.device, graphicsQueueNodeIndex, 0, &window->vkSwapChain.queue);
		window->vkSwapChain.queueNodeIndex = graphicsQueueNodeIndex;
	}
	// Get surface format
	{
		uint32_t formatCount;
		// Get the list of VkFormat's that are supported:
		err = window->vkSwapChain.fpGetPhysicalDeviceSurfaceFormatsKHR(window->vkSwapChain.gpu, window->vkSwapChain.surf, &formatCount, NULL);
		SDL_assert(!err);
		VkSurfaceFormatKHR *surfFormats = (VkSurfaceFormatKHR *)SDL_malloc(formatCount * sizeof(VkSurfaceFormatKHR));
		err = window->vkSwapChain.fpGetPhysicalDeviceSurfaceFormatsKHR(window->vkSwapChain.gpu, window->vkSwapChain.surf, &formatCount, surfFormats);
		SDL_assert(!err);
		// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
		// the surface has no preferred format.  Otherwise, at least one
		// supported format will be returned.
		if (formatCount == 1 && surfFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			SDL_Log("Surface has no preferred format, VK_FORMAT_B8G8R8A8_UNORM format was set.");
			window->vkSwapChain.colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		}
		else 
		{
			SDL_assert(formatCount >= 1);
			window->vkSwapChain.colorFormat = surfFormats[0].format;
		}
		window->vkSwapChain.colorSpace = surfFormats[0].colorSpace;
		SDL_free(surfFormats);
	}
	{
		VkFormat depthFormats[] = 
		{
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};
		const size_t depthCount = SDL_arraysize(depthFormats);
		for (size_t i = 0; i < depthCount; i++)
		{
			VkFormatProperties formatProps;
			vkGetPhysicalDeviceFormatProperties(window->vkSwapChain.gpu, depthFormats[i], &formatProps);
			// Format must support depth stencil attachment for optimal tiling
			if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				window->vkSwapChain.depthFormat = depthFormats[i];
				break;
			}
		}
		SDL_assert(window->vkSwapChain.depthFormat != VK_FORMAT_UNDEFINED);
	}
	// Get Memory information and properties
	vkGetPhysicalDeviceMemoryProperties(window->vkSwapChain.gpu, &window->vkSwapChain.memoryProperties);
	SDL_Log("Vulkan initialized successfully\n\n");
	//*/
	return SGL_TRUE;
}
static inline VkBool32 memory_type_from_properties(SGL_VkSwapChain * swapchain, U32 typeBits, VkFlags requirements_mask, U32 *typeIndex) 
{
	// Search memtypes to find first index with those properties
	for (uint32_t i = 0; i < 32; i++) 
	{
		if ((typeBits & 1) == 1) 
		{
			// Type is available, does it match user properties?
			if ((swapchain->memoryProperties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) 
			{
				*typeIndex = i;
				return VK_TRUE;
			}
		}
		typeBits >>= 1;
	}
	// No memory types matched, return failure
	return VK_FALSE;
}
static inline VkBool32 CreateBuffer(SGL_VkSwapChain * swapchain, VkBufferUsageFlags usage, VkDeviceSize size, void * data, VkBuffer *buffer, VkDeviceMemory *memory)
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
		.pNext = NULL,
		.usage = usage,
		.size = size,
		.flags = 0
	};

	VkResult err = vkCreateBuffer(swapchain->device, &bufferCreateInfo, VK_NULL_HANDLE, buffer);
	assert(!err);
	vkGetBufferMemoryRequirements(swapchain->device, *buffer, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	memory_type_from_properties(swapchain, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAlloc.memoryTypeIndex);
	err = vkAllocateMemory(swapchain->device, &memAlloc, VK_NULL_HANDLE, memory);
	assert(!err);
	if (data != VK_NULL_HANDLE)
	{
		void *mapped;
		err = vkMapMemory(swapchain->device, *memory, 0, size, 0, &mapped);
		assert(!err);
		memcpy(mapped, data, size);
		vkUnmapMemory(swapchain->device, *memory);
	}
	err = vkBindBufferMemory(swapchain->device, *buffer, *memory, 0);
	assert(!err);
	return VK_TRUE;
}
static inline VkBool32 CreateBufferWD(SGL_VkSwapChain * swapchain, VkBufferUsageFlags usage, VkDeviceSize size, void * data, VkBuffer * buffer, VkDeviceMemory * memory, VkDescriptorBufferInfo * descriptor)
{
	VkBool32 res = CreateBuffer(swapchain, usage, size, data, buffer, memory);
	if (res)
	{
		descriptor->offset = 0;
		descriptor->buffer = *buffer;
		descriptor->range = size;
		return VK_TRUE;
	}
	else
	{
		return VK_FALSE;
	}
}
static inline void InitCommandBuffer(SGL_VkSwapChain* swapchain, VkCommandBuffer* cmdBuffer)
{
	VkResult err;
	const VkCommandBufferAllocateInfo cmd = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = swapchain->cmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	err = vkAllocateCommandBuffers(swapchain->device, &cmd, cmdBuffer);
	SDL_assert(!err);

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
	err = vkBeginCommandBuffer(*cmdBuffer, &cmd_buf_info);
	SDL_assert(!err);
}
static inline void demo_flush_init_cmd(SGL_VkSwapChain * swapchain) 
{
	VkResult err;

	if (swapchain->setupCmdBuffer == VK_NULL_HANDLE)
	{
		int asd = 0;
		return;
	}

	err = vkEndCommandBuffer(swapchain->setupCmdBuffer);
	SDL_assert(!err);

	const VkCommandBuffer cmd_bufs[] = { swapchain->setupCmdBuffer };
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

	err = vkQueueSubmit(swapchain->queue, 1, &submit_info, nullFence);
	SDL_assert(!err);

	err = vkQueueWaitIdle(swapchain->queue);
	SDL_assert(!err);

	vkFreeCommandBuffers(swapchain->device, swapchain->cmdPool, 1, cmd_bufs);
	swapchain->setupCmdBuffer = VK_NULL_HANDLE;
}
//why cant i use the same commandbuffer for everything??
static void demo_set_image_layout(SGL_VkSwapChain* swapchain, VkCommandBuffer* cmdBuffer, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout, VkImageLayout new_image_layout, VkAccessFlagBits srcAccessMask, VkAccessFlagBits dstAccessMask)
{
	VkResult err;

	if (*cmdBuffer == VK_NULL_HANDLE) 
	{
		//SDL_Log("cmdBuffer is NULL, fix the function under this log message");
		const VkCommandBufferAllocateInfo cmd = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = NULL,
			.commandPool = swapchain->cmdPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		err = vkAllocateCommandBuffers(swapchain->device, &cmd, cmdBuffer);
		SDL_assert(!err);

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
		err = vkBeginCommandBuffer(*cmdBuffer, &cmd_buf_info);
		SDL_assert(!err);
	}

	VkImageMemoryBarrier image_memory_barrier =
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = old_image_layout,
		.newLayout = new_image_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = { aspectMask, 0, 1, 0, 1 }
	};
	//switch (new_image_layout)
	//{
	//case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	//	break;
	//}
	//case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//	break;
	//}
	//case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//	break;
	//}
	//case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	//	break;
	//}
	//case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	//	break;
	//}
	//default:
	//	break;
	//}
	//if (new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	//{
	//	/* Make sure anything that was copying from this image has completed */
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	//}
	//
	//if (new_image_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	//}
	//
	//if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) 
	//{
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	//}
	//
	//if (new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	//{
	//	/* Make sure any Copy or CPU writes to image are flushed */
	//	image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
	//}

	VkPipelineStageFlags src_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	VkPipelineStageFlags dest_stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	const char* layoutArray[] = 
	{
		"VK_IMAGE_LAYOUT_UNDEFINED",
		"VK_IMAGE_LAYOUT_GENERAL",
		"VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL",
		"VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL",
		"VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL",
		"VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL",
		"VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL",
		"VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL",
		"VK_IMAGE_LAYOUT_PREINITIALIZED",
	};
	/*
	const char* oldLayout = image_memory_barrier.oldLayout < 9 ? layoutArray[image_memory_barrier.oldLayout] : "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
	const char* newLayout = image_memory_barrier.newLayout < 9 ? layoutArray[image_memory_barrier.newLayout] : "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
	char * dstText = SDL_malloc(sizeof(char) * 256);
	dstText[0] = '0';
	dstText[1] = '\0';
	U32 dstTextIndex = 0;
	{
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_SHADER_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_SHADER_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_SHADER_READ_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_SHADER_WRITE_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_SHADER_WRITE_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_SHADER_WRITE_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_COLOR_ATTACHMENT_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_TRANSFER_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_TRANSFER_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_TRANSFER_READ_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_TRANSFER_WRITE_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_TRANSFER_WRITE_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_TRANSFER_WRITE_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_HOST_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_HOST_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_HOST_READ_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_HOST_WRITE_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_HOST_WRITE_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_HOST_WRITE_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_MEMORY_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_MEMORY_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_MEMORY_READ_BIT", size);
		dstTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_MEMORY_READ_BIT)
	{
		if (dstTextIndex != 0)
		{
			dstText[dstTextIndex++] = ' ';
			dstText[dstTextIndex++] = '|';
			dstText[dstTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_MEMORY_READ_BIT");
		SDL_strlcpy(&dstText[dstTextIndex], "VK_ACCESS_MEMORY_READ_BIT", size);
		dstTextIndex += --size;
	}
	}
	char * srcText = SDL_malloc(sizeof(char) * 256);
	srcText[0] = '0';
	srcText[1] = '\0';
	U32 srcTextIndex = 0;
	{
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_SHADER_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_SHADER_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_SHADER_READ_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_SHADER_WRITE_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_SHADER_WRITE_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_SHADER_WRITE_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_COLOR_ATTACHMENT_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_COLOR_ATTACHMENT_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_TRANSFER_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_TRANSFER_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_TRANSFER_READ_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_TRANSFER_WRITE_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_TRANSFER_WRITE_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_TRANSFER_WRITE_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_HOST_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_HOST_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_HOST_READ_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_HOST_WRITE_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_HOST_WRITE_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_HOST_WRITE_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_MEMORY_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_MEMORY_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_MEMORY_READ_BIT", size);
		srcTextIndex += --size;
	}
	if (image_memory_barrier.srcAccessMask & VK_ACCESS_MEMORY_READ_BIT)
	{
		if (srcTextIndex != 0)
		{
			srcText[srcTextIndex++] = ' ';
			srcText[srcTextIndex++] = '|';
			srcText[srcTextIndex++] = ' ';
		}
		U32 size = sizeof("VK_ACCESS_MEMORY_READ_BIT");
		SDL_strlcpy(&srcText[srcTextIndex], "VK_ACCESS_MEMORY_READ_BIT", size);
		srcTextIndex += --size;
	}
	}
	SDL_Log("\nCreated vkCmdPipelineBarrier:\noldLayout: %s\nnewLayout: %s\nsrcAccessMask: %s\ndstAccessMask: %s\n", oldLayout, newLayout, srcText, dstText);
	//*/

	VkImageMemoryBarrier *pmemory_barrier = &image_memory_barrier;
	vkCmdPipelineBarrier(*cmdBuffer, src_stages, dest_stages, 0, 0, NULL, 0,
		NULL, 1, pmemory_barrier);
}
static void demo_prepare_buffers(SGL_VkSwapChain * swapchain) 
{
	VkResult err;
	VkSwapchainKHR oldSwapchain = swapchain->swapchain;

	// Check the surface capabilities and formats
	VkSurfaceCapabilitiesKHR surfCapabilities;
	err = swapchain->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(
		swapchain->gpu, swapchain->surf, &surfCapabilities);
	SDL_assert(!err);

	uint32_t presentModeCount;
	err = swapchain->fpGetPhysicalDeviceSurfacePresentModesKHR(
		swapchain->gpu, swapchain->surf, &presentModeCount, NULL);
	SDL_assert(!err);
	VkPresentModeKHR *presentModes =
		(VkPresentModeKHR *)malloc(presentModeCount * sizeof(VkPresentModeKHR));
	SDL_assert(presentModes);
	err = swapchain->fpGetPhysicalDeviceSurfacePresentModesKHR(
		swapchain->gpu, swapchain->surf, &presentModeCount, presentModes);
	SDL_assert(!err);

	VkExtent2D swapchainExtent;
	// width and height are either both -1, or both not -1.
	if (surfCapabilities.currentExtent.width == (uint32_t)-1)
	{
		// If the surface size is undefined, the size is set to
		// the size of the images requested.
		swapchainExtent.width = swapchain->windowSize.x;
		swapchainExtent.height = swapchain->windowSize.y;
	}
	else 
	{
		// If the surface size is defined, the swap chain size must match
		swapchainExtent = surfCapabilities.currentExtent;
		swapchain->windowSize.x = surfCapabilities.currentExtent.width;
		swapchain->windowSize.y = surfCapabilities.currentExtent.height;
		swapchain->windowHalfSizef.x = (float)surfCapabilities.currentExtent.width / 2;
		swapchain->windowHalfSizef.y = (float)surfCapabilities.currentExtent.height / 2;
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
		(desiredNumberOfSwapchainImages > surfCapabilities.maxImageCount)) 
	{
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

	const VkSwapchainCreateInfoKHR swapchainCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.pNext = NULL,
		.surface = swapchain->surf,
		.minImageCount = desiredNumberOfSwapchainImages,
		.imageFormat = swapchain->colorFormat,
		.imageColorSpace = swapchain->colorSpace,
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
		.clipped = VK_TRUE,
	};
	err = swapchain->fpCreateSwapchainKHR(swapchain->device, &swapchainCreateInfo, NULL,
		&swapchain->swapchain);
	SDL_assert(!err);

	// If we just re-created an existing swapchain, we should destroy the old
	// swapchain at this point.
	// Note: destroying the swapchain also cleans up all its associated
	// presentable images once the platform is done with them.
	if (oldSwapchain != VK_NULL_HANDLE) {
		swapchain->fpDestroySwapchainKHR(swapchain->device, oldSwapchain, NULL);
	}

	err = swapchain->fpGetSwapchainImagesKHR(swapchain->device, swapchain->swapchain, &swapchain->swapchainImageCount, NULL);
	SDL_assert(!err);

	err = swapchain->fpGetSwapchainImagesKHR(swapchain->device, swapchain->swapchain,
		&swapchain->swapchainImageCount,
		swapchain->swapchainImages);
	SDL_assert(!err);

	U32 i;
	for (i = 0; i < swapchain->swapchainImageCount; i++) 
	{
		VkImageViewCreateInfo color_image_view = 
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.pNext = NULL,
			.format = swapchain->colorFormat,
			.components =
		{
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A,
		},
			.subresourceRange = 
			{ 
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1 
			},
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.flags = 0,
		};
		// Render loop will expect image to have been used before and in
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		// layout and will change to COLOR_ATTACHMENT_OPTIMAL, so init the image
		// to that state
		demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, swapchain->swapchainImages[i], VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 0);

		color_image_view.image = swapchain->swapchainImages[i];

		err = vkCreateImageView(swapchain->device, &color_image_view, NULL,
			&swapchain->swapchainViews[i]);
		SDL_assert(!err);
	}
	//demo_flush_init_cmd(swapchain);
	if (NULL != presentModes) 
	{
		free(presentModes);
	}
}
static void demo_prepare_depth(SGL_VkSwapChain * swapchain)
{
	const VkImageCreateInfo image = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = swapchain->depthFormat,
		.extent = { swapchain->windowSize.x, swapchain->windowSize.y, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
		.flags = 0,
	};

	VkImageViewCreateInfo view = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = NULL,
		.image = VK_NULL_HANDLE,
		.format = swapchain->depthFormat,
		.subresourceRange = 
		{ 
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1 
		},
		.flags = 0,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
	};
	VkMemoryRequirements mem_reqs;
	VkResult err;
	SGL_bool pass;

	/* create image */
	err = vkCreateImage(swapchain->device, &image, NULL, &swapchain->depthStencil.image);
	SDL_assert(!err);

	vkGetImageMemoryRequirements(swapchain->device, swapchain->depthStencil.image, &mem_reqs);
	SDL_assert(!err);
	VkMemoryAllocateInfo mem_alloc =
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = mem_reqs.size,
		.memoryTypeIndex = 0,
	};

	pass = memory_type_from_properties(swapchain, mem_reqs.memoryTypeBits,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, /* No requirements */
		&mem_alloc.memoryTypeIndex);
	SDL_assert(pass);

	/* allocate memory */
	err = vkAllocateMemory(swapchain->device, &mem_alloc, NULL,
		&swapchain->depthStencil.mem);
	SDL_assert(!err);

	/* bind memory */
	err =
		vkBindImageMemory(swapchain->device, swapchain->depthStencil.image, swapchain->depthStencil.mem, 0);
	SDL_assert(!err);
	demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, swapchain->depthStencil.image, VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);

	/* create image view */
	view.image = swapchain->depthStencil.image;
	err = vkCreateImageView(swapchain->device, &view, NULL, &swapchain->depthStencil.view);
	SDL_assert(!err);
}
static void demo_prepare_texture_image(SGL_VkSwapChain *swapchain, SGL_Texture *tex_obj, VkImageTiling tiling, VkImageUsageFlags usage, VkFlags required_props) 
{
	const VkFormat tex_format = tex_obj->format;
	VkResult err;
	SGL_bool pass;

	const VkImageCreateInfo image_create_info = 
	{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = NULL,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = tex_format,
		.extent = { tex_obj->w, tex_obj->h, 1 },
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = tiling,
		.usage = usage,
		.flags = 0,
		.initialLayout = VK_IMAGE_LAYOUT_PREINITIALIZED,
	};

	VkMemoryRequirements mem_reqs;

	err =
		vkCreateImage(swapchain->device, &image_create_info, NULL, &tex_obj->image);
	SDL_assert(!err);

	vkGetImageMemoryRequirements(swapchain->device, tex_obj->image, &mem_reqs);

	tex_obj->mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	tex_obj->mem_alloc.pNext = NULL;
	tex_obj->mem_alloc.allocationSize = mem_reqs.size;
	tex_obj->mem_alloc.memoryTypeIndex = 0;

	pass = memory_type_from_properties(swapchain, mem_reqs.memoryTypeBits,
		required_props,
		&tex_obj->mem_alloc.memoryTypeIndex);
	SDL_assert(pass);

	/* allocate memory */
	err = vkAllocateMemory(swapchain->device, &tex_obj->mem_alloc, NULL,
		&(tex_obj->mem));
	SDL_assert(!err);

	/* bind memory */
	err = vkBindImageMemory(swapchain->device, tex_obj->image, tex_obj->mem, 0);
	SDL_assert(!err);

	if (required_props & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) 
	{
		const VkImageSubresource subres = 
		{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.mipLevel = 0,
			.arrayLayer = 0,
		};
		VkSubresourceLayout layout;
		void *data;

		vkGetImageSubresourceLayout(swapchain->device, tex_obj->image, &subres,
			&layout);

		err = vkMapMemory(swapchain->device, tex_obj->mem, 0,
			tex_obj->mem_alloc.allocationSize, 0, &data);
		SDL_assert(!err);
		SDL_memcpy(data, tex_obj->surf->pixels, tex_obj->mem_alloc.allocationSize);

		vkUnmapMemory(swapchain->device, tex_obj->mem);
	}

	tex_obj->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, tex_obj->image, VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_PREINITIALIZED, tex_obj->imageLayout,
		VK_ACCESS_HOST_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT);
	int g = 0;
	/* setting the image layout does not reference the actual memory so no need
	* to add a mem ref */
}
static void demo_prepare_textures(SGL_VkSwapChain *swapchain) 
{
	{
		SDL_Surface* surf = SGL_DataLoadImage("chimppiVulkant0");
		swapchain->tex.surf = surf;
		swapchain->tex.w = surf->w;
		swapchain->tex.h = surf->h;
		swapchain->tex.mipLevels = 1;
		swapchain->tex.format = SGL_PixelFormatToVkFormat(surf->format->format);
	}
	const VkFormat tex_format = swapchain->tex.format;
	VkFormatProperties props;
	uint32_t i;

	vkGetPhysicalDeviceFormatProperties(swapchain->gpu, tex_format, &props);

	for (i = 0; i < 1; i++) 
	{
		VkResult err;
		if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT) 
		{
			/* Must use staging buffer to copy linear texture to optimized */
			SGL_Texture staging_texture = swapchain->tex;
			


			//memset(&staging_texture, 0, sizeof(staging_texture));
			demo_prepare_texture_image(swapchain, &staging_texture,
				VK_IMAGE_TILING_LINEAR,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			demo_prepare_texture_image(
				swapchain, &swapchain->tex, VK_IMAGE_TILING_OPTIMAL,
				(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, staging_texture.image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				staging_texture.imageLayout,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_READ_BIT);

			demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, swapchain->tex.image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				swapchain->tex.imageLayout,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT, VK_ACCESS_TRANSFER_WRITE_BIT);

			VkImageCopy copy_region = 
			{
				.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
				.srcOffset = { 0, 0, 0 },
				.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
				.dstOffset = { 0, 0, 0 },
				.extent = { staging_texture.w,
				staging_texture.h, 1 },
			};
			vkCmdCopyImage(
				swapchain->setupCmdBuffer, staging_texture.image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapchain->tex.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

			demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, swapchain->tex.image,
				VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				swapchain->tex.imageLayout,
				VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT);

			demo_flush_init_cmd(swapchain);

			//destroy useless texture
			vkFreeMemory(swapchain->device, staging_texture.mem, NULL);
			vkDestroyImage(swapchain->device, staging_texture.image, NULL);
		}
		else {
			/* Can't support VK_FORMAT_R8G8B8A8_UNORM !? */
			SDL_assert(!"No support for current texture format as texture image format");
		}

		const VkSamplerCreateInfo sampler = {
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
		err = vkCreateSampler(swapchain->device, &sampler, NULL,
			&swapchain->tex.sampler);
		SDL_assert(!err);

		/* create image view */
		view.image = swapchain->tex.image;
		err = vkCreateImageView(swapchain->device, &view, NULL,
			&swapchain->tex.view);
		SDL_assert(!err);
	}
}
static void demo_prepare_cube_data_buffer(SGL_VkSwapChain *swapchain)
{

	{
		SGL_Vec4 eye = { 0.0f, 3.0f, 5.0f ,0.0f };
		SGL_Vec4 origin = { 0.0f, 0.0f, 0.0f, 0.0f };
		SGL_Vec4 up = { 0.0f, 1.0f, 0.0f, 0.0f };
		swapchain->spin_angle = 0.01f;
		swapchain->spin_increment = 0.01f;
		float fow = (float)degreesToRadians(45.0f);
		swapchain->projection_matrix = SM_Perspective((float)degreesToRadians(45.0f), swapchain->windowHalfSizef.x/swapchain->windowHalfSizef.y, 0.1f, 100.0f);
		swapchain->view_matrix = SM_LookAt(&eye, &origin, &up);
		swapchain->model_matrix = SM_IdentityMat4();
	}
	VkBufferCreateInfo buf_info;
	VkMemoryRequirements mem_reqs;
	U8 *pData;
	int i;
	SGL_Mat4 MVP, VP;
	VkResult err;
	SGL_bool pass;
	struct vktexcube_vs_uniform data;
	VP = SM_M4Multiply(&swapchain->view_matrix, &swapchain->projection_matrix);
	MVP = SM_M4Multiply(&swapchain->model_matrix, &VP);
	memcpy(data.mvp, &MVP, sizeof(MVP));
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

	err = vkCreateBuffer(swapchain->device, &buf_info, NULL, &swapchain->uniformBuffer.buffer);
	SDL_assert(!err);

	vkGetBufferMemoryRequirements(swapchain->device, swapchain->uniformBuffer.buffer,
		&mem_reqs);
	VkMemoryAllocateInfo mem_alloc = 
	{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = NULL,
		.allocationSize = mem_reqs.size,
		.memoryTypeIndex = 0,
	};
	swapchain->uniformBuffer.allocSize = mem_alloc.allocationSize;

	pass = memory_type_from_properties(swapchain, mem_reqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &mem_alloc.memoryTypeIndex);
	SDL_assert(pass);

	err = vkAllocateMemory(swapchain->device, &mem_alloc, NULL,
		&(swapchain->uniformBuffer.memory));
	SDL_assert(!err);

	err = vkMapMemory(swapchain->device, swapchain->uniformBuffer.memory, 0, swapchain->uniformBuffer.allocSize, 0, (void **)&pData);
	SDL_assert(!err);

	memcpy(pData, &data, sizeof data);

	vkUnmapMemory(swapchain->device, swapchain->uniformBuffer.memory);

	err = vkBindBufferMemory(swapchain->device, swapchain->uniformBuffer.buffer,
		swapchain->uniformBuffer.memory, 0);
	SDL_assert(!err);

	swapchain->uniformBuffer.descriptor.buffer = swapchain->uniformBuffer.buffer;
	swapchain->uniformBuffer.descriptor.offset = 0;
	swapchain->uniformBuffer.descriptor.range = sizeof(data);
}
static void generateQuad(SGL_VkSwapChain *swapchain)
{
	// Setup vertices for a single uv-mapped quad
	static const float vertexData[] =
	{
		 1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-1.0f,-1.0f, 0.0f,	0.0f, 0.0f,
		 1.0f,-1.0f, 0.0f,	1.0f, 0.0f,
	};
	U32 indexData[] = { 0,1,2, 2,3,0 };
	CreateBuffer(swapchain, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, SDL_arraysize(vertexData) * sizeof(float), vertexData, &swapchain->vertexBuffer.buf, &swapchain->vertexBuffer.mem);
	// Setup indices
	swapchain->indexBuffer.count = SDL_arraysize(indexData);
	CreateBuffer(swapchain, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, SDL_arraysize(indexData) * sizeof(U32),indexData, &swapchain->indexBuffer.buf, &swapchain->indexBuffer.mem);
	// Binding description
	swapchain->vertexBuffer.bindingDescriptions = SDL_malloc(sizeof(VkVertexInputBindingDescription));
	swapchain->vertexBuffer.attributeDescriptions = SDL_malloc(sizeof(VkVertexInputAttributeDescription)*2);

	swapchain->vertexBuffer.bindingDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	swapchain->vertexBuffer.bindingDescriptions[0].stride = sizeof(float)*5;
	swapchain->vertexBuffer.bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	// Location 0 : Position
	swapchain->vertexBuffer.attributeDescriptions[0].location = 0;
	swapchain->vertexBuffer.attributeDescriptions[0].binding = VERTEX_BUFFER_BIND_ID;
	swapchain->vertexBuffer.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	swapchain->vertexBuffer.attributeDescriptions[0].offset = 0;

	// Location 1 : Texture coordinates
	swapchain->vertexBuffer.attributeDescriptions[1].location = 1;
	swapchain->vertexBuffer.attributeDescriptions[1].binding = VERTEX_BUFFER_BIND_ID;
	swapchain->vertexBuffer.attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	swapchain->vertexBuffer.attributeDescriptions[1].offset = sizeof(float)*3;

	swapchain->vertexBuffer.inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	swapchain->vertexBuffer.inputState.pNext = VK_NULL_HANDLE;
	swapchain->vertexBuffer.inputState.vertexBindingDescriptionCount = 1;
	swapchain->vertexBuffer.inputState.pVertexBindingDescriptions = swapchain->vertexBuffer.bindingDescriptions;
	swapchain->vertexBuffer.inputState.vertexAttributeDescriptionCount = 2;
	swapchain->vertexBuffer.inputState.pVertexAttributeDescriptions = swapchain->vertexBuffer.attributeDescriptions;
}
static void UpdateUniformBuffers(SGL_VkSwapChain *swapchain)
{
	{
		SGL_Vec4 eye = { 0.0f, 3.0f, 5.0f ,0.0f };
		SGL_Vec4 origin = { 0.0f, 0.0f, 0.0f, 0.0f };
		SGL_Vec4 up = { 0.0f, 1.0f, 0.0f, 0.0f };
		SGL_Mat4 identityM = SM_IdentityMat4();
		swapchain->spin_angle = 0.01f;
		swapchain->spin_increment = 0.01f;
		float fow = (float)degreesToRadians(45.0f);
		swapchain->projection_matrix = SM_Perspective((float)degreesToRadians(45.0f), swapchain->windowHalfSizef.x / swapchain->windowHalfSizef.y, 0.1f, 100.0f);
		swapchain->view_matrix = SM_LookAt(&eye, &origin, &up);
		swapchain->ubo.model = SM_M4Multiply(&swapchain->view_matrix, &identityM);

		swapchain->ubo.projection = swapchain->projection_matrix;
	}
	// Vertex shader
	//glm::mat4 viewMatrix = glm::mat4();
	//uboVS.projection = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.001f, 256.0f);
	//viewMatrix = glm::translate(viewMatrix, glm::vec3(0.0f, 0.0f, zoom));
	//
	//uboVS.model = glm::mat4();
	//uboVS.model = viewMatrix * glm::translate(uboVS.model, glm::vec3(0, 0, 0));
	//uboVS.model = glm::rotate(uboVS.model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	//uboVS.model = glm::rotate(uboVS.model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//uboVS.model = glm::rotate(uboVS.model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	uint8_t *pData;
	swapchain->uniformBuffer.allocSize = sizeof(SGL_UBO);
	VkResult err = vkMapMemory(swapchain->device, swapchain->uniformBuffer.memory, 0, sizeof(SGL_UBO), 0, (void **)&pData);
	assert(!err);
	memcpy(pData, &swapchain->ubo, sizeof(SGL_UBO));
	vkUnmapMemory(swapchain->device, swapchain->uniformBuffer.memory);
}
static void prepareUniformBuffers(SGL_VkSwapChain *swapchain)
{
	// Vertex shader uniform buffer block
	CreateBufferWD(swapchain, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, sizeof(SGL_UBO), &swapchain->ubo, &swapchain->uniformBuffer.buffer, &swapchain->uniformBuffer.memory, &swapchain->uniformBuffer.descriptor);

	UpdateUniformBuffers(swapchain);
}
static void setupDescriptorSetLayout(SGL_VkSwapChain *swapchain)
{
	VkDescriptorSetLayoutBinding setLayoutBindings[] =
	{
		// Binding 0 : Vertex shader uniform buffer
		{
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.binding = 0,
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
		.pNext = NULL,
		.pBindings = setLayoutBindings,
		.bindingCount = SDL_arraysize(setLayoutBindings),
	};

	VkResult err = vkCreateDescriptorSetLayout(swapchain->device, &descriptorLayout, VK_NULL_HANDLE, &swapchain->descriptorSetLayout);
	assert(!err);

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = 
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.setLayoutCount = 1,
		.pSetLayouts = &descriptorLayout
	};

	err = vkCreatePipelineLayout(swapchain->device, &pPipelineLayoutCreateInfo, VK_NULL_HANDLE, &swapchain->pipelineLayout);
	assert(!err);
}

static void demo_prepare_descriptor_layout(SGL_VkSwapChain *swapchain)
{
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
	VkResult err;

	err = vkCreateDescriptorSetLayout(swapchain->device, &descriptor_layout, NULL,
		&swapchain->descriptorSetLayout);
	SDL_assert(!err);

	const VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.pNext = NULL,
		.setLayoutCount = 1,
		.pSetLayouts = &swapchain->descriptorSetLayout,
	};

	err = vkCreatePipelineLayout(swapchain->device, &pPipelineLayoutCreateInfo, NULL,
		&swapchain->pipelineLayout);
	SDL_assert(!err);
}
static void demo_prepare_render_pass(SGL_VkSwapChain * swapchain)
{
	const VkAttachmentDescription attachments[2] = 
	{
		[0] =
	{
		.format = swapchain->colorFormat,
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
		.format = swapchain->depthFormat,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
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
	VkResult err;

	err = vkCreateRenderPass(swapchain->device, &rp_info, NULL, &swapchain->renderPass);
	SDL_assert(!err);
}
VkPipelineShaderStageCreateInfo LoadShader(SGL_VkSwapChain* swapChain, const SGL_Shader* shader, VkShaderStageFlagBits stageBits, const char* pName)
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
	SDL_assert(!err);
	VkPipelineShaderStageCreateInfo shaderStage =
	{
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = stageBits,
		.module = shaderModule,
		.pName = pName
	};
	swapChain->shaderCount++;
	swapChain->shaders = SDL_realloc(swapChain->shaders, sizeof(VkShaderModule)*swapChain->shaderCount);
	swapChain->shaders[swapChain->shaderCount] = shaderModule;
	return shaderStage;
}
static void demo_prepare_pipeline(SGL_VkSwapChain * swapchain) 
{
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
	VkResult err;

	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
	memset(&dynamicState, 0, sizeof dynamicState);
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pDynamicStates = dynamicStateEnables;

	memset(&pipeline, 0, sizeof(pipeline));
	pipeline.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline.layout = swapchain->pipelineLayout;

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
	VkPipelineShaderStageCreateInfo shaderStages[2];
	{
		//SGL_Shader vert = SGL_DataLoadShader("texVert");
		//SGL_Shader frag = SGL_DataLoadShader("texFrag");
		SGL_Shader vert = SGL_DataLoadShader("cubeVert");
		SGL_Shader frag = SGL_DataLoadShader("cubeFrag");
		shaderStages[0] = LoadShader(swapchain, &vert, VK_SHADER_STAGE_VERTEX_BIT, "main");
		shaderStages[1] = LoadShader(swapchain, &frag, VK_SHADER_STAGE_FRAGMENT_BIT, "main");
		SDL_free(vert.data);
		SDL_free(frag.data);
	}
	memset(&pipelineCache, 0, sizeof(pipelineCache));
	pipelineCache.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	err = vkCreatePipelineCache(swapchain->device, &pipelineCache, NULL,
		&swapchain->pipelineCache);
	SDL_assert(!err);

	//pipeline.pVertexInputState = &swapchain->vertexBuffer.inputState;
	pipeline.pVertexInputState = &vi;
	pipeline.pInputAssemblyState = &ia;
	pipeline.pRasterizationState = &rs;
	pipeline.pColorBlendState = &cb;
	pipeline.pMultisampleState = &ms;
	pipeline.pViewportState = &vp;
	pipeline.pDepthStencilState = &ds;
	pipeline.stageCount = 2;
	pipeline.pStages = shaderStages;
	pipeline.renderPass = swapchain->renderPass;
	pipeline.pDynamicState = &dynamicState;

	err = vkCreateGraphicsPipelines(swapchain->device, swapchain->pipelineCache, 1,
		&pipeline, NULL, &swapchain->mainPipeline);
	SDL_assert(!err);
/*
	vkDestroyShaderModule(swapchain->device, swapchain->frag_shader_module, NULL);
	vkDestroyShaderModule(swapchain->device, swapchain->vert_shader_module, NULL);
*/
}

static void demo_prepare_descriptor_pool(SGL_VkSwapChain * swapchain)
{
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
	VkResult err;

	err = vkCreateDescriptorPool(swapchain->device, &descriptor_pool, NULL,
		&swapchain->descriptorPool);
	SDL_assert(!err);
}
static void demo_prepare_descriptor_set(SGL_VkSwapChain * swapchain) 
{
	VkDescriptorImageInfo tex_descs[DEMO_TEXTURE_COUNT];
	VkWriteDescriptorSet writes[2];
	VkResult err;
	uint32_t i;

	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = NULL,
		.descriptorPool = swapchain->descriptorPool,
		.descriptorSetCount = 1,
		.pSetLayouts = &swapchain->descriptorSetLayout };
	err = vkAllocateDescriptorSets(swapchain->device, &alloc_info, &swapchain->descriptorSet);
	SDL_assert(!err);

	memset(&tex_descs, 0, sizeof(tex_descs));
	for (i = 0; i < 1; i++) {
		tex_descs[i].sampler = swapchain->tex.sampler;
		tex_descs[i].imageView = swapchain->tex.view;
		tex_descs[i].imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	}

	memset(&writes, 0, sizeof(writes));

	writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[0].dstSet = swapchain->descriptorSet;
	writes[0].descriptorCount = 1;
	writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writes[0].pBufferInfo = &swapchain->uniformBuffer.descriptor;

	writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writes[1].dstSet = swapchain->descriptorSet;
	writes[1].dstBinding = 1;
	writes[1].descriptorCount = DEMO_TEXTURE_COUNT;
	writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	writes[1].pImageInfo = tex_descs;

	vkUpdateDescriptorSets(swapchain->device, 2, writes, 0, NULL);
}

static void demo_prepare_framebuffers(SGL_VkSwapChain * swapchain)
{
	VkImageView attachments[2];
	attachments[1] = swapchain->depthStencil.view;

	const VkFramebufferCreateInfo fb_info = 
	{
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = NULL,
		.renderPass = swapchain->renderPass,
		.attachmentCount = 2,
		.pAttachments = attachments,
		.width = swapchain->windowSize.x,
		.height = swapchain->windowSize.y,
		.layers = 1,
	};
	VkResult err;
	uint32_t i;

	swapchain->framebuffers = (VkFramebuffer *)malloc(swapchain->swapchainImageCount *
		sizeof(VkFramebuffer));
	SDL_assert(swapchain->framebuffers);

	for (i = 0; i < swapchain->swapchainImageCount; i++) 
	{
		attachments[0] = swapchain->swapchainViews[i];
		err = vkCreateFramebuffer(swapchain->device, &fb_info, NULL,
			&swapchain->framebuffers[i]);
		SDL_assert(!err);
	}
}
static void demo_draw_build_cmd(SGL_VkSwapChain * swapchain, VkCommandBuffer cmd_buf) {
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
		{
			.color.float32 = { 0.2f, 0.2f, 0.2f, 0.2f },
		},
		{
			.depthStencil = { 1.0f, 0 }
		}
	};
	const VkRenderPassBeginInfo rp_begin = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = swapchain->renderPass,
		.framebuffer = swapchain->framebuffers[swapchain->currentBuffer],
		.renderArea.offset.x = 0,
		.renderArea.offset.y = 0,
		.renderArea.extent.width = swapchain->windowSize.x,
		.renderArea.extent.height = swapchain->windowSize.y,
		.clearValueCount = 2,
		.pClearValues = clear_values,
	};
	VkResult err;

	err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
	SDL_assert(!err);

	vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->mainPipeline);
	vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS,
		swapchain->pipelineLayout, 0, 1, &swapchain->descriptorSet, 0,
		NULL);

	VkViewport viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.width = (float)swapchain->windowSize.x;
	viewport.height = (float)swapchain->windowSize.y;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = swapchain->windowSize.x;
	scissor.extent.height = swapchain->windowSize.y;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

	vkCmdDraw(cmd_buf, 12 * 3, 1, 0, 0);
	vkCmdEndRenderPass(cmd_buf);
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	VkImageMemoryBarrier prePresentBarrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapchain->swapchainImages[swapchain->currentBuffer],
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 } 
	};
	vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
		NULL, 1, &prePresentBarrier);

	err = vkEndCommandBuffer(cmd_buf);
	SDL_assert(!err);
}
static void demo_draw_build_cmd_test(SGL_VkSwapChain * swapchain, VkCommandBuffer cmd_buf) 
{
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
		{
			.color.float32 = { 0.2f, 0.2f, 0.2f, 0.2f },
		},
		{
			.depthStencil = { 1.0f, 0 }
		}
	};
	const VkRenderPassBeginInfo rp_begin = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = NULL,
		.renderPass = swapchain->renderPass,
		.framebuffer = swapchain->framebuffers[swapchain->currentBuffer],
		.renderArea.offset.x = 0,
		.renderArea.offset.y = 0,
		.renderArea.extent.width = swapchain->windowSize.x,
		.renderArea.extent.height = swapchain->windowSize.y,
		.clearValueCount = 2,
		.pClearValues = clear_values,
	};
	VkResult err;

	err = vkBeginCommandBuffer(cmd_buf, &cmd_buf_info);
	SDL_assert(!err);

	vkCmdBeginRenderPass(cmd_buf, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->mainPipeline);
	vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, swapchain->pipelineLayout, 0, 1, &swapchain->descriptorSet, 0, VK_NULL_HANDLE);
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(cmd_buf, VERTEX_BUFFER_BIND_ID, 1, &swapchain->vertexBuffer.buf, offsets);
	vkCmdBindIndexBuffer(cmd_buf, swapchain->indexBuffer.buf, 0, VK_INDEX_TYPE_UINT32);

	VkViewport viewport;
	memset(&viewport, 0, sizeof(viewport));
	viewport.width = (float)swapchain->windowSize.x;
	viewport.height = (float)swapchain->windowSize.y;
	viewport.minDepth = (float)0.0f;
	viewport.maxDepth = (float)1.0f;
	vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

	VkRect2D scissor;
	memset(&scissor, 0, sizeof(scissor));
	scissor.extent.width = swapchain->windowSize.x;
	scissor.extent.height = swapchain->windowSize.y;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	vkCmdSetScissor(cmd_buf, 0, 1, &scissor);
	vkCmdDrawIndexed(cmd_buf, swapchain->indexBuffer.count, 1, 0, 0, 0);
	vkCmdEndRenderPass(cmd_buf);
	// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	VkImageMemoryBarrier prePresentBarrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = swapchain->swapchainImages[swapchain->currentBuffer],
		.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
	};
	vkCmdPipelineBarrier(cmd_buf, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
		NULL, 1, &prePresentBarrier);

	err = vkEndCommandBuffer(cmd_buf);
	SDL_assert(!err);
}
U32 SGL_PrepareVulkan(SGL_VkSwapChain * swapchain)
{
	VkResult err;

	const VkCommandPoolCreateInfo cmd_pool_info = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = NULL,
		.queueFamilyIndex = swapchain->queueNodeIndex,
		.flags = 0,
	};
	err = vkCreateCommandPool(swapchain->device, &cmd_pool_info, NULL,
		&swapchain->cmdPool);
	SDL_assert(!err);

	const VkCommandBufferAllocateInfo cmd = 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = NULL,
		.commandPool = swapchain->cmdPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};

	demo_prepare_buffers(swapchain);
	demo_prepare_depth(swapchain);
	demo_prepare_textures(swapchain);
	demo_prepare_cube_data_buffer(swapchain);
	//generateQuad(swapchain);
	//prepareUniformBuffers(swapchain);

	demo_prepare_descriptor_layout(swapchain);
	demo_prepare_render_pass(swapchain);
	demo_prepare_pipeline(swapchain);

	for (U32 i = 0; i < swapchain->swapchainImageCount; i++) 
	{
		err = vkAllocateCommandBuffers(swapchain->device, &cmd, &swapchain->swapchainCmdBuffers[i]);
		SDL_assert(!err);
	}

	demo_prepare_descriptor_pool(swapchain);
	demo_prepare_descriptor_set(swapchain);

	demo_prepare_framebuffers(swapchain);

	for (U32 i = 0; i < swapchain->swapchainImageCount; i++) 
	{
		swapchain->currentBuffer = i;
		//demo_draw_build_cmd_test(swapchain, swapchain->swapchainCmdBuffers[i]);
		demo_draw_build_cmd(swapchain, swapchain->swapchainCmdBuffers[i]);
	}

	//Prepare functions above may generate pipeline commands
	//that need to be flushed before beginning the render loop.
	demo_flush_init_cmd(swapchain);

	swapchain->currentBuffer = 0;
	swapchain->prepared = SGL_TRUE;
	return SGL_TRUE;
}
U32 SGL_UpdateUniforms(SGL_VkSwapChain* swapchain)
{
	SGL_Mat4 MVP, Model, VP;
	int matrixSize = sizeof(SGL_Mat4);
	U8* pData;
	VkResult err;

	VP = SM_M4Multiply(&swapchain->view_matrix, &swapchain->projection_matrix);
	//mat4x4_mul(VP, vkContext->projection_matrix, vkContext->view_matrix);

	// Rotate 22.5 degrees around the Y axis
	Model = swapchain->model_matrix;
	//mat4x4_dup(Model, vkContext->model_matrix);
	SGL_Vec4 up = { 0.0f,1.0f,0.0f,0.0f };
	swapchain->model_matrix = SM_Rotate(&Model, (float)degreesToRadians(swapchain->spin_angle), &up);
	//mat4x4_mul(MVP, VP, vkContext->model_matrix);
	MVP = SM_M4Multiply(&swapchain->model_matrix, &VP);

	err = vkMapMemory(swapchain->device, swapchain->uniformBuffer.memory, 0,
		swapchain->uniformBuffer.allocSize, 0,
		(void **)&pData);
	SDL_assert(!err);

	memcpy(pData, (const void *)&MVP, matrixSize);

	vkUnmapMemory(swapchain->device, swapchain->uniformBuffer.memory);
	return SGL_TRUE;
}
U32 SGL_UpdateUniformsTest(SGL_VkSwapChain* swapchain)
{
	SGL_Mat4 Model;
	U8* pData;
	VkResult err;

	//mat4x4_mul(VP, vkContext->projection_matrix, vkContext->view_matrix);

	// Rotate 22.5 degrees around the Y axis
	Model = swapchain->ubo.model;
	//mat4x4_dup(Model, vkContext->model_matrix);
	SGL_Vec4 up = { 0.0f,1.0f,0.0f,0.0f };
	Model = SM_Rotate(&Model, (float)degreesToRadians(swapchain->spin_angle), &up);
	//mat4x4_mul(MVP, VP, vkContext->model_matrix);
	//MVP = SM_M4Multiply(&swapchain->model_matrix, &VP);

	err = vkMapMemory(swapchain->device, swapchain->uniformBuffer.memory, sizeof(SGL_Mat4), sizeof(SGL_Mat4), 0, (void **)&pData);
	SDL_assert(!err);

	SDL_memcpy(pData, (const void *)&Model, sizeof(SGL_Mat4));

	vkUnmapMemory(swapchain->device, swapchain->uniformBuffer.memory);
	return SGL_TRUE;
}
U32 SGL_Draw(SGL_VkSwapChain * swapchain)
{
	VkResult err = 0;
	VkSemaphore presentCompleteSemaphore;
	VkSemaphoreCreateInfo presentCompleteSemaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = NULL,
		.flags = 0,
	};
	VkFence nullFence = VK_NULL_HANDLE;

	err = vkCreateSemaphore(swapchain->device, &presentCompleteSemaphoreCreateInfo,
		NULL, &presentCompleteSemaphore);
	SDL_assert(!err);

	// Get the index of the next available swapchain image:
	err = swapchain->fpAcquireNextImageKHR(swapchain->device, swapchain->swapchain, UINT64_MAX,
		presentCompleteSemaphore,
		(VkFence)0, // TODO: Show use of fence
		&swapchain->currentBuffer);
	if (err == VK_ERROR_OUT_OF_DATE_KHR)
	{
		// demo->swapchain is out of date (e.g. the window was resized) and
		// must be recreated:
		//	demo_resize(demo);
		//	demo_draw(demo);
		vkDestroySemaphore(swapchain->device, presentCompleteSemaphore, NULL);
		return SGL_FALSE;
	}
	else if (err == VK_SUBOPTIMAL_KHR) {
		// demo->swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
		int g = 0;
	}
	else {
		SDL_assert(!err);
	}

	// Assume the command buffer has been run on current_buffer before so
	// we need to set the image layout back to COLOR_ATTACHMENT_OPTIMAL
	//NOT SURE ABOUT swapChain->setupCmdBuffer
	//InitCommandBuffer(swapchain, &swapchain->setupCmdBuffer);
	demo_set_image_layout(swapchain, &swapchain->setupCmdBuffer, swapchain->swapchainImages[swapchain->currentBuffer],
		VK_IMAGE_ASPECT_COLOR_BIT,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		0, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT);
	demo_flush_init_cmd(swapchain);
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
		&swapchain->swapchainCmdBuffers[swapchain->currentBuffer],
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = NULL };

	err = vkQueueSubmit(swapchain->queue, 1, &submit_info, nullFence);
	SDL_assert(!err);

	VkPresentInfoKHR present = 
	{
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = NULL,
		.swapchainCount = 1,
		.pSwapchains = &swapchain->swapchain,
		.pImageIndices = &swapchain->currentBuffer,
	};

	// TBD/TODO: SHOULD THE "present" PARAMETER BE "const" IN THE HEADER?
	//this causes a winError
	err = swapchain->fpQueuePresentKHR(swapchain->queue, &present);
	//DWORD winError = GetLastError();
	if (err == VK_ERROR_OUT_OF_DATE_KHR) {
		// demo->swapchain is out of date (e.g. the window was resized) and
		// must be recreated:
		//demo_resize(swapchain);
		int gasd = 0;
	}
	else if (err == VK_SUBOPTIMAL_KHR) {
		// demo->swapchain is not as optimal as it could be, but the platform's
		// presentation engine will still present the image correctly.
		int gasd = 0;
	}
	else {
		SDL_assert(!err);
	}

	err = vkQueueWaitIdle(swapchain->queue);
	SDL_assert(err == VK_SUCCESS);

	vkDestroySemaphore(swapchain->device, presentCompleteSemaphore, NULL);
	return SGL_TRUE;
}
