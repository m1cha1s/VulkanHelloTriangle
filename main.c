#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <vulkan/vulkan_core.h>

//#define VK_USE_PLATFORM_X11_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #define GLFW_EXPOSE_NATIVE_X11
// #include <GLFW/glfw3native.h>

#define WIDTH 800
#define HEIGHT 600

static GLFWwindow* Win = NULL;
static VkInstance Instance = { 0 };
static VkPhysicalDevice PhyDev = VK_NULL_HANDLE;
static VkDevice Dev;
static VkQueue GraphicsQueue = VK_NULL_HANDLE;
static VkSurfaceKHR Surface = VK_NULL_HANDLE;

void initVulkan();
void deinitVulkan();

int main() {
    printf("Hello!\n");

    initVulkan();

    while (!glfwWindowShouldClose(Win)) {
        glfwPollEvents();
    }

    deinitVulkan();

    return 0;
}

static uint32_t FindQueueFamilies() {
    uint32_t Idx = 0;

    uint32_t QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &QueueFamilyCount, NULL);
    VkQueueFamilyProperties QueueFamilies[QueueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(PhyDev, &QueueFamilyCount, QueueFamilies);

    uint32_t CurrIdx = 1; // Starting to index from 1 so that 0 can act like "NULL".
    for (uint32_t i = 0; i < QueueFamilyCount; i++) {
        if (QueueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            Idx = CurrIdx;
        }

        if (Idx) break; // Short path to quit when found all queues.

        CurrIdx++;
    }

    return Idx;
}

static bool IsDeviceSuitable() {
#if 0 // NOTE(m1cha1s): We just want a Vulkan capable GPU.
    VkPhysicalDeviceProperties DevProps = { 0 };
    VkPhysicalDeviceFeatures DevFeatures = { 0 };
    vkGetPhysicalDeviceProperties(PhyDev, &DevProps);
    vkGetPhysicalDeviceFeatures(PhyDev, &DevFeatures);
    return DevProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && DevFeatures.geometryShader;
#else
    uint32_t QueueIdx = FindQueueFamilies();
    return QueueIdx; // Check if not 0.
#endif
}

void initVulkan() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    Win = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Hello Triangle", NULL, NULL);

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

    // --- Create logical device ---

    uint32_t GraphicsIdx = FindQueueFamilies();

    float QueuePriority = 1.0f;

    VkDeviceQueueCreateInfo QueueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = GraphicsIdx,
        .queueCount = 1,
        .pQueuePriorities = &QueuePriority,
    };

    VkPhysicalDeviceFeatures DevFeatures = { 0 };
    vkGetPhysicalDeviceFeatures(PhyDev, &DevFeatures);

    VkDeviceCreateInfo DevCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &QueueCreateInfo,
        .queueCreateInfoCount = 1,
        .pEnabledFeatures = &DevFeatures,
        .enabledExtensionCount = 0,
        .enabledLayerCount = 0,
    };

    if (vkCreateDevice(PhyDev, &DevCreateInfo, NULL, &Dev) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create a logical device.");
        exit(6942);
    }

    vkGetDeviceQueue(Dev, GraphicsIdx, 0, &GraphicsQueue); // Retriving the queue to accualy do something with it.

    // VkDisplaySurfaceCreateInfoKHR SurfCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR,
    // };
}

void deinitVulkan() {
    printf("B\n");
    vkDestroyDevice(Dev, NULL);
    vkDestroyInstance(Instance, NULL);

    glfwDestroyWindow(Win);
    glfwTerminate();

    printf("Bye!\n");
}
