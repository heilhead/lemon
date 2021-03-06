#include <lemon/shader/ShaderReflection.h>
#include <lemon/shared/logger.h>
#include <cstdio>
#include <magic_enum.hpp>

// Enable access to tint's internals
#undef INCLUDE_TINT_TINT_H_

#include <src/ast/module.h>
#include <src/ast/pipeline_stage.h>
#include <src/sem/function.h>
#include <src/sem/struct.h>

using namespace magic_enum::bitwise_operators;
using namespace lemon::shader;

using TintSemVariable = const tint::sem::Variable*;

struct BindingPoint {
    uint32_t group;
    uint32_t binding;

    bool
    operator==(const BindingPoint& other) const
    {
        return (group == other.group && binding == other.binding);
    }
};

struct VariableDescriptor {
    PipelineStage stage;
    TintSemVariable variable;
};

struct BindingDescriptor {
    BindingPoint bindingPoint;
    VariableDescriptor variable;
    tint::inspector::ResourceBinding resource;
};

namespace std {
    template<>
    struct hash<BindingPoint> {
        size_t
        operator()(const BindingPoint& k) const
        {
            auto hashGroup = std::hash<uint32_t>{}(k.group);
            auto hashBinding = std::hash<uint32_t>{}(k.binding);
            return hashGroup ^ (hashBinding << 1);
        }
    };
} // namespace std

PipelineStage
getPipelineStage(tint::inspector::EntryPoint& entryPoint)
{
    switch (entryPoint.stage) {
    case tint::ast::PipelineStage::kVertex:
        return PipelineStage::kVertex;
    case tint::ast::PipelineStage::kFragment:
        return PipelineStage::kFragment;
    case tint::ast::PipelineStage::kCompute:
        return PipelineStage::kCompute;
    default:
        return PipelineStage::kNone;
    }
}

ResourceBindingDescriptor
createBindingReflection(const tint::Program& program, const BindingPoint& bindingPoint,
                        const VariableDescriptor& variable, const tint::inspector::ResourceBinding& resource)
{
    ResourceBindingDescriptor result;
    result.stage = variable.stage;
    result.resourceType = (ResourceType)resource.resource_type;
    result.bindGroup = resource.bind_group;
    result.binding = resource.binding;
    result.dim = (TextureDimension)resource.dim;
    result.sampledKind = (SampledKind)resource.sampled_kind;
    result.imageFormat = (ImageFormat)resource.image_format;

    auto& symbols = program.Symbols();
    auto var = variable.variable;
    auto* unwrappedType = var->Type()->UnwrapRef();
    auto* structSem = unwrappedType->As<tint::sem::Struct>();
    if (structSem != nullptr && structSem->IsBlockDecorated()) {
        auto membersSem = structSem->Members();

        std::transform(membersSem.begin(), membersSem.end(), std::back_inserter(result.members),
                       [&](auto* memberSem) -> StructMember {
                           StructMember member;
                           member.name = symbols.NameFor(memberSem->Name());
                           member.id = lemon::sid(member.name);
                           member.offset = memberSem->Offset();
                           member.size = memberSem->Size();
                           member.align = memberSem->Align();
                           return member;
                       });

        result.size = structSem->Size();
        result.sizeNoPadding = structSem->SizeNoPadding();
        result.align = structSem->Align();
    }

    result.name = symbols.NameFor(var->Declaration()->symbol());
    result.id = lemon::sid(result.name);

    return result;
}

ShaderReflection::ShaderReflection(const std::string& codePath, const std::string& code)
{
    tint::Source::File sourceFile(codePath, code);
    program = tint::Program(tint::reader::wgsl::Parse(&sourceFile));
}

bool
ShaderReflection::isValid() const
{
    return program.IsValid();
}

std::string
ShaderReflection::getDiagnostic() const
{
    // TODO: Diagnostics can't be retreived on some errors resulting in invalid memory access (?)
    return program.Diagnostics().str();
}

std::vector<ResourceBindingDescriptor>
ShaderReflection::getBindingReflection() const
{
    LEMON_ASSERT(isValid());

    tint::inspector::Inspector inspector(&program);
    auto entryPoints = inspector.GetEntryPoints();

    LEMON_ASSERT(inspector.error().empty());

    std::unordered_map<BindingPoint, VariableDescriptor> boundVars;

    // First, collect all variable semantic used across all entry points. This is required
    // to set pipeline stage flags for each variable before the reflection step.
    for (auto& entryPoint : entryPoints) {
        auto* func = program.AST().Functions().Find(program.Symbols().Get(entryPoint.name));
        LEMON_ASSERT(func != nullptr);

        auto pipelineStage = getPipelineStage(entryPoint);
        auto* funcSem = program.Sem().Get(func);

        for (auto* var : funcSem->ReferencedModuleVariables()) {
            if (auto bindingPoint = var->Declaration()->binding_point()) {
                BindingPoint bp = {bindingPoint.group->value(), bindingPoint.binding->value()};

                auto search = boundVars.find(bp);
                if (search == boundVars.end()) {
                    boundVars.insert({bp, VariableDescriptor{pipelineStage, var}});
                } else {
                    search->second.stage |= pipelineStage;
                }
            }
        }
    }

    std::vector<ResourceBindingDescriptor> result;
    std::unordered_set<BindingPoint> processed;

    // Once we have full variable semantic details for each variable used, perform type reflection.
    for (auto& entryPoint : entryPoints) {
        auto bindings = inspector.GetResourceBindings(entryPoint.name);
        LEMON_ASSERT(inspector.error().empty());

        for (auto& binding : bindings) {
            BindingPoint bp = {binding.bind_group, binding.binding};
            auto varSearch = boundVars.find(bp);
            LEMON_ASSERT(varSearch != boundVars.end());

            if (processed.find(bp) != processed.end()) {
                continue;
            }

            processed.insert(bp);

            result.emplace_back(createBindingReflection(program, bp, varSearch->second, binding));
        }
    }

    std::sort(result.begin(), result.end());

    return result;
}
