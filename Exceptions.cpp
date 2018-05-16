// Exceptions.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cstdio>
#include <iostream>

static int curr_catches = 0;

struct unw_item_t {
	unw_item_t();
	virtual ~unw_item_t();
	void unreg();
	unw_item_t *prev_;
};

struct jmp_buf_splice {
	jmp_buf_splice();
	~jmp_buf_splice();
	jmp_buf buf_;
	jmp_buf_splice *prev_;
	unw_item_t *objs_;
};

static jmp_buf_splice *root_slice_ = NULL;

jmp_buf_splice::jmp_buf_splice()
{
	objs_ = NULL;
	prev_ = root_slice_;
	root_slice_ = this;
}

jmp_buf_splice::~jmp_buf_splice()
{
	root_slice_ = prev_;
}

unw_item_t::unw_item_t()
{
	if(NULL != root_slice_) {
		prev_ = root_slice_->objs_;
		root_slice_->objs_ = this;
	}
}

unw_item_t::~unw_item_t()
{
	unreg();
	std::cout << "Call destructor\n";
}

void unw_item_t::unreg()
{
	if(NULL != root_slice_ && (prev_ != NULL)) {
		root_slice_->objs_ = prev_;
		prev_ = NULL;
	}
}

static int pop_slice()
{
	jmp_buf_splice *sl = root_slice_;
	assert(NULL != sl);
	root_slice_ = sl->prev_;
	return 0;
}

int throw_slice(const char *str)
{
	if (NULL == str) {
		return -1;
	}
	jmp_buf_splice *sl = root_slice_;
	unw_item_t *obj = root_slice_->objs_;
	while (NULL != obj) {
		unw_item_t *tmp = obj;
		obj = obj->prev_;
		tmp->~unw_item_t();
	}
	pop_slice();
	longjmp(sl->buf_, int(str));
	return 0;
}


struct my_type_int : unw_item_t
{
	my_type_int(int _val) 
	{
		val = _val;
	}

	~my_type_int()
	{
		std::cout << "My value " << val << "\n";
	}

	int val;
};


#define TRY { \
    jmp_buf_splice __sl; \
	const char *__exc = (const char *)setjmp (__sl.buf_); \
	curr_catches = 0; \
	if (NULL == __exc) 

#define CATCH(exc) \
	else if (exc == __exc && ++curr_catches)

#define THROW(exc)  \
	throw_slice(exc); 

#define FINAL \
	else { \
		if(curr_catches == 0) { \
			THROW(__exc); \
		} \
	} \
	} 


#define BAD_FILE "1"

#define OUT_OF_MEMORY "2"



struct type_throw_exception : unw_item_t
{
	type_throw_exception(int _val)
	{
		val = _val;
	}
	
	~type_throw_exception()
	{
		THROW(OUT_OF_MEMORY);
	}
	
	int val;
};



void test_exc() // Test shows that we can't have two exceptions at the same time
{
	TRY	{
		type_throw_exception A(57);
		THROW(BAD_FILE);
	}
	CATCH(BAD_FILE) {
		std::cout << 1;
	}
	CATCH(OUT_OF_MEMORY) {
		std::cout << 2;
	}
	FINAL
}

void fun1() 
{
	my_type_int B(45);
	THROW(BAD_FILE);
}

void fun2() 
{
	my_type_int A(57);
	fun1();
}

void test_class_destruction() // Test shows destruction in functions
{
	TRY{
		fun2();
	}
	CATCH(BAD_FILE) {
		std::cout << "Already caught\n";
	}
	FINAL
}

void test_destruction_without_catch() // Test shows programm without true catches
{
	TRY{
		fun2();
	}
	CATCH(OUT_OF_MEMORY) {
		std::cout << "Already caught\n";
	}
	FINAL
}

void test_exception_in_second_layer() // Test shows that exception could flow throw tries
{
	TRY{
		TRY{
			fun2();
		}
		CATCH(OUT_OF_MEMORY) {
			std::cout << "Hmmm\n";
		}
		FINAL
	}
	CATCH(BAD_FILE) {
		std::cout << "Good catch!\n";
	}
	FINAL
}


int main()
{
	test_exception_in_second_layer();
	system("pause");
    return 0;
}

