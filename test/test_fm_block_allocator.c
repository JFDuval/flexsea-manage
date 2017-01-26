#ifdef __cplusplus
extern "C" {
#endif
#include "../inc/fm_block_allocator.h"
#include "../flexsea-shared/unity/unity.h"
#include "test_fm_block_allocator.h"
#include <string.h>

void test_init(void)
{
	fm_pool_init();

	void* data = fm_pool_allocate_block();
	TEST_ASSERT_NOT_NULL(data);
	memset(data, '0', FM_BLOCK_SIZE);
	//fm_pool_free_block(data);
}

void test_allocations(void)
{
	fm_pool_init();
	void * prev;

	void* data;
	for (size_t i = 0; i< FM_NUM_BLOCKS; i++) {
		data = fm_pool_allocate_block();
		TEST_ASSERT_NOT_NULL(data);
		TEST_ASSERT_NOT_EQUAL(data, prev);
		prev = data;
	}
	void* data2 = fm_pool_allocate_block();
	TEST_ASSERT_NULL(data2);

	fm_pool_free_block(data);
	void* data3 = fm_pool_allocate_block();
	TEST_ASSERT_NOT_NULL(data3);
}


void test_msg_queue(void)
{
	MsgQueue q;
	fm_pool_init();
	int status = fm_queue_init(NULL, 1);
	TEST_ASSERT_EQUAL(-1, status);

	status = fm_queue_init(&q, 2);
	TEST_ASSERT_EQUAL(0, status);

	char* block = fm_pool_allocate_block();

	const char* test_str = "Sweet test string man!";
	strcpy(block, test_str);

	status = fm_queue_put(&q, block);

	TEST_ASSERT_EQUAL(0, status);

	char* return_block = fm_queue_get(&q);

	TEST_ASSERT_NOT_NULL(return_block);

	TEST_ASSERT_EQUAL_STRING(test_str, return_block);
	fm_pool_free_block(return_block);

	return_block = fm_queue_get(&q);

	TEST_ASSERT_NULL(return_block);


}

void test_queue_a_lot(void)
{

	MsgQueue q;
	fm_pool_init();
	fm_queue_init(&q, FM_NUM_BLOCKS);

	for (size_t j = 0; j < 100; j++)
	{
		for (size_t i = 0; i < FM_NUM_BLOCKS; i++)
		{
			char* block = fm_pool_allocate_block();
			sprintf(block, "Block # %zu\n", i);
			fm_queue_put(&q, block);
		}

		for (size_t i = 0; i < FM_NUM_BLOCKS; i++)
		{
			char* return_block = fm_queue_get(&q);
			char block[FM_BLOCK_SIZE];
			sprintf(block, "Block # %zu\n", i);
			TEST_ASSERT_EQUAL_STRING(block, return_block);
			fm_pool_free_block(return_block);
		}
	}
}

void test_block_wrapper(void)
{
	MsgQueue q;
	fm_queue_init(&q, 10);
	fm_pool_init();
	BlockWrapper* b = (BlockWrapper*) fm_pool_allocate_block();
	b->bytes_written = sprintf(b->data, "Hello");
	b->data[b->bytes_written] = '\0';
	fm_queue_put(&q, b);

	// Get it back, and verify the size

	BlockWrapper* b_returned = fm_queue_get(&q);

	TEST_ASSERT_EQUAL(5, b_returned->bytes_written);

	TEST_ASSERT_EQUAL_STRING("Hello", b->data);
}

void test_fm_block_allocator(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_init);
	RUN_TEST(test_allocations);
	RUN_TEST(test_msg_queue);
	RUN_TEST(test_queue_a_lot);
	RUN_TEST(test_block_wrapper);
	UNITY_END();

}
