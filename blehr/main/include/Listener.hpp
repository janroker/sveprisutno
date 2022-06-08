#pragma once

#include <Observable.hpp>
#include <esp_log.h>

template<class... ParamTypes>
class Observable;

template<class... ParamTypes>
class Listener {
public:
    virtual void notify(Observable<ParamTypes...> &observable, ParamTypes... params) = 0;

private:
    Observable<ParamTypes...> &observable;
protected:
    Listener(Observable<ParamTypes...> &observable);

    ~Listener() = default;
};

template<class... ParamTypes>
Listener<ParamTypes ...>::Listener(Observable<ParamTypes...> &observable) : observable(observable) {
    ESP_LOGI("Listener", "Constructor");
}

