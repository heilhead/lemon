#pragma once

namespace lemon::res {
    template<class TObject>
    const TObject* ResourceInstance::getObject(ResourceObjectHandle handle) const {
        for (ResourceObject* pObj: objects) {
            if (pObj->getHandle() == handle) {
                return dynamic_cast<TObject*>(pObj);
            }
        }

        return nullptr;
    }

    template<class TObject, typename ...Args>
    void ResourceInstance::createObject(ResourceObjectHandle handle, Args&& ... args) {
        auto* pObj = new TObject(std::forward<Args>(args)...);
        pObj->setHandle(handle);
        objects.push_back(pObj);
    }
}