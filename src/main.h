#pragma once

#include <iostream>

#include <vector>
#include <string>
#include <string_view>
#include <optional> // C++17, for device queue querying
#include <set>
#include <array> // for returning arrays of things

#include <cstring> // for strcmp

#include "glm_wrapper.h"
#include "camera.h"
#include "vloader.h"

using std::cout;
using std::cerr;

class appvk {
public:
	void run() {
		init();
		loop();
		cleanup();
	}

private:

	constexpr static unsigned int screenWidth = 3840;
	constexpr static unsigned int screenHeight = 2160;

	constexpr static bool verbose = false;
	constexpr static bool shader_debug = false;

#ifndef NDEBUG
	constexpr static bool debug = true;
#else
	constexpr static bool debug = false;
#endif
	
	GLFWwindow* w;
	
	bool resizeOccurred = false;

	VkSurfaceKHR surf = VK_NULL_HANDLE;

    static void windowSizeCallback(GLFWwindow* w, int width, int height);

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    void createWindow();
    void checkValidation();
    const std::vector<const char*> getExtensions();

    static VKAPI_ATTR VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT mSev,
        VkDebugUtilsMessageTypeFlagsEXT mType,
        const VkDebugUtilsMessengerCallbackDataEXT* data,
        void* userData);
    
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    void populateDebugMessenger(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
    void setupDebugMessenger();

    VkInstance instance = VK_NULL_HANDLE;
    void createInstance();
	
	VkPhysicalDevice pdev = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples;

	const std::vector<const char*> requiredExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME,
	};

    enum manufacturer { nvidia, intel, any };

    bool checkDeviceExtensions(VkPhysicalDevice pdev);
    VkSampleCountFlagBits getSamples(unsigned int try_samples);
    void checkChooseDevice(VkPhysicalDevice pd, manufacturer m);
    void pickPhysicalDevice(manufacturer m);

	struct queueIndices {
		std::optional<uint32_t> graphics;
		std::optional<uint32_t> compute;
		std::optional<uint32_t> transfer;
	};

    struct swapChainSupportDetails {
		VkSurfaceCapabilitiesKHR cap;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

    queueIndices findQueueFamily(VkPhysicalDevice pd);
    swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice pdev);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formatList);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& modeList);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& cap);
	
	VkDevice dev = VK_NULL_HANDLE;
	VkQueue gQueue = VK_NULL_HANDLE;
    void createLogicalDevice();
	
	VkSwapchainKHR swap = VK_NULL_HANDLE;
	std::vector<VkImage> swapImages;
	VkFormat swapFormat;
	VkExtent2D swapExtent;
    std::vector<VkImageView> swapImageViews;
    void createSurface();
	void createSwapChain();
    void createSwapViews();

    VkFormat findImageFormat(const std::vector<VkFormat>& formats, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkImageView createImageView(VkImage im, VkFormat format, unsigned int mipLevels, VkImageAspectFlags aspectMask);
	
	VkRenderPass renderPass = VK_NULL_HANDLE;

	VkFormat depthFormat;

    void createRenderPass();

	struct ubo {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformMemories;
	void createUniformBuffers();

    VkDescriptorSetLayout dSetLayout = VK_NULL_HANDLE;
    void createDescriptorSetLayout();

    VkDescriptorPool dPool = VK_NULL_HANDLE;
    void createDescriptorPool();

    std::vector<VkDescriptorSet> dSet;
    void allocDescriptorSets();

	std::vector<char> readFile(const std::string& path);
    VkShaderModule createShaderModule(const std::vector<char>& spv);
	
	VkPipelineLayout pipeLayout = VK_NULL_HANDLE;
	VkPipeline gpipe = VK_NULL_HANDLE;
	void createGraphicsPipeline();

	bool printed = false;
    void printShaderStats();

	std::vector<VkFramebuffer> swapFramebuffers; // ties render attachments to image views in the swapchain
    void createFramebuffers();

	VkCommandPool cp = VK_NULL_HANDLE;
	void createCommandPool();
	
	uint32_t findMemoryType(uint32_t legalMemoryTypes, VkMemoryPropertyFlags properties);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags props, VkBuffer& buf, VkDeviceMemory& bufMem);

    VkCommandBuffer beginSingleCommand();
    void endSingleCommand(VkCommandBuffer buf);

	void createImage(unsigned int width, unsigned int height, VkFormat format, unsigned int mipLevels, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkImageLayout oldl, VkImageLayout newl, unsigned int mipLevels);
    
    void copyBufferToImage(VkBuffer buf, VkImage img, uint32_t width, uint32_t height);
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	VkBuffer stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

	VkBuffer vertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vertexMemory = VK_NULL_HANDLE;
    void createVertexBuffer(const std::vector<vload::vertex>& verts);

	VkBuffer indexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory indexMemory = VK_NULL_HANDLE;
    void createIndexBuffer(const std::vector<uint32_t>& indices);

	VkImage texImage = VK_NULL_HANDLE;
	VkDeviceMemory texMem = VK_NULL_HANDLE;
    VkImageView texView = VK_NULL_HANDLE;
	VkSampler texSamp = VK_NULL_HANDLE;
	unsigned int texMipLevels;
	void createTextureImage(std::string_view path);

    void createSampler();
	void generateMipmaps(VkImage image, VkFormat format, unsigned int width, unsigned int height, unsigned int levels);

	VkImage depthImage = VK_NULL_HANDLE;
	VkDeviceMemory depthMemory = VK_NULL_HANDLE;
	VkImageView depthView = VK_NULL_HANDLE;
    void createDepthImage();

	VkImage msImage = VK_NULL_HANDLE;
	VkDeviceMemory msMemory = VK_NULL_HANDLE;
	VkImageView msImageView = VK_NULL_HANDLE;
    void createMultisampleImage();
	
	std::vector<VkCommandBuffer> commandBuffers;
	
	void allocRenderCmdBuffers(uint32_t numIndices);

	constexpr static unsigned int framesInFlight = 2;

	// swapchain image acquisition requires a binary semaphore since it might be hard for implementations to do timeline semaphores
	std::vector<VkSemaphore> imageAvailSems; // use seperate semaphores per frame so we can send >1 frame at once
	std::vector<VkSemaphore> renderDoneSems;
	std::vector<VkFence> inFlightFences; // use fences so we actually wait until a frame completes before moving on to the next one
	std::vector<VkFence> imagesInFlight; // track frames in flight because acquireNextImageKHR may not return swapchain indices in order
    void createSyncs();

	uint32_t numIndices = 0;

    void init();
    void recreateSwapChain();

	// this scene is set up so that the camera is in -Z looking towards +Z.
    cam::camera c;
	glm::vec3 cpos = glm::vec3(0.0, 0.0, -3.0);
	
    void updateUniformBuffer(uint32_t imageIndex);

	size_t currFrame = 0;

	void drawFrame();
    void loop();

    void cleanupSwapChain();
    void cleanup();
};