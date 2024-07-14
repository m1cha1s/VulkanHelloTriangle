#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 600

static GLFWwindow* window = NULL;

void initVulkan();
void deinitVulkan();

int main() {
    printf("Hello!\n");

    initVulkan();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    deinitVulkan();

    return 0;
}

void initVulkan() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Hello Triangle", NULL, NULL);
}

void deinitVulkan() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
