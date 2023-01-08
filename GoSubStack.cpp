#include "GoSubStack.h"

GoSubStack* GoSubStack::instance = nullptr;
GoSubStack* GoSubStack::GetInstance() {
    if (instance == nullptr) {
        instance = new GoSubStack();
    }
    return instance;
}
