#pragma once

#include <windows.h>
#include <cstdint>
#include <functional>
#include <GLFW/glfw3.h>

namespace lemon {
	enum class WindowKind {
		Window,
		Fullscreen,
		WindowedFullscreen
	};

    enum class LoopControl {
        Continue,
        Abort
    };

	struct WindowDescriptor {
		WindowKind kind{ WindowKind::Window };
		uint32_t width{ 640 };
		uint32_t height{ 480 };
	};

	class Window {
	public:
		explicit Window(WindowDescriptor& desc);
        ~Window();

	protected:
		HWND hWnd;
		GLFWwindow* window;

	public:
		[[nodiscard]] HWND getContextHandle() const {
			return hWnd;
		}

		void loop(const std::function<LoopControl(float)>& callback) const;
	};
}
