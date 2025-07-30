#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <imgui.h>

namespace Seek {

	namespace Menu {

		class EntryDebugger {
		public:
			std::string name;

			ImVec2 size{ 800, 100 };

			virtual void InitEntry() {
				std::cout << "Fath" << std::endl;
			};
		};
	}
}