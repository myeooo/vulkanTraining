#include "VulkanRenderer.h"
#include <iostream>
VulkanRenderer::VulkanRenderer()
{
	window = nullptr;
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::cleanUp()
{
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, g_Allocator);
}

int VulkanRenderer::init(GLFWwindow* nwindow)
{
	window = nwindow;
	try{
		createInstanse();
		getPhysicalDevice();
		createLogicalDevice();
	}
	catch (const std::runtime_error& e) {
		std::cout << "error " << e.what();
		return EXIT_FAILURE;
	}
	return 0;
}



void VulkanRenderer::createLogicalDevice()
{
	// get the queue family indices for the chosen Physical Device
	QueueFamilyindices indices = getQueueFamilies(mainDevice.physicalDevicde);
	
	// Queue the logical devidce need to create and info to do so 
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;
	float priority = 1.0f;
	queueCreateInfo.pQueuePriorities = &priority;		

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = 1;						// number of queue create Infos
	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	
	VkPhysicalDeviceFeatures deviceFeature = {};
	deviceCreateInfo.pEnabledFeatures = &deviceFeature;

	// create logical device for the given physical device
	VkResult ret = vkCreateDevice(mainDevice.physicalDevicde, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
	if (ret != VK_SUCCESS) {
		throw std::runtime_error("failed to create a Logical Device");
	}
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
}

void VulkanRenderer::createInstanse()
{
	// Information about application itself
	// Most data here doesn't affect the program and is for developer convinience

	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);		// Custom name of app version
	appInfo.pEngineName = "No Engine";						// Cusstom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);		// Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_0;				//The Vulkan Version

	//creation information for a Vkinstance (vulkan instance)
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();
	// set up extension instance will use
	uint32_t glfwExtensionCount = 0;						//GLFW may require multiple extensions

	// Get glfw extensions
	const char**  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// add glfw extension to list extensions 
	for (size_t i = 0; i < glfwExtensionCount; i++) {
		instanceExtensions.push_back(glfwExtensions[i]);
	}
	// check Instance Extensiong support
	if (!checkInstanceExtensionSupport(&instanceExtensions)) {
		throw std::runtime_error("vkInstance does not support required extensions!");

	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	// create instance 

	VkResult result = vkCreateInstance(&createInfo, g_Allocator, &instance);
	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to create vulkan instance");
	}
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtension)
{
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	//create a list of vkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	for (const auto& checkExtension : *checkExtension) {
		bool hasExtension = false;
		for (const auto& extension : extensions) {
			if (strcmp(checkExtension, extension.extensionName)) {
				hasExtension = true;
				break;
			}
		}
		if (!hasExtension) {
			return false;
		}
	}
	return true;
}

void VulkanRenderer::getPhysicalDevice()
{
	//Enumirate physical device the vkinstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	// if no device available, then none support vulkan
	if (deviceCount == 0) {
		throw std::runtime_error("cant find GPUs that support vulkan instance");
	}

	// get list of physical device 

	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	mainDevice.physicalDevicde = deviceList[0];
	for (const auto& device : deviceList) {
		if (checkDeviceSuitable(device)) {
			mainDevice.physicalDevicde = device;
			break;
		}
	}
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	//// information about the device itself (ID, name, type, vendor, etc)
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//// information about what the device can do (geo shader, tess shader, wide lines, etc)
	//VkPhysicalDeviceFeatures deviceFeature;
	//vkGetPhysicalDeviceFeatures(device, &deviceFeature);
	QueueFamilyindices indices = getQueueFamilies(device);

	return indices.isValid();
}

QueueFamilyindices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyindices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());
	int i = 0;
	for (const auto& queueFamily : queueFamilyList) {
		// first check if queue family has at least 1 queue in that family (could have no queues)
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;				// if queue family is valid, then get index
		}
		if (indices.isValid()) {
			break;
		}
		i++;
	}
	return indices;
}
