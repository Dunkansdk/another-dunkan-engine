#pragma once

#include <array>
#include <cstdint>

namespace QUARK {

	template <typename DataType, std::size_t Capacity>
	struct Slotmap {

	public:
		using value_type 	= DataType;
		using index_type 	= std::uint32_t;
		using gen_type	 	= index_type;
		using key_type		= struct { index_type id; gen_type generation; };

		// constexpr = this should be able to be computed at compile time
		[[nodiscard]] constexpr std::size_t size() 			const { return m_size; }
		[[nodiscard]] constexpr std::size_t capacity() 	const { return Capacity; }

	private:
		index_type 												m_size{};
		index_type												m_freelist{};
		gen_type													m_generation{};
		std::array<key_type, Capacity> 		m_index{};
		std::array<value_type, Capacity> 	m_data{};
		std::array<index_type, Capacity> 	m_erase{};

	};

}
