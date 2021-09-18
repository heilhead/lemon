#include "mesh.h"

#include "ozz/base/io/stream.h"
#include "ozz/base/io/archive.h"
#include "ozz/animation/runtime/skeleton.h"
#include "ozz/animation/runtime/animation.h"

#include <lemon/utils/utils.h>

void
loadMesh() {
    const char* filename = R"(C:\git\lemon\resources\ozz-sample\SK_Sample.ozz)";
    ozz::io::File file(filename, "rb");

    if (!file.opened()) {
        lemon::utils::printErr("failed to open file: ", filename);
        return;
    }

    ozz::io::IArchive archive(&file);

    if (!archive.TestTag<ozz::animation::Skeleton>()) {
        lemon::utils::printErr("archive doesn't contain the expected object type");
        return;
    }

    ozz::animation::Skeleton skeleton;
    archive >> skeleton;
}

void
loadSkeleton() {
    const char* filename = R"(C:\git\lemon\resources\ozz-sample\SKEL_Sample.ozz)";
    ozz::io::File file(filename, "rb");

    if (!file.opened()) {
        lemon::utils::printErr("failed to open file: ", filename);
        return;
    }

    ozz::io::IArchive archive(&file);

    if (!archive.TestTag<ozz::animation::Skeleton>()) {
        lemon::utils::printErr("archive doesn't contain the expected object type");
        return;
    }

    ozz::animation::Skeleton skeleton;
    archive >> skeleton;
}

void
loadAnimation() {
    const char* filename = R"(C:\git\lemon\resources\ozz-sample\A_Sample.ozz)";
    ozz::io::File file(filename, "rb");

    if (!file.opened()) {
        lemon::utils::printErr("failed to open file: ", filename);
        return;
    }

    ozz::io::IArchive archive(&file);

    if (!archive.TestTag<ozz::animation::Animation>()) {
        lemon::utils::printErr("archive doesn't contain the expected object type");
        return;
    }

    ozz::animation::Animation animation;
    archive >> animation;
}

void
testMeshLoading() {
    lemon::utils::print("ozz skeleton before load");

    {
        const char* filename =
            R"(D:\Downloads\ozz-0.13.0-windows64-vs2019\bin\samples\attach\media\skeleton.ozz)";
        ozz::io::File file(filename, "rb");

        if (!file.opened()) {
            lemon::utils::printErr("failed to open file: ", filename);
            return;
        }

        ozz::io::IArchive archive(&file);

        if (!archive.TestTag<ozz::animation::Skeleton>()) {
            lemon::utils::printErr("archive doesn't contain the expected object type");
            return;
        }

        ozz::animation::Skeleton skeleton;
        archive >> skeleton;
    }

    lemon::utils::print("ozz skeleton after load");
}
