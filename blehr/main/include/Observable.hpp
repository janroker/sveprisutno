#pragma once

#undef max
#undef min

#include <forward_list>

#undef max
#undef min

#include <Listener.hpp>

#include <esp_log.h>

template<class... ParamTypes>
class Listener;

template<class... ParamTypes>
class Observable {
public:
    static constexpr char *tag {"Observer"};

    void registerListener(Listener<ParamTypes...> &listener);
    void unregisterListener(Listener<ParamTypes...> &listener);

private:
    std::forward_list<Listener<ParamTypes...> *> listeners;

protected:
    Observable() = default;
    void trigger(ParamTypes... args);
};

template<class... ParamTypes>
void Observable<ParamTypes...>::registerListener(Listener<ParamTypes...> &listener) {
    ESP_LOGI(tag, "registerListener");

    this->listeners.push_front(&listener);
}

template<class... ParamTypes>
void Observable<ParamTypes...>::unregisterListener(Listener<ParamTypes...> &listener) {
    ESP_LOGI(tag, "unregisterListener");

    this->listeners.remove(&listener);
}

template<class... ParamTypes>
void Observable<ParamTypes...>::trigger(ParamTypes ...args) {
    // int i = 0;
    for (auto listener: this->listeners) {
        //++i;
        //ESP_LOGI(tag, "trigger %d", i);

        listener->notify(*this, std::forward<ParamTypes>(args)...);
    }
}

