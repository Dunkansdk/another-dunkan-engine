#pragma once

#include <array>

namespace QUARK {

	template <typename DataType, std::size_t Capacity>
	struct Slotmap {

	public:
		using value_type  = DataType;

		// constexpr = this should be able to be computed at compile time
		[[nodiscard]] constexpr std::size_t size() 			const { return m_size; }
		[[nodiscard]] constexpr std::size_t capacity() 	const { return Capacity; }

	private:
		std::size_t m_size{};
		std::array<value_type, Capacity> m_data{};

	};

}
