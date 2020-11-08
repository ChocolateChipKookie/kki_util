//
// Created by kookie on 30. 10. 2020..
//

#ifndef KKI_UTIL_UTIL_H
#define KKI_UTIL_UTIL_H

#include <memory>
#include <mutex>

namespace kki{

    template <typename T>
    using ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    inline ref<T> make_ref(Args&&... args){
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    template <typename T_obj, typename T_mu>
    class lockable{
    public:
        template<typename... Args>
        explicit lockable(Args&&... args) : _obj(args...){}

        T_obj& get(){
            return _obj;
        }

        T_obj& operator()(){
            return get();
        }

        std::mutex& get_mutex(){
            return _mu;
        }

    private:
        T_mu _mu;
        T_obj _obj;
    };
}


#endif //KKI_UTIL_UTIL_H
