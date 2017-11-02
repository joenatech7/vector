#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

typedef struct
{
	char *str;
	int i;
}test_data_t;


vector_t *
vector_create(vector_size_t capacity, vector_size_t obj_size, data_ops_t ops)
{
	
	vector_t *v = malloc(sizeof(vector_t));
	if (!v)
	{
		return NULL;
	} // end if
	
	vector_size_t _capacity = MAX(VECTOR_MIN_CAPACITY, capacity);
	v->data = malloc(_capacity * obj_size);
	if (!v->data)
	{
		sfree(v);
		return NULL;
	} // end if
	
	v->capacity = _capacity;
	v->size = 0;
	v->obj_size = obj_size;
	v->data_ops = ops;
	
	return v;
} // end vector_create()

int
vector_destroy(vector_t *v)
{
;
} // end vector_destroy()

int
vector_push_back(vector_t *v, void *obj)
{
	if (!v || !v->data || !obj)
	{
		return -1;
	} // end if
	
	void *_obj = v->data_ops.ctor(obj);
	
	//printf("in vpb, string = %s\n", ((test_data_t *)_e)->str);
	
	void *addr = vector_assign(v, v->size, _obj);
	if (!addr)
	{
		return -1;
	} // end if
	
	sfree(_obj);
	
	(v->size)++;
	return 0;
} // end vector_push_back()

void *
vector_addr_at_index(vector_t *v, vector_size_t i)
{
	if (!v || !v->data || i > v->size)
	{
		return NULL;
	} // end if
	
	return (v->data + (i * v->obj_size));
} // end vector_addr_at_index()

int 
vector_assign(vector_t *v, vector_size_t i, void *obj)
{
	if (!v || i > v->size || !obj)
	{
		return -1;
	} // end if
	
	void *addr = vector_addr_at_index(v, i);
	if (!addr)
	{
		return -1;
	} // end if
	
	memcpy(addr, obj, v->obj_size); // potential error
	return 0;
} // end vector_assign()

vector_t *
vector_copy(vector_t *src) // clean up before exit (avoid memory leaks)
{
	if (!src || !src->data)
	{
		return NULL;
	} // end if
	
	vector_t *dest = vector_create(src->capacity, src->obj_size, src->data_ops);
	if (!dest)
	{
		return NULL;
	} // end if
	
	vector_iterator_t *start = vector_begin(src);
	vector_iterator_t *end = vector_end(src);
	if (!start || !end)
	{
		return NULL;
	} // end if
	
	while (start != end)
	{
		void *addr = iterator_next(start);
		if (!addr)
		{
			return NULL;
		} // end if
		
		int res = vector_push_back(addr);
		if (res == -1)
		{
			return NULL;
		} // end 
	} // end while
} // end vector_copy()

vector_t *
vector_copy_assign(vector_t *dest, vector_t *src)
{
	if (!dest || !dest->data || !src || !src->data)
	{
		return dest;
	} // end if
	
	int res = vector_destroy(dest);
	if (res == -1)
	{
	
	} // end if
	
	dest = vector_copy(src);
	return dest;
} // end vector_copy_assign()


int
vector_destroy(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	vector_iterator_t *start = vector_begin(v);
	vector_iterator_t *end = vector_end(v);
	if (!start || !end)
	{
		return -1;
	} // end if
	
	while (start != end)
	{
		void *addr = iterator_next(start);
		if (!addr)
		{
			return -1;
		} // end if
		
		int res = v->data_ops.dtor(addr);
		if (res == -1)
		{
			return -1;
		} // end 
	} // end while
	
	sfree(v->data);
	sfreev(v);
	
	return 0;
} // end vector_destroy()

int
vector_insert_at(vector_t *v, vector_size_t i, void *obj)
{
	if (!v || !v->data || i > v->size || !obj)
	{
		return -1;
	} // end if
	
	int res = vector_move_right(v, i);
	if (res == -1)
	{
		return -1;
	} // end if
	
	void *_obj = v->data_ops.ctor(obj);
	
	void *addr = vector_assign(v, v->size, _obj);
	if (!addr)
	{
		sfree(_obj);
		return -1;
	} // end if
	
	sfree(_obj);
	
	(v->size)++;
} // end vector_insert_at()

int
vector_move_right(vector_t *v, vector_size_t i)
{
	vector_size_t mv_bytes = (v->size - i) * v->obj_size;
	if (mv_bytes == 0)
	{
		return 0;
	} // end if
	
	void *mv_addr = vector_addr_at_index(v, i);
	if (!mv_addr)
	{
		return -1;
	} // end if
	
	memmove(mv_addr + v->obj_size, mv_addr, mv_bytes);
	return 0;
} // end vector_move_right()

int vector_delete_at(vector_ct *v, vector_size i)
{
	if (!v || !v->data || i >= v->size)
	{
		return -1;
	} // end if
	
	void *del_addr = vector_addr_at_index(v, i);
	if (!del_addr)
	{
		return -1;
	} // end if

	int res = v->data_ops.dtor(del_addr);
	if (res == -1)
	{
		return -1;
	} // end 	
	
	return vector_move_left(v, i);
} // end vector_delete_at()

int vector_pop_front(vector_t *v)
{
	return vector_delete_at(v, 0);
} // end vector_pop_front()

int vector_push_front(vector_t *v)
{
	return vector_insert_at(v, 0);
} // end vector_push_front()

int _vector_pop_back(vector_t *v)
{
	if (!v || !v->data)
	{
	
	} // end if
	
	return vector_insert_at(v, v->size);
} // end vector_pop_back()

int
vector_move_left(vector_t *v, vector_size_t i)
{
	vector_size_t mv_bytes = (v->size - i - 1) * v->obj_size;
	if (mv_bytes == 0)
	{
		return 0;
	} // end if
	
	void *mv_addr = vector_addr_at_index(v, i);
	if (!mv_addr)
	{
		return -1;
	} // end if
	
	memmove(mv_addr, mv_addr + v->obj_size, mv_bytes);
	return 0;
} // end vector_move_left()

//------------------------------lookup

void *
vector_get(vector_t *v, vector_size_t i)
{
	if (!v || !v->data || i >= v->size)
	{
		return NULL;
	} // end if
	
	return vector_addr_at_index(v, i);
} // end vector_get()

void *
vector_front(vector_t *v)
{
	if (!v)
	{
		return NULL;
	} // end if
	
	return vector_get(v, 0);
} // end vector_front()

void *
vector_back(vector_t *v)
{
	if (!v)
	{
		return NULL;
	} // end if
	
	return vector_get(v, v->size - 1);
} // end vector_back()


//-----------------------------iterator functions
//---------------------------------------------------------------------------------------

vector_iterator_t *
vector_get_iterator(vector_t *v, vector_size_t i)
{
	if (!v || !v->data || i > v->size)
	{
		return NULL;
	} // end if
	
	return vector_iterator *it = iterator_create(vector_addr_at_index(v, i), v->obj_size);
} // end vector_get_iterator()

vector_iterator_t *
vector_begin(vector_t *v)
{
	if (!v || !v->data)
	{
		return NULL;
	} // end if
	
	return vector_get_iterator(v, 0);
} // end vector_begin()


vector_iterator_t *
vector_end(vector_t *v)
{
	if (!v || !v->data)
	{
		return NULL;
	} // end if
	
	return vector_get_iterator(v, v->size);
} // end vector_end()

vector_size_t
vector_iterator_index(vector_t *v, vector_iterator_t *it)
{
	if (!v || !v->data || !it || !it->addr || v->obj_size != it->obj_size)
	{
		return -1; // potential problem
	} // end if
	
	return (it->addr - v->data) / (v->obj_size);
} // end vector_iterator_index()

//-----------------------------------sizing
///////////////////////////////////////////////////////////////
int 
vector_resize_up(vector_t *v)
{
	 if (!v || !v->data)
	 {
	 	return -1;
	 } // end if
	 
	 vector_size_t new_capacity = v->capacity * VECTOR_GROWTH_FACTOR;
	 
	 return vector_resize(v, new_capacity);
} // end vector_resize_up()

int 
vector_shrink_to_fit(vector_t *v)
{
	if (!v || !v->data)
	{
		return -1;
	} // end if
	
	vector_size_t new_capacity = MAX(v->size, VECTOR_MIN_CAPACITY);
	
	return vector_resize(v, new_capacity);
} // end if

int vector_resize(vector_t *v , vector_size_t new_capacity)
{
	if (!v || !v->data || new_capacity < VECTOR_MIN_CAPACITY)
	{
		return -1;
	} // end if
	
	void *old_data = v->data;
	
	v->data = malloc(new->capacity * v->obj_size);
	if (!v->data)
	{
		return -1;
	} // end if
	
	memcpy(v->data, old_data, v->size * v->obj_size);
	
	v->capacity = new_capacity;
	
	free(old_data);
	
	return 0;
} // end vector_resize()
/////------------------------------errr
//////////////////-------------------------------------------------------------
void safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
} // end safe_free()
