#pragma once

#include <iostream>
#include <vector>
#include <string_view>
#include <optional> // C++17, for device queue querying
#include <utility> // for std::pair
#include <tuple>

#include "vformat.hpp"

#include "glm_mat_wrapper.hpp"
#include "camera.hpp"
#include "terrain.hpp"

using std::cout;
using std::cerr;

class appvk {
public:

	appvk();
	~appvk();

	void run();

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

    constexpr static std::array<const char*, 1> validationLayers = {
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

	constexpr static std::array<const char*, 2> requiredExtensions = {
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

	struct mvp {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	std::vector<VkBuffer> mvpBuffers;
	std::vector<VkDeviceMemory> mvpMemories;
	void createUniformBuffers();

    VkDescriptorSetLayout dSetLayout = VK_NULL_HANDLE;
	VkDescriptorSetLayout skySetLayout = VK_NULL_HANDLE;
    void createDescriptorSetLayouts();

    VkDescriptorPool dPool = VK_NULL_HANDLE;
	VkDescriptorPool skyPool = VK_NULL_HANDLE;
    void createDescriptorPools();

    std::vector<VkDescriptorSet> terrainSet;
	std::vector<VkDescriptorSet> grassSet;
	std::vector<VkDescriptorSet> skySet;
	void allocDescriptorSets(VkDescriptorPool pool, std::vector<VkDescriptorSet>& dSet, VkDescriptorSetLayout layout);
    void allocDescriptorSetUniform(std::vector<VkDescriptorSet>& dSet);
	void allocDescriptorSetTexture(std::vector<VkDescriptorSet>& dSet, VkSampler samp, VkImageView view);

	std::vector<char> readFile(std::string_view path);
    VkShaderModule createShaderModule(const std::vector<char>& spv);
	
	VkPipelineLayout terrainPipeLayout = VK_NULL_HANDLE;
	VkPipeline terrainPipe = VK_NULL_HANDLE;

	VkPipelineLayout skyPipeLayout = VK_NULL_HANDLE;
	VkPipeline skyPipe = VK_NULL_HANDLE;

	VkPipeline grassPipe = VK_NULL_HANDLE;
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
	void createCubeImage(unsigned int width, unsigned int height, VkFormat format, VkSampleCountFlagBits samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags props, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createCubeImageView(VkImage im, VkFormat format);
	void transitionImageLayout(VkImage image, VkImageLayout oldl, VkImageLayout newl, unsigned int mipLevels, unsigned int layers);
    
    void copyBufferToImage(VkBuffer buf, VkImage img, uint32_t width, uint32_t height, uint32_t layers);
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

	VkBuffer terrainVertBuf = VK_NULL_HANDLE;
	VkDeviceMemory terrainVertMem = VK_NULL_HANDLE;

	VkBuffer grassVertBuf = VK_NULL_HANDLE;
	VkDeviceMemory grassVertMem = VK_NULL_HANDLE;

	VkBuffer skyVertBuf = VK_NULL_HANDLE;
	VkDeviceMemory skyVertMem = VK_NULL_HANDLE;

	VkBuffer grassVertInstBuf = VK_NULL_HANDLE;
	VkDeviceMemory grassVertInstMem = VK_NULL_HANDLE;
    std::pair<VkBuffer, VkDeviceMemory> createVertexBuffer(std::vector<vformat::vertex>& v);
	std::pair<VkBuffer, VkDeviceMemory> createVertexBuffer(const std::vector<uint8_t>& verts);

	VkBuffer terrainIndBuf = VK_NULL_HANDLE;
	VkDeviceMemory terrainIndMem = VK_NULL_HANDLE;
    std::pair<VkBuffer, VkDeviceMemory> createIndexBuffer(const std::vector<uint32_t>& indices);

	VkImage terrainImage = VK_NULL_HANDLE;
	VkDeviceMemory terrainMem = VK_NULL_HANDLE;
    VkImageView terrainView = VK_NULL_HANDLE;
	VkSampler terrainSamp = VK_NULL_HANDLE;
	unsigned int terrainMipLevels;

	VkImage grassImage = VK_NULL_HANDLE;
	VkDeviceMemory grassMem = VK_NULL_HANDLE;
    VkImageView grassView = VK_NULL_HANDLE;
	VkSampler grassSamp = VK_NULL_HANDLE;
	unsigned int grassMipLevels;

	VkImage cubeImage = VK_NULL_HANDLE;
	VkDeviceMemory cubeMem = VK_NULL_HANDLE;
    VkImageView cubeView = VK_NULL_HANDLE;
	VkSampler cubeSamp = VK_NULL_HANDLE;
	std::tuple<VkImage, VkDeviceMemory, unsigned int> createTextureImage(std::string_view path, bool flip);
	std::tuple<VkImage, VkDeviceMemory> createCubemapImage(std::array<std::string_view, 6> paths, bool flip);

    VkSampler createSampler(unsigned int mipLevels);
	VkSampler createCubeSampler();
	void generateMipmaps(VkImage image, VkFormat format, unsigned int width, unsigned int height, unsigned int levels, unsigned int layers);

	VkImage depthImage = VK_NULL_HANDLE;
	VkDeviceMemory depthMemory = VK_NULL_HANDLE;
	VkImageView depthView = VK_NULL_HANDLE;
    void createDepthImage();

	VkImage msImage = VK_NULL_HANDLE;
	VkDeviceMemory msMemory = VK_NULL_HANDLE;
	VkImageView msImageView = VK_NULL_HANDLE;
    void createMultisampleImage();
	
	std::vector<VkCommandBuffer> commandBuffers;
	
	uint32_t terrainIndices;
	uint32_t grassVertices;
	uint32_t grassInstances;
	uint32_t grassIndices;
	uint32_t skyVertices;
	void allocRenderCmdBuffers();

	constexpr static unsigned int framesInFlight = 2;

	// swapchain image acquisition requires a binary semaphore since it might be hard for implementations to do timeline semaphores
	std::vector<VkSemaphore> imageAvailSems; // use seperate semaphores per frame so we can send >1 frame at once
	std::vector<VkSemaphore> renderDoneSems;
	std::vector<VkFence> inFlightFences; // use fences so we actually wait until a frame completes before moving on to the next one
	std::vector<VkFence> imagesInFlight; // track frames in flight because acquireNextImageKHR may not return swapchain indices in order
    void createSyncs();

    void recreateSwapChain();

	// this scene is set up so that the camera is in -Z looking towards +Z.
    cam::camera c;
	ter::terrain t;

	std::vector<glm::mat4> grassMatBuf;
	void initGrass(const std::vector<vformat::vertex>& verts, const std::vector<uint32_t>& indices);
	
    void updateUniformBuffer(uint32_t imageIndex);

	size_t currFrame = 0;

	void drawFrame();

    void cleanupSwapChain();
    void cleanup();
};