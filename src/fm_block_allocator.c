/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'flexsea-manage' Mid-level computing, and networking
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] fm_block_allocator: Fixed sized block allocator
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2017-01-20 | igutekunst | Initial implementation
	*
*****************************************************************************/
#include "../inc/fm_block_allocator.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

struct Block {
	struct Block * next;
	struct Block * prev;
	size_t index;
	char data[FM_BLOCK_SIZE];
};


typedef struct Block Block;

Block memory_pool[FM_NUM_BLOCKS];

#define offsetof(st, m) ((size_t)(&((st *)0)->m))

static Block* get_block(void * raw_block) {
	size_t offset = offsetof(Block, data);
	Block* b = raw_block - offset;
	return b;
}

static Block* first;

void fm_pool_init() 
{
	// all prev pointers are set to NULL because we
	// dont need them for allocations
	size_t i;
	for (i = 0; i < FM_NUM_BLOCKS - 1; i++) {
		memory_pool[i].index = i;
		memory_pool[i].prev = NULL;
		memory_pool[i].next = &memory_pool[i+1];
	}
	i++;
	memory_pool[i].next = NULL;
	memory_pool[i].prev = NULL;

	first = &memory_pool[0];
}

void* fm_pool_allocate_block(void)
{
	Block* new_block = first;

	if (first != NULL)
		first = first->next;

	if (new_block)
	{
		new_block->next = NULL;
		new_block->prev = NULL;
		return new_block->data;
	}
	return NULL;
}


int fm_pool_free_block(void* raw_data) {
	if (raw_data != NULL)
	{
		Block* block =  get_block(raw_data);
		block->next = first;
		first = block;
		block->prev = NULL;
		return 0;
	}
	return -1;
}


int fm_queue_init(MsgQueue* q, size_t max_size)
{
	if (q == NULL)
		return -1;

	if (max_size <= 0)
		return -1;

	q->max_size = max_size;
	q->size = 0;
	q->head = NULL;
	q->tail = NULL;
		return 0;
}


int fm_queue_put(MsgQueue* q, void* item) {
	if (q == NULL || item == NULL)
		return -1;

	if (q->size >= q->max_size)
		return -1;

	Block** head = (Block**)&q->head;
	Block** tail = (Block**)&q->tail;

	Block* block =  get_block(item);
	block->next = *head;
	if (*head != NULL) {
		(*head)->prev = block;
	}
	*head = block;
	q->size++;

	if (q->size == 1)
		*tail = block;
	return 0;
}

void* fm_queue_get(MsgQueue* q ) {
	if (q == NULL)
		return NULL;

	if (q->size == 0)
		return NULL;

	q->size--;

	Block** head = (Block**)&q->head;
	Block** tail = (Block**)&q->tail;

	Block* item = *tail;

	// *tail shouldn't be NULL
	// since the size was > 0
	*tail = (*tail)->prev;

	if (q->size == 0)
		*head = NULL;

	item->prev = NULL;
	item->next = NULL;

	return item->data;
}

