//
// Created by kookie on 30. 10. 2020..
//

#ifndef KKI_UTIL_UTIL_H
#define KKI_UTIL_UTIL_H

#include <memory>

namespace kki{

    template <typename T>
    using ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    inline ref<T> make_ref(Args&&... args){
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}


#endif //KKI_UTIL_UTIL_H
