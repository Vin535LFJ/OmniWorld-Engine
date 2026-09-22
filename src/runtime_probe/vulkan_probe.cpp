#include "vulkan_probe.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace ow {

namespace {

constexpr std::int32_t kVkSuccess = 0;
constexpr std::uint32_t kMaxMemoryTypes = 32;
constexpr std::uint32_t kMaxMemoryHeaps = 16;

#ifdef _WIN32
#define OW_VKAPI_CALL __stdcall
#else
#define OW_VKAPI_CALL
#endif

struct VkInstance_T;
struct VkPhysicalDevice_T;
using VkInstance = VkInstance_T*;
using VkPhysicalDevice = VkPhysicalDevice_T*;

using PfnGetInstanceProcAddr = void* (OW_VKAPI_CALL*)(void*, const char*);
using PfnCreateInstance = std::int32_t (OW_VKAPI_CALL*)(const void*, const void*, VkInstance*);
using PfnEnumerateInstanceExtensionProperties =
    std::int32_t (OW_VKAPI_CALL*)(const char*, std::uint32_t*, void*);
using PfnEnumerateInstanceLayerProperties = std::int32_t (OW_VKAPI_CALL*)(std::uint32_t*, void*);
using PfnEnumeratePhysicalDevices =
    std::int32_t (OW_VKAPI_CALL*)(VkInstance, std::uint32_t*, VkPhysicalDevice*);
using PfnGetPhysicalDeviceProperties = void (OW_VKAPI_CALL*)(VkPhysicalDevice, void*);
using PfnGetPhysicalDeviceMemoryProperties = void (OW_VKAPI_CALL*)(VkPhysicalDevice, void*);
using PfnGetPhysicalDeviceQueueFamilyProperties =
    void (OW_VKAPI_CALL*)(VkPhysicalDevice, std::uint32_t*, void*);
using PfnDestroyInstance = void (OW_VKAPI_CALL*)(VkInstance, const void*);

struct VkApplicationInfoC {
    std::int32_t sType;
    const void* pNext;
    const char* pApplicationName;
    std::uint32_t applicationVersion;
    const char* pEngineName;
    std::uint32_t engineVersion;
    std::uint32_t apiVersion;
};

struct VkInstanceCreateInfoC {
    std::int32_t sType;
    const void* pNext;
    std::uint32_t flags;
    const VkApplicationInfoC* pApplicationInfo;
    std::uint32_t enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    std::uint32_t enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
};

struct VkExtensionPropertiesC {
    char extensionName[256];
    std::uint32_t specVersion;
};

struct VkLayerPropertiesC {
    char layerName[256];
    std::uint32_t specVersion;
    std::uint32_t implementationVersion;
    char description[256];
};

struct VkPhysicalDevicePropertiesC {
    std::uint32_t apiVersion;
    std::uint32_t driverVersion;
    std::uint32_t vendorID;
    std::uint32_t deviceID;
    std::int32_t deviceType;
    char deviceName[256];
    std::uint8_t pipelineCacheUUID[16];
    std::uint8_t tail[1024];
};

struct VkMemoryTypeC {
    std::uint32_t propertyFlags;
    std::uint32_t heapIndex;
};

struct VkMemoryHeapC {
    std::uint64_t size;
    std::uint32_t flags;
};

struct VkPhysicalDeviceMemoryPropertiesC {
    std::uint32_t memoryTypeCount;
    VkMemoryTypeC memoryTypes[kMaxMemoryTypes];
    std::uint32_t memoryHeapCount;
    VkMemoryHeapC memoryHeaps[kMaxMemoryHeaps];
};

struct VkQueueFamilyPropertiesC {
    std::uint32_t queueFlags;
    std::uint32_t queueCount;
    std::uint32_t timestampValidBits;
    std::uint32_t minImageTransferGranularity[3];
};

std::string vk_api_version_string(std::uint32_t v) {
    return std::to_string((v >> 22) & 0x7ff) + "." + std::to_string((v >> 12) & 0x3ff) + "." +
           std::to_string(v & 0xfff);
}

std::string vk_driver_version_string(std::uint32_t vendor, std::uint32_t v) {
    char raw[16];
    std::snprintf(raw, sizeof(raw), "0x%08x", v);
    if (vendor == 0x10de) {
        return std::to_string(v >> 22) + "." + std::to_string((v >> 14) & 0x3ff) + "." +
               std::to_string(v & 0x3fff) + " (raw " + raw + ")";
    }
    if (vendor == 0x8086 || vendor == 0x1002 || vendor == 0x10ee) {
        return std::to_string(v >> 22) + "." + std::to_string((v >> 12) & 0x3ff) + "." +
               std::to_string(v & 0xfff) + " (raw " + raw + ")";
    }
    return raw;
}

std::string vk_device_type_string(std::int32_t t) {
    switch (t) {
        case 1: return "integrated_gpu";
        case 2: return "discrete_gpu";
        case 3: return "virtual_gpu";
        case 4: return "cpu";
        default: return "other";
    }
}

std::string vk_queue_flags_string(std::uint32_t f) {
    std::string out;
    if (f & 0x1) out += "graphics|";
    if (f & 0x2) out += "compute|";
    if (f & 0x4) out += "transfer|";
    if (f & 0x8) out += "sparse|";
    if (f & 0x10) out += "protected|";
    if (out.empty()) {
        return "none";
    }
    out.pop_back();
    return out;
}

void* load_vulkan_library(std::string& path) {
#ifdef _WIN32
    HMODULE module = LoadLibraryA("vulkan-1.dll");
    if (module == nullptr) {
        return nullptr;
    }
    char buffer[MAX_PATH] = {};
    if (GetModuleFileNameA(module, buffer, MAX_PATH) > 0) {
        path = buffer;
    }
    return reinterpret_cast<void*>(module);
#else
    void* module = dlopen("libvulkan.so.1", RTLD_NOW | RTLD_LOCAL);
    if (module == nullptr) {
        module = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    }
    if (module != nullptr) {
        path = "libvulkan.so.1";
    }
    return module;
#endif
}

void* get_symbol(void* module, const char* name) {
#ifdef _WIN32
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(module), name));
#else
    return dlsym(module, name);
#endif
}

}

section probe_vulkan(bool& ok) {
    ok = false;
    section s{"vulkan"};

    std::string loader_path = "unknown";
    void* module = load_vulkan_library(loader_path);
    if (module == nullptr) {
        s.add_field("status", "failed");
        s.add_field("error", "vulkan loader library not found");
        return s;
    }
    s.add_field("loader", loader_path);

    auto vkGetInstanceProcAddr =
        reinterpret_cast<PfnGetInstanceProcAddr>(get_symbol(module, "vkGetInstanceProcAddr"));
    if (vkGetInstanceProcAddr == nullptr) {
        s.add_field("status", "failed");
        s.add_field("error", "vkGetInstanceProcAddr not found in loader");
        return s;
    }
    s.add_field("status", "ok");

    auto vkEnumerateInstanceLayerProperties = reinterpret_cast<PfnEnumerateInstanceLayerProperties>(
        vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
    auto vkEnumerateInstanceExtensionProperties =
        reinterpret_cast<PfnEnumerateInstanceExtensionProperties>(
            vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));
    auto vkCreateInstance =
        reinterpret_cast<PfnCreateInstance>(vkGetInstanceProcAddr(nullptr, "vkCreateInstance"));

    if (vkEnumerateInstanceLayerProperties != nullptr) {
        std::uint32_t count = 0;
        std::int32_t r = vkEnumerateInstanceLayerProperties(&count, nullptr);
        if (r == kVkSuccess && count > 0) {
            std::vector<VkLayerPropertiesC> layers(count);
            r = vkEnumerateInstanceLayerProperties(&count, layers.data());
            bool has_validation = false;
            for (std::uint32_t i = 0; i < count; ++i) {
                auto& layer = s.add_child("instance_layer");
                layer.add_field("name", layers[i].layerName);
                layer.add_field("description", layers[i].description);
                layer.add_field("spec_version", vk_api_version_string(layers[i].specVersion));
                if (std::strcmp(layers[i].layerName, "VK_LAYER_KHRONOS_validation") == 0) {
                    has_validation = true;
                }
            }
            s.add_field("instance_layer_count", std::to_string(count));
            s.add_field("validation_layers",
                        has_validation ? "present (VK_LAYER_KHRONOS_validation)" : "absent");
        } else {
            s.add_field("instance_layer_count", "0");
            s.add_field("validation_layers", "absent");
        }
    }

    if (vkEnumerateInstanceExtensionProperties != nullptr) {
        std::uint32_t count = 0;
        std::int32_t r = vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
        if (r == kVkSuccess && count > 0) {
            std::vector<VkExtensionPropertiesC> extensions(count);
            vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
            std::string names;
            for (std::uint32_t i = 0; i < count; ++i) {
                names += extensions[i].extensionName;
                if (i + 1 < count) {
                    names += " ";
                }
            }
            s.add_field("instance_extension_count", std::to_string(count));
            s.add_field("instance_extensions", names);
        }
    }

    if (vkCreateInstance == nullptr) {
        s.add_field("error", "vkCreateInstance not found");
        return s;
    }

    VkApplicationInfoC app{};
    app.sType = 0;
    app.pApplicationName = "runtime_probe";
    app.pEngineName = "omniworld";
    app.apiVersion = 0;
    VkInstanceCreateInfoC create{};
    create.sType = 1;
    create.pApplicationInfo = &app;

    VkInstance instance = nullptr;
    const std::int32_t r = vkCreateInstance(&create, nullptr, &instance);
    if (r != kVkSuccess || instance == nullptr) {
        s.add_field("status", "failed");
        s.add_field("error", "vkCreateInstance failed with VkResult " + std::to_string(r));
        return s;
    }

    auto vkEnumeratePhysicalDevices = reinterpret_cast<PfnEnumeratePhysicalDevices>(
        vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    auto vkGetPhysicalDeviceProperties = reinterpret_cast<PfnGetPhysicalDeviceProperties>(
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties"));
    auto vkGetPhysicalDeviceMemoryProperties =
        reinterpret_cast<PfnGetPhysicalDeviceMemoryProperties>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties"));
    auto vkGetPhysicalDeviceQueueFamilyProperties =
        reinterpret_cast<PfnGetPhysicalDeviceQueueFamilyProperties>(
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
    auto vkDestroyInstance =
        reinterpret_cast<PfnDestroyInstance>(vkGetInstanceProcAddr(instance, "vkDestroyInstance"));

    std::uint32_t device_count = 0;
    if (vkEnumeratePhysicalDevices == nullptr ||
        vkEnumeratePhysicalDevices(instance, &device_count, nullptr) != kVkSuccess ||
        device_count == 0) {
        s.add_field("physical_device_count", "0");
        if (vkDestroyInstance != nullptr) {
            vkDestroyInstance(instance, nullptr);
        }
        return s;
    }
    s.add_field("physical_device_count", std::to_string(device_count));
    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    for (std::uint32_t i = 0; i < device_count; ++i) {
        auto& d = s.add_child("physical_device_" + std::to_string(i));
        d.add_field("index", std::to_string(i));

        if (vkGetPhysicalDeviceProperties != nullptr) {
            VkPhysicalDevicePropertiesC props{};
            vkGetPhysicalDeviceProperties(devices[i], &props);
            d.add_field("name", props.deviceName);
            d.add_field("type", vk_device_type_string(props.deviceType));
            char vid[16];
            char did[16];
            std::snprintf(vid, sizeof(vid), "0x%04x", props.vendorID);
            std::snprintf(did, sizeof(did), "0x%04x", props.deviceID);
            d.add_field("vendor_id", vid);
            d.add_field("device_id", did);
            d.add_field("api_version", vk_api_version_string(props.apiVersion));
            d.add_field("driver_version", vk_driver_version_string(props.vendorID, props.driverVersion));
        }

        if (vkGetPhysicalDeviceQueueFamilyProperties != nullptr) {
            std::uint32_t family_count = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &family_count, nullptr);
            d.add_field("queue_family_count", std::to_string(family_count));
            if (family_count > 0) {
                std::vector<VkQueueFamilyPropertiesC> families(family_count);
                vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &family_count, families.data());
                for (std::uint32_t f = 0; f < family_count && f < 16; ++f) {
                    d.add_field("queue_family_" + std::to_string(f),
                                vk_queue_flags_string(families[f].queueFlags) + " x" +
                                    std::to_string(families[f].queueCount));
                }
            }
        }

        if (vkGetPhysicalDeviceMemoryProperties != nullptr) {
            VkPhysicalDeviceMemoryPropertiesC mem{};
            vkGetPhysicalDeviceMemoryProperties(devices[i], &mem);
            d.add_field("memory_type_count", std::to_string(mem.memoryTypeCount));
            for (std::uint32_t t = 0; t < mem.memoryTypeCount && t < kMaxMemoryTypes; ++t) {
                char flags[16];
                std::snprintf(flags, sizeof(flags), "0x%02x", mem.memoryTypes[t].propertyFlags);
                d.add_field("memory_type_" + std::to_string(t),
                            std::string("property_flags ") + flags + " heap " +
                                std::to_string(mem.memoryTypes[t].heapIndex));
            }
            d.add_field("memory_heap_count", std::to_string(mem.memoryHeapCount));
            std::uint64_t device_local = 0;
            for (std::uint32_t h = 0; h < mem.memoryHeapCount && h < kMaxMemoryHeaps; ++h) {
                const bool is_device_local = (mem.memoryHeaps[h].flags & 0x1) != 0;
                if (is_device_local) {
                    device_local += mem.memoryHeaps[h].size;
                }
                d.add_field("heap_" + std::to_string(h),
                            fmt_gib(mem.memoryHeaps[h].size) +
                                (is_device_local ? " device_local" : " host"));
            }
            d.add_field("device_local_memory", fmt_gib(device_local));
        }
    }

    if (vkDestroyInstance != nullptr) {
        vkDestroyInstance(instance, nullptr);
    }
    ok = true;
    return s;
}

}
