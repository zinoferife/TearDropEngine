#include "chunk.h"

void td::allocator::chunk::init(std::size_t block_size, std::uint8_t blocks)
{
	data = new std::uint8_t[block_size * blocks];
	first_aval_block = 0;
	block_available = blocks;
	std::uint8_t i = 0;
	std::uint8_t* p = data;
	//give the first byte of each block the block index
	for (; i != blocks; p += block_size)
	{
		*p = ++i;
	}

}

void td::allocator::chunk::release()
{
	delete[] data;
	first_aval_block = 0;
	block_available = 0;
	data = NULL;
}


//pass in the wrong pointer to this function, leads to all hell breaking loose
void td::allocator::chunk::deallocate(void* p, std::size_t block_size)
{
	assert(p >= data && "invalid pointer in chunk");
	std::uint8_t* to_release = static_cast<std::uint8_t*>(p);
	//alignment check
	assert((to_release - data) % block_size == 0 && "Not aligned to block boundires in chunk");
	*to_release = first_aval_block;
	first_aval_block = static_cast<std::uint8_t>((to_release - data) / block_size);
	//truncation check
	assert(first_aval_block == (to_release - data) / block_size && "Truncated block size in chunck");
	++block_available;

}

void* td::allocator::chunk::allocate(std::size_t block_size)
{
	if (!block_available) return 0;
	std::uint8_t* result = data + (first_aval_block * block_size);
	first_aval_block = *result;
	--block_available;
	return result;
}
