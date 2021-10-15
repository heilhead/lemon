// void gameLoopPseudocode() {
//     while (true) {
//         gFrameManager.swap();
//
//         auto gameStageFuture = gScheduler.schedule([] {
//             co_await updateInput();
//             co_await updatePhysics();
//             co_await updateGameObjects();
//             co_await updateAI();
//             co_await updateAnimationData();
//             co_await prepareRenderData();
//         });
//
//         auto renderStageFuture = gScheduler.schedule([] {
//             co_await updateTransformMatrices();
//             co_await calculateCulling();
//             co_await prepareCommandLists();
//
//             dispatchRender();
//
//             co_await presentFrame();
//         });
//
//         gFrameManager.blockGameLoop(
//             gameStageFuture,
//             renderStageFuture,
//         );
//     }
// }

//    std::unique_ptr<Scheduler> sched = std::make_unique<Scheduler>();
//
//    auto result = folly::coro::blockingWait(
//        IOTask(readTextFile("C:\\git\\lemon\\engine\\core\\include\\lemon\\engine.h"))
//    );
//
//    if (result) {
//        std::cout << "result ok: " << *result << std::endl;
//    } else {
//        std::cout << "result error: " << (int)result.error() << std::endl;
//    }
