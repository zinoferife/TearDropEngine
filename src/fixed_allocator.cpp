#include "fixed_allocator.h"

void* td::allocator::fixed_allocator::allocate()
{
	if (alloc_chunk == 0 || alloc_chunk->block_available == 0) {
		chunk_vec_t::iterator iter = chunks.begin();
		for (;; ++iter) {
			if (iter == chunks.end()) {
				//all chunks are filled up, append a new chunk
				chunks.reserve(chunks.size() + 1);
				chunk nchunk;
				nchunk.init(blocksize, numblocks);
				chunks.push_back(nchunk);
				alloc_chunk = &chunks.back();
				dealloc_chunk = &chunks.back();
				break;
			}if (iter->block_available > 0) {
				//found
				alloc_chunk = &(*iter);
				break;
			}
		}
	}
	assert(alloc_chunk != 0 && "Alloc chunk is null, invalid allocation");
	assert(alloc_chunk->block_available > 0 && "No block available in Alloc chunk");
	return alloc_chunk->allocate(blocksize);
}

void td::allocator::fixed_allocator::deallocate(void* p)
{
	

}
