    // for (int i = 0; i < 3; i++) {
    //     path p1 = dirs[i];

    //    auto bun = createBundle();

    //    for (int j = 0; j < 3; j++) {
    //        path p2 = p1;
    //        p2 /= dirs[j];

    //        auto mat = createMaterial();
    //        mat->shaderConfig.insert({"ENABLE_SKINNING", 0});

    //        for (int k = 0; k < 3; k++) {
    //            path texPath = p2;
    //            texPath /= textures[k];

    //            auto tex = createTexture();
    //            tex->decoder = TextureResource::Decoder::PNG;
    //            tex->format = wgpu::TextureFormat::RGBA8UnormSrgb;

    //            saveMetadata<TextureResource>(std::move(tex), texPath);

    //            mat->common.references.push_back(RawResourceReference{
    //                .location = texPath.string(),
    //                .type = TextureResource::getType(),
    //            });
    //            mat->textures.insert({textureTypes[k], texPath.string()});
    //        }

    //        path matPath = p1;
    //        matPath /= materials[j];

    //        bun->common.references.push_back(RawResourceReference{
    //            .location = matPath.string(),
    //            .type = MaterialResource::getType(),
    //        });

    //        saveMetadata<MaterialResource>(std::move(mat), matPath);
    //    }

    //    path bunPath = bundles[i];

    //    saveMetadata<BundleResource>(std::move(bun), bunPath);
    //}

    //{
    //    auto bun = createBundle();
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_AB",
    //        .type = BundleResource::getType(),
    //    });
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_BC",
    //        .type = BundleResource::getType(),
    //    });
    //    path bunPath = "RB_ABC";
    //    saveMetadata<BundleResource>(std::move(bun), bunPath);
    //}

    //{
    //    auto bun = createBundle();
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_A",
    //        .type = BundleResource::getType(),
    //    });
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_B",
    //        .type = BundleResource::getType(),
    //    });
    //    path bunPath = "RB_AB";
    //    saveMetadata<BundleResource>(std::move(bun), bunPath);
    //}

    //{
    //    auto bun = createBundle();
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_B",
    //        .type = BundleResource::getType(),
    //    });
    //    bun->common.references.push_back(RawResourceReference{
    //        .location = "RB_C",
    //        .type = BundleResource::getType(),
    //    });
    //    path bunPath = "RB_BC";
    //    saveMetadata<BundleResource>(std::move(bun), bunPath);
    //}
