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
U32 SGL_InitVulkan(SGL_Window * window)
{
	VkResult err;
	// Init vkSwapChain struct
	for (U32 i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		window->vkSwapChain.cameras[i].camType = 0;
	}
	memset(&window->vkSwapChain, 0, sizeof(SGL_VkSwapChain));
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
				assert(!err);
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
		assert(!err);

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
			assert(!err);

			if (deviceExtensionCount > 0)
			{
				VkExtensionProperties *deviceExtensions = SDL_malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
				err = vkEnumerateDeviceExtensionProperties(window->vkSwapChain.gpu, NULL, &deviceExtensionCount, deviceExtensions);
				assert(!err);
				for (uint32_t i = 0; i < deviceExtensionCount; i++)
				{
					if (!SDL_strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, deviceExtensions[i].extensionName))
					{
						swapchainExtFound = 1;
						extensionNames[enabledExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
					}
					assert(enabledExtensionCount < 64);
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
			assert(!err);
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
	SDL_Log("Vulkan initialized successfully\n\n");//*/
	return SGL_TRUE;
}
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
