#pragma once

#include <string>
#include <vector>
#include <memory>
#include <tint/tint.h>
#include <lemon/shared/Hash.h>

namespace lemon {
    namespace shader {
        struct StructMember {
            lemon::StringID id;
            std::string name;
            uint32_t offset;
            uint32_t size;
            uint32_t align;
        };

        enum class PipelineStage {
            kNone = 0,
            kVertex = 1 << 0,
            kFragment = 1 << 1,
            kCompute = 1 << 2,
        };

        /// The dimensionality of a texture
        enum class TextureDimension {
            /// Invalid texture
            kNone = -1,
            /// 1 dimensional texture
            k1d,
            /// 2 dimensional texture
            k2d,
            /// 2 dimensional array texture
            k2dArray,
            /// 3 dimensional texture
            k3d,
            /// cube texture
            kCube,
            /// cube array texture
            kCubeArray,
        };

        /// Component type of the texture's data. Same as the Sampled Type parameter
        /// in SPIR-V OpTypeImage.
        enum class SampledKind { kUnknown = -1, kFloat, kUInt, kSInt };

        /// Enumerator of texture image formats
        enum class ImageFormat {
            kNone = -1,
            kR8Unorm,
            kR8Snorm,
            kR8Uint,
            kR8Sint,
            kR16Uint,
            kR16Sint,
            kR16Float,
            kRg8Unorm,
            kRg8Snorm,
            kRg8Uint,
            kRg8Sint,
            kR32Uint,
            kR32Sint,
            kR32Float,
            kRg16Uint,
            kRg16Sint,
            kRg16Float,
            kRgba8Unorm,
            kRgba8UnormSrgb,
            kRgba8Snorm,
            kRgba8Uint,
            kRgba8Sint,
            kBgra8Unorm,
            kBgra8UnormSrgb,
            kRgb10A2Unorm,
            kRg11B10Float,
            kRg32Uint,
            kRg32Sint,
            kRg32Float,
            kRgba16Uint,
            kRgba16Sint,
            kRgba16Float,
            kRgba32Uint,
            kRgba32Sint,
            kRgba32Float,
        };

        /// kXXX maps to entries returned by GetXXXResourceBindings call.
        enum class ResourceType {
            kUniformBuffer,
            kStorageBuffer,
            kReadOnlyStorageBuffer,
            kSampler,
            kComparisonSampler,
            kSampledTexture,
            kMultisampledTexture,
            kReadOnlyStorageTexture,
            kWriteOnlyStorageTexture,
            kDepthTexture,
            kDepthMultisampledTexture,
            kExternalTexture
        };

        /// Note: This is extracted from `tint` inspector code, with some additions.
        /// Container for information about how a resource is bound.
        struct ResourceBindingDescriptor {
            /// Hashed string ID.
            lemon::StringID id;
            /// Declaration variable name.
            std::string name;
            /// Pipeline stages that use the resource.
            PipelineStage stage;
            /// Type of resource that is bound.
            ResourceType resourceType;
            /// Bind group the binding belongs
            uint32_t bindGroup;
            /// Identifier to identify this binding within the bind group
            uint32_t binding;
            /// Size for this binding, in bytes, if defined.
            uint32_t size;
            /// Size for this binding without trailing structure padding, in bytes, if
            /// defined.
            uint32_t sizeNoPadding;
            /// Align for this binding, in bytes, if defined.
            uint32_t align;
            /// Dimensionality of this binding, if defined.
            TextureDimension dim;
            /// Kind of data being sampled, if defined.
            SampledKind sampledKind;
            /// Format of data, if defined.
            ImageFormat imageFormat;
            /// Struct member information.
            std::vector<StructMember> members;
        };

        class ShaderReflection {
            tint::Program program;

        public:
            ShaderReflection(const std::string& codePath, const std::string& code);

            bool
            isValid() const;

            std::string
            getDiagnostic() const;

            std::vector<ResourceBindingDescriptor>
            getBindingReflection() const;
        };
    } // namespace shader
} // namespace lemon
