#pragma once

#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <lemon/resource/common.h>
#include <lemon/resource/ResourceInstance.h>
#include <lemon/resource/ResourceMetadata.h>
#include <lemon/resource/types/model/LemonModel.h>

namespace lemon::res {
    class ModelResource : public ResourceInstance {
    public:
        /////////////////////////////////////////////////////////////////////////////////////
        // BEGIN Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        struct Metadata : ResourceMetadataBase {
            template<class TArchive>
            void
            serialize(TArchive& ar) {
                ResourceMetadataBase::serialize(ar);
            }
        };

        LEMON_RESOURCE_TRAITS(ModelResource);

        /////////////////////////////////////////////////////////////////////////////////////
        // END Resource traits
        /////////////////////////////////////////////////////////////////////////////////////

        /// <summary>
        /// A simple wrapper around `model::ModelNode` that maps mesh pointers directly.
        ///
        /// Note: This code assumes that `model::LemonModel` is immutable at this point, as
        /// it takes pointers to `std::vector` elements and assumes they won't be relocated.
        /// </summary>
        class Model : public ResourceObject {
        public:
            Model(const model::LemonModel* pModel, const model::ModelNode* pNode) {
                node = pNode;
                meshes.reserve(pNode->meshes.size());
                for (auto idx : pNode->meshes) {
                    meshes.push_back(&pModel->meshes[idx]);
                }
            }

        private:
            const model::ModelNode* node;
            folly::small_vector<const model::ModelMesh*, kObjectCapacity> meshes;

        public:
            inline const std::string&
            getName() const {
                return node->name;
            }

            inline const folly::small_vector<const model::ModelMesh*, kObjectCapacity>&
            getMeshes() const {
                return meshes;
            }
        };

    public:
        ModelResource();
        ~ModelResource() override;

        VoidTask<ResourceLoadingError>
        load(ResourceMetadata& meta) override;

    private:
        std::unique_ptr<model::LemonModel> data;
    };
} // namespace lemon::res
