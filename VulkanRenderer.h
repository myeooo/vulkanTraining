#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <vector>
#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();
	~VulkanRenderer();
	void cleanUp();
	int init(GLFWwindow* window);
private:
	struct {
		VkPhysicalDevice physicalDevicde;
		VkDevice logicalDevice;
	}mainDevice;
	VkInstance instance;
	VkAllocationCallbacks* g_Allocator = nullptr;
	GLFWwindow* window;
	// create function
	void createInstanse();
	// get function
	void getPhysicalDevice();
	// support functions
	bool checkInstanceExtensionSupport(std::vector<const char*>* checkExtension);
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// getter function
	QueueFamilyindices getQueueFamilies(VkPhysicalDevice device);
};

