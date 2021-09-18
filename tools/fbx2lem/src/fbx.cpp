#include "fbx.h"

#define AI_CONFIG_PP_SBP_REMOVE aiPrimitiveType_POINT | aiPrimitiveType_LINE

#include <vector>
#include <cstdint>
#include <algorithm>
#include <limits>

#include <glm/glm.hpp>

#include <lemon/resources.h>
#include <lemon/utils/utils.h>
#include <lemon/scheduler.h>
#include <lemon/serialization.h>
#include <lemon/serialization/glm.h>
#include <lemon/serialization/DataBuffer.h>
#include <lemon/resource/types/model/LemonModel.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Logger.hpp>
#include <assimp/DefaultLogger.hpp>

using namespace lemon::res::model;

template<class T>
std::enable_if_t<std::is_integral_v<T>, T>
fpack(float v) {
    return static_cast<T>(std::numeric_limits<T>::max() * v);
}

template<typename T, size_t N, glm::qualifier Q = glm::qualifier::defaultp>
glm::vec<N, T, Q>
fpack(const glm::vec<N, float, Q>& v) {
    if constexpr (N == 1) {
        return glm::vec<N, T, Q>(fpack<T>(v.x));
    } else if constexpr (N == 2) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y));
    } else if constexpr (N == 3) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y), fpack<T>(v.z));
    } else if constexpr (N == 4) {
        return glm::vec<N, T, Q>(fpack<T>(v.x), fpack<T>(v.y), fpack<T>(v.z), fpack<T>(v.w));
    }
}

glm::vec3
fvec3(const aiVector3D& v) {
    return glm::vec3(v.x, v.y, v.z);
}

glm::vec2
fvec2(const aiVector2D& v) {
    return glm::vec2(v.x, v.y);
}

glm::vec2
fvec2(const aiVector3D& v) {
    return glm::vec2(v.x, v.y);
}

glm::mat4
fmat4(const aiMatrix4x4& m) {
    // N.B. Convert from `aiMatrix4x4` row-major order to `glm::mat4` column-major order.

    // clang-format off
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
    // clang-format on
}

struct BuildVertexData {
    glm::f32vec3 position;    // => float32x3
    glm::f32vec3 normal;      // => snorm8x3
    glm::f32vec3 tangent;     // => snorm8x3
    glm::f32vec2 texCoord[2]; // => unorm16x2
    glm::u8vec4 jointIndex;   // => uint8x4
    glm::f32vec4 jointWeight; // => unorm8x4

    MeshPackedVertex
    pack(MeshComponents components) const {
        MeshPackedVertex v;

        if (enum_integer(components & MeshComponents::Position)) {
            v.position = position;
        }

        if (enum_integer(components & MeshComponents::Normal)) {
            v.normal = fpack<int8_t>(normal);
        }

        if (enum_integer(components & MeshComponents::Tangent)) {
            v.tangent = fpack<int8_t>(tangent);
        }

        if (enum_integer(components & MeshComponents::UV0)) {
            v.uv0 = fpack<uint16_t>(texCoord[0]);
        }

        if (enum_integer(components & MeshComponents::UV1)) {
            v.uv1 = fpack<uint16_t>(texCoord[0]);
        }

        if (enum_integer(components & MeshComponents::JointInfluence)) {
            v.jointIndex = jointIndex;
            v.jointWeight = fpack<uint8_t>(jointWeight);
        }

        return v;
    }
};

struct BuildMeshData {
    MeshComponents components;
    std::vector<BuildVertexData> vertices;
    std::vector<uint32_t> indices;
    std::optional<std::vector<glm::mat4>> joints;

    BuildMeshData(const aiMesh* mesh) {
        size_t numUVTracks = mesh->GetNumUVChannels();

        if (numUVTracks > 2) {
            lemon::utils::halt("maximum supported number of texture coordinate tracks is 2");
        }

        if (mesh->mNumBones > 256) {
            lemon::utils::halt("maximum supported number of bones is 256");
        }

        components = MeshComponents::Position;

        if (mesh->HasNormals()) {
            components |= MeshComponents::Normal;
        }

        if (mesh->HasTangentsAndBitangents()) {
            components |= MeshComponents::Tangent;
        }

        if (numUVTracks > 0) {
            components |= MeshComponents::UV0;
        }

        if (numUVTracks == 2) {
            components |= MeshComponents::UV1;
        }

        if (mesh->HasBones()) {
            components |= MeshComponents::JointInfluence;
        }

        size_t numVertices = mesh->mNumVertices;

        vertices.resize(numVertices);

        // 1. Vertex data.
        for (auto i = 0; i < numVertices; i++) {
            auto& v = vertices[i];

            v.position = fvec3(mesh->mVertices[i]);

            if (mesh->HasNormals()) {
                v.normal = fvec3(mesh->mNormals[i]);
            }

            if (mesh->HasTangentsAndBitangents()) {
                v.tangent = fvec3(mesh->mTangents[i]);
            }

            for (auto j = 0; j < numUVTracks; j++) {
                if (mesh->mNumUVComponents[j] != 2) {
                    lemon::utils::halt("only 2D UV coordinates are supported");
                }

                v.texCoord[j] = fvec2(mesh->mTextureCoords[j][i]);
            }
        }

        // 2. Index buffer.
        for (auto i = 0; i < mesh->mNumFaces; i++) {
            auto face = mesh->mFaces[i];

            if (face.mNumIndices != 3) {
                lemon::utils::halt("only triangle faces are supported");
            }

            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        // 3. Joint influence data.
        if (mesh->HasBones()) {
            std::vector<glm::mat4> jointData;

            for (uint8_t jointIndex = 0; jointIndex < mesh->mNumBones; jointIndex++) {
                auto joint = mesh->mBones[jointIndex];

                jointData.emplace_back(fmat4(joint->mOffsetMatrix));

                for (auto j = 0; j < joint->mNumWeights; j++) {
                    auto& weightData = joint->mWeights[j];
                    auto weight = weightData.mWeight;
                    auto& v = vertices[weightData.mVertexId];

                    for (auto k = 0; k < 4; k++) {
                        if (v.jointWeight[k] == 0.f) {
                            v.jointWeight[k] = weight;
                            v.jointIndex[k] = jointIndex;
                            break;
                        } else if (k == 3) {
                            lemon::utils::halt("vertex influenced by more than 4 joints");
                        }
                    }
                }
            }

            joints = std::move(jointData);
        } else {
            joints = {};
        }
    }

    template<class Archive>
    inline void
    CEREAL_SAVE_FUNCTION_NAME(Archive& ar) const {
        MeshIndexFormat indexFormat = vertices.size() > std::numeric_limits<uint16_t>::max()
                                          ? MeshIndexFormat::U32
                                          : MeshIndexFormat::U16;

        LEMON_SERIALIZE(ar, components);
        LEMON_SERIALIZE(ar, indexFormat);

        size_t vbSize = vertices.size() * MeshPackedVertex::getSize(components);
        LEMON_SERIALIZE(ar, vbSize);

        for (auto& v : vertices) {
            v.pack(components).serialize(ar, components);
        }

        size_t ibSize = indices.size() * enum_integer(indexFormat);
        LEMON_SERIALIZE(ar, ibSize);

        for (uint32_t idx32 : indices) {
            if (indexFormat == MeshIndexFormat::U16) {
                uint16_t idx16 = static_cast<uint16_t>(idx32);
                LEMON_SERIALIZE(ar, idx16);
            } else {
                LEMON_SERIALIZE(ar, idx32);
            }
        }

        LEMON_SERIALIZE(ar, joints);
    }
};

struct BuildModel {
    std::vector<BuildMeshData> meshes;
    std::vector<ModelNode> nodes;

    BuildModel(const aiScene* scene) {
        for (int i = 0; i < scene->mNumMeshes; i++) {
            auto* mesh = scene->mMeshes[i];
            if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
                lemon::utils::printErr("mesh ", i,
                                       " contains unsupported primitive type: ", mesh->mPrimitiveTypes);
                lemon::utils::halt(nullptr);
            }

            lemon::utils::print("exporting mesh [", i, "] vertices: ", mesh->mNumVertices,
                                " skinned: ", mesh->HasBones() ? "yes" : "no");

            addMesh(mesh);
        }

        findNodes(scene->mRootNode);
    }

    void
    addMesh(const aiMesh* mesh) {
        meshes.emplace_back(BuildMeshData(mesh));
    }

    void
    addNode(const aiNode* node) {
        lemon::utils::print("exporting node [", node->mName.C_Str(), "] meshes: ", node->mNumMeshes);

        ModelNode data;
        data.transform = glm::mat4(1.0);
        data.name = std::string(node->mName.C_Str());

        for (auto i = 0; i < node->mNumMeshes; i++) {
            data.meshes.push_back(node->mMeshes[i]);
        }

        while (node != nullptr) {
            data.transform = fmat4(node->mTransformation) * data.transform;
            node = node->mParent;
        }

        nodes.emplace_back(data);
    }

    void
    findNodes(aiNode* node) {
        if (node->mNumMeshes > 0) {
            addNode(node);
        }

        for (int i = 0; i < node->mNumChildren; i++) {
            findNodes(node->mChildren[i]);
        }
    }

    template<class Archive>
    inline void
    CEREAL_SAVE_FUNCTION_NAME(Archive& ar) const {
        LEMON_SERIALIZE(ar, meshes);
        LEMON_SERIALIZE(ar, nodes);
    }
};

void
lemon::converter::convert(std::filesystem::path inFile, std::filesystem::path outFile) {
    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
    Assimp::Importer importer;

    // aiProcess_FlipUVs ??
    // aiProcess_FlipWindingOrder ??
    // aiProcess_GenBoundingBoxes ??
    // aiProcess_TransformUVCoords ??

    const auto ppFlags =
        aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType | aiProcess_LimitBoneWeights | aiProcess_ValidateDataStructure |
        aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_OptimizeMeshes |
        aiProcess_GenUVCoords | aiProcess_FindInstances | aiProcess_OptimizeGraph;

    lemon::utils::print("building model: [", outFile, "]");
    lemon::utils::print("source: [", inFile, "]");

    const aiScene* scene = importer.ReadFile(inFile.string(), ppFlags);
    BuildModel model(scene);

    {
        std::ofstream os{outFile, std::ios::binary};
        cereal::BinaryOutputArchive archive(os);
        archive(model);
    }

    {
        std::ifstream is{outFile, std::ios::binary};
        cereal::BinaryInputArchive archive(is);
        LemonModel testModel;
        archive(testModel);
    }

    Assimp::DefaultLogger::kill();
}