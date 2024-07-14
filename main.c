#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 600

static GLFWwindow* Window = NULL;
static VkInstance Instance = { 0 };
static VkPhysicalDevice PhyDev = VK_NULL_HANDLE;

void initVulkan();
void deinitVulkan();

int main() {
    printf("Hello!\n");

    initVulkan();

    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
    }

    deinitVulkan();

    return 0;
}

static bool IsDeviceSuitable(VkPhysicalDevice Dev) {
#if 0 // NOTE(m1cha1s): We just want a Vulkan capable GPU.
    VkPhysicalDeviceProperties DevProps = { 0 };
    VkPhysicalDeviceFeatures DevFeatures = { 0 };
    vkGetPhysicalDeviceProperties(PhyDev, &DevProps);
    vkGetPhysicalDeviceFeatures(PhyDev, &DevFeatures);
    return DevProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && DevFeatures.geometryShader;
#else
    return true;
#endif
}

static uint32_t FindQueueFams() {

}

void initVulkan() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Hello Triangle", NULL, NULL);

    // --- Vulkan shmoo start ---

    VkApplicationInfo AppInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Vulkan Hello Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "No Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };

    uint32_t glfwExtCount = 0;
    const char** glfwExts = NULL;

    glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

    VkInstanceCreateInfo CreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &AppInfo,
        .enabledExtensionCount = glfwExtCount,
        .ppEnabledExtensionNames = glfwExts,
        .enabledLayerCount = 0,
    };

    VkResult Result = vkCreateInstance(&CreateInfo, NULL, &Instance);
    assert(Result == VK_SUCCESS && "VK Init failed!");
    // NOTE(m1cha1s): Skipping potential driver issue: VK_ERROR_INCOMPATIBLE_DRIVER

    uint32_t ExtCount = 0;
    vkEnumerateInstanceExtensionProperties(NULL, &ExtCount, NULL);
    VkExtensionProperties exts[ExtCount];
    vkEnumerateInstanceExtensionProperties(NULL, &ExtCount, exts);

    for (uint32_t i = 0; i < ExtCount; i++) {
        printf("\t%s\n", exts[i].extensionName);
    }

    // --- Physical devices ---

    uint32_t DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, NULL);
    if (!DeviceCount) {
        fprintf(stderr, "Failed to find a Vulkan capable GPU.");
        exit(69);
    }

    VkPhysicalDevice Devices[DeviceCount];
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices);

    // NOTE(m1cha1s): Choosing the first device instead of the "best" one.
    for (uint32_t i = 0; i < DeviceCount; i++) {
        if (IsDeviceSuitable(Devices[i])) {
            PhyDev = Devices[i];
            break;
        }
    }

    if (PhyDev == VK_NULL_HANDLE) {
        fprintf(stderr, "Failed to find a suitable GPU.");
        exit(420);
    }


}

void deinitVulkan() {
    vkDestroyInstance(Instance, NULL);

    glfwDestroyWindow(Window);
    glfwTerminate();

    printf("Bye!\n");
}
