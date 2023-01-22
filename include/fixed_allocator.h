#pragma once
#include "chunk.h"
#include <vector>


namespace td {
	namespace allocator {
		class fixed_allocator
		{


			void* allocate();
			void deallocate(void* p);

		private:
			typedef std::vector<chunk> chunk_vec_t;
			std::size_t blocksize;
			std::uint8_t numblocks;
			chunk_vec_t chunks;

			chunk* alloc_chunk;
			chunk* dealloc_chunk;

		};
	}
}

