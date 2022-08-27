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

		constexpr explicit Slotmap() { clear(); }

		// constexpr = this should be able to be computed at compile time
		[[nodiscard]] constexpr std::size_t size() 			const noexcept { return m_size; }
		[[nodiscard]] constexpr std::size_t capacity() 	const noexcept { return Capacity; }

		[[nodiscard]] constexpr key_type push_back(value_type&& temp_value) {
			auto slotid = allocate();
			return {};
		}

		constexpr void clear() noexcept { freelist_init(); m_generation = 0; }

	private:
		[[nodiscard]] constexpr index_type allocate() noexcept {

			//m_freelist
		}

		constexpr void freelist_init() noexcept {
			for(index_type i{}; i < m_index.size(); ++i) {
				m_index[1].id = i + 1;
			}
			m_freelist = 0;
		}

		index_type 												m_size{};
		index_type												m_freelist{};
		gen_type													m_generation{};
		std::array<key_type, Capacity> 		m_index{};
		std::array<value_type, Capacity> 	m_data{};
		std::array<index_type, Capacity> 	m_erase{};

	};

}
