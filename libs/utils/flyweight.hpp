#ifndef GARNET_LIBS_UTILS_FLYWEIGHT
#define GARNET_LIBS_UTILS_FLYWEIGHT
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>
namespace Garnet {
class SimpleFlyWeight {
    SimpleFlyWeight() = default;
    std::vector<std::string> pool_;

   public:
    static SimpleFlyWeight& instance() {
        static SimpleFlyWeight a;
        return a;
    }
    using value_type = std::string;
    using id_type = std::size_t;

    id_type id(const std::string& value) {
        // とりあえずシンプルに線形探索
        // ソースファイルを保存するだけなら、構築よりも参照のほうが圧倒的に
        // 多いので、構築が多少非効率でも参照が爆速なほうが良い
        auto pos = std::ranges::find(pool_, value);
        if (pos == pool_.end()) {
            pool_.push_back(value);
            return pool_.size() - 1;
        }
        return std::distance(pool_.begin(), pos);
    }
    std::string& value(id_type id) { return pool_[id]; }
};
}  // namespace Garnet
#endif
