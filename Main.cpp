#include "VulkanRenderer.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
GLFWwindow* window;
VulkanRenderer vkRenderer;
void initWindow(std::string wname = "test window", const int width = 800, const int height = 600) {
	glfwInit();
	// set GLFW to not work with opengl API
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	window = glfwCreateWindow(width, height, wname.c_str(), nullptr, nullptr);
}

int main() {

	initWindow();
	if (vkRenderer.init(window) == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	vkRenderer.cleanUp();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}