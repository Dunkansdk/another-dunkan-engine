#pragma once

#include <array>
#include <cstdint>
#include <stdexcept>
#include <cassert>

namespace QUARK {

	template <typename DataType, typename IndexType = std::uint32_t, std::size_t Capacity = 10>
	struct Slotmap {

	public:
		using value_type 			= DataType;
		using index_type 			= IndexType;
		using gen_type	 			= index_type;
		using key_type				= struct { index_type id; gen_type generation; };
		using iterator    		= value_type*;
		using const_iterator 	= value_type const*;

		constexpr explicit Slotmap() { clear(); }

		// constexpr = this should be able to be computed at compile time
		[[nodiscard]] constexpr std::size_t size() 			const noexcept { return m_size; }
		[[nodiscard]] constexpr std::size_t capacity() 	const noexcept { return Capacity; }

		/*
		 * value_type&& -> temporal variable (rvalue)
		 *
		 */
		[[nodiscard]] constexpr key_type push_back(value_type&& temp_value) {
			auto reserved_id = allocate();
			auto& slot = m_index[reserved_id];

			// move data
			m_data[slot.id] = std::move(temp_value);
			m_erase[slot.id] = reserved_id;

			// key for the user
			auto key { slot };
			key.id = reserved_id;

			return key;
		}

		/*
		 * value_type const& -> lvalue, no temporal value
		 *
		 */
		[[nodiscard]] constexpr key_type push_back(value_type const& ref_value) {
			return push_back(value_type{ref_value});
		}

		constexpr void clear() noexcept { freelist_init(); }

		constexpr bool erase(key_type key) noexcept {
			if(!is_valid(key)) { return false; }
			free(key);
			return true;
		}

		[[nodiscard]]constexpr bool is_valid(key_type key) const noexcept {
			if (key.id >= Capacity || m_index[key.id].generation != key.generation) { return false; }
			return true;
		}

		[[nodiscard]] constexpr iterator  begin() noexcept { return m_data.begin(); }
		[[nodiscard]] constexpr iterator    end() noexcept { return m_data.begin() + m_size; }
		[[nodiscard]] constexpr iterator cbegin() const noexcept { return m_data.cbegin(); }
		[[nodiscard]] constexpr iterator   cend() const noexcept { return m_data.cbegin() + m_size; }

	private:
		[[nodiscard]] constexpr index_type allocate() {
			if (m_size >= Capacity) throw std::runtime_error("No space left in the slotmap");
			assert(m_freelist < Capacity);

			// Reserve
			auto slotid = m_freelist;
			m_freelist = m_index[slotid].id; // Freelist -> first free

			// Init slot
			auto& slot = m_index[slotid];
			slot.id = m_size;
			slot.generation = m_generation;

			// Update space and generation
			++m_size;
			++m_generation;

			return slotid;
		}

		constexpr void free(key_type key) noexcept {
			assert(is_valid(key));

			auto& slot = m_index[key.id];
			auto data_id = slot.id; // save id of data slot to check if it is last or not

			// update freelist
			slot.id = m_freelist;
			slot.generation = m_generation;
			m_freelist = key.id;

			// copy data to free slot
			if (data_id != m_size - 1) {
      	// data slot is not last, copy last here
				m_data[data_id] = m_data[m_size - 1];
				m_erase[data_id] = m_erase[m_size - 1];
				m_index[m_erase[data_id]].id = data_id;
			}

			// update size
			--m_size;
			++m_generation;
		}

		constexpr void freelist_init() noexcept {
			for(index_type i{}; i < m_index.size(); ++i) {
				m_index[i].id = i + 1;
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
