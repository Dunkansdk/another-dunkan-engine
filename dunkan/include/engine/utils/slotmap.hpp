#pragma once

#include <array>
#include <iterator>
#include <cstdint>
#include <stdexcept>
#include <cassert>

#include <iostream>

namespace ADE {

	template <typename DATA_TYPE, std::size_t CAPACITY = 10, typename INDEX_TYPE = std::uint32_t>
	struct Slotmap {

	public:
		using value_type 			= DATA_TYPE;
		using index_type 			= INDEX_TYPE;
		using gen_type	 			= index_type;
		using key_type				= struct { index_type id; gen_type generation; };
		using iterator    			= value_type*;
		using const_iterator 		= value_type const*;

		constexpr explicit Slotmap() { clear(); }

		// constexpr = this should be able to be computed at compile time
		[[nodiscard]] constexpr std::size_t size() 		const noexcept { return m_size; }
		[[nodiscard]] constexpr std::size_t capacity() 	const noexcept { return CAPACITY; }

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

        [[nodiscard]] constexpr DATA_TYPE& operator[](key_type const& key) {
            assert(is_valid(key));
            auto index = m_index[key.id];
            return m_data[index.id];
        }

		constexpr void clear() noexcept { freelist_init(); }

		constexpr bool erase(key_type key) noexcept {
			if(!is_valid(key)) { return false; }
			free(key);
			return true;
		}

		[[nodiscard]]constexpr bool is_valid(key_type key) const noexcept {
			if (key.id >= CAPACITY || m_index[key.id].generation != key.generation) { return false; }
			return true;
		}

		[[nodiscard]] constexpr iterator        begin()     noexcept { return m_data.begin(); }
		[[nodiscard]] constexpr iterator        end()       noexcept { return m_data.begin() + m_size; }
		[[nodiscard]] constexpr const_iterator  cbegin()    const noexcept { return m_data.cbegin(); }
		[[nodiscard]] constexpr const_iterator  cend()      const noexcept { return m_data.cbegin() + m_size; }

	private:
		[[nodiscard]] constexpr index_type allocate() {
			if (m_size >= CAPACITY) throw std::runtime_error("No space left in the slotmap");
			assert(m_freelist < CAPACITY);

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

		index_type                          m_size{};
		index_type                          m_freelist{};
		gen_type                            m_generation{};
		std::array<key_type, CAPACITY>      m_index{};
		std::array<value_type, CAPACITY>    m_data{};
		std::array<index_type, CAPACITY>    m_erase{};

	};

}
