#pragma once

#include <array>
#include <vector>
#include <cstdio>
#include <cstdint>

namespace QUARK {

	struct MemoryViewer {

		public:
			constexpr static std::size_t default_width {16};

			static std::size_t calculate_lines(std::size_t const size, std::size_t const width = default_width)
			{
    		auto const lines {size / width};
    		if ((size & 0xF) != 0)
      		return lines + 1;
    		return lines;
			}

			static void show_memory_line(std::uint8_t const* memory, std::size_t width = default_width)
			{
				std::printf("%16p ||", (void*)memory);
				for(std::size_t i{}; i < width; ++i) { std::printf(" %02X", memory[i]);	}
				std::printf(" || ", (void*)memory);
				for(std::size_t i{}; i < width; ++i) {
					auto character = (memory[i] > 31 && memory[i] < 128) ? memory[i] : '.'; // Character filter fix
					std::printf("%c", character);
				}
				std::printf(" || \n");
			}

			static void show_memory(std::uint8_t const* memory, std::size_t const size, std::size_t width = default_width)
			{
				auto const lines { calculate_lines(size, width) };
				std::printf("     ADDRESS     || 01 00 00 00 02 00 00 00 04 00 00 00 08 00 00 00\n");
				std::printf("-----------------||------------------------------------------------\n");
				for(std::size_t i{}; i < lines; ++i) {
					show_memory_line(memory, width);
					memory += width;
				}
				std::printf("-------------------------------------------------------------------\n");
			}

			static void show_memory_object(auto const& object) {
  			auto* pointer = reinterpret_cast<std::uint8_t const*>(&object);
				std::printf("  Object Sizeof  || %ld\n", sizeof(object));
  			show_memory(pointer, sizeof(object));
			}

			static void show_memory_ptr(auto const* memory, std::size_t const size) {
				auto* pointer = reinterpret_cast<std::uint8_t const*>(memory);
				std::printf("  Object Sizeof  || %ld\n", sizeof(size));
				show_memory(pointer, size);
			}

		private:
	};

}
