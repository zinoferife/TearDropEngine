#pragma once
#include <cstdint>
#include <cstddef>
#include <cassert>

//represents a chunk in the allocator
//based on andrei alex loki lib
namespace td {
	namespace allocator {
		struct chunk
		{
			std::uint8_t* data;
			std::uint8_t first_aval_block;
			std::uint8_t block_available;

			void init(std::size_t block_size, std::uint8_t blocks);
			void release();
			void deallocate(void* p, std::size_t block_size);
			void* allocate(std::size_t block_size);
		};
	}
}

