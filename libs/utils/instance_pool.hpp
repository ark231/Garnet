#ifndef GARNET_LIBS_UTILS_INSTANCE_POOL
#define GARNET_LIBS_UTILS_INSTANCE_POOL
#include <cstddef>
#include <vector>
namespace Garnet {
template <typename T>
class InstancePool {
    InstancePool() = default;
    ~InstancePool() = default;

   public:
    InstancePool(InstancePool&) = delete;
    InstancePool(InstancePool&&) = delete;

    static InstancePool& instance() {
        static InstancePool a;
        return a;
    }

    static T* aquire() {
        if (instance().pool_.empty()) {
            return new T();
        } else {
            auto result = instance().pool_.back();
            instance().pool_.pop_back();
            return result;
        }
    }
    static void return_instance(T* instance) { InstancePool::instance().pool_.push_back(instance); }

   private:
    std::vector<T*> pool_;
};
}  // namespace Garnet
#endif
