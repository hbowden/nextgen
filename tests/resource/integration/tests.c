/*
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "unity.h"
#include "memory/memory.h"
#include "crypto/crypto.h"
#include "crypto/random.h"
#include "crypto/hash.h"
#include "resource/resource.c"
#include "io/io.h"

#include <pthread.h>

static uint32_t iterations = 1000;

static void test_resource_generator(void)
{
	  uint32_t i;
	  int32_t fd = 0;
		struct resource_generator *rsrc_gen = NULL;

		rsrc_gen = get_resource_generator();
		TEST_ASSERT_NOT_NULL(rsrc_gen);

		fd = rsrc_gen->get_desc();
    TEST_ASSERT(fd > 0);

		for(i = 0; i < iterations; i++)
    {
				fd = rsrc_gen->get_desc();
		    TEST_ASSERT(fd > 0);

        rsrc_gen->free_desc(&fd);
    }

		// int32_t sock = 0;
		//
		// for(i = 0; i < iterations; i++)
    // {
		// 	  printf("u: %d\n", i);
		// 		sock = rsrc_gen->get_socket();
		// 		printf("sock: %d\n", sock);
		//     TEST_ASSERT(sock > 0);
		//
    //     rsrc_gen->free_socket(&sock);
    // }

    char *file = NULL;

		for(i = 0; i < iterations; i++)
		{
			  file = rsrc_gen->get_filepath();
				TEST_ASSERT_NOT_NULL(file);
				TEST_ASSERT(close(open(file, O_RDWR)) == 0);
				rsrc_gen->free_filepath(&file);
		}

		char *dir = NULL;

		for(i = 0; i < iterations; i++)
		{
			  dir = rsrc_gen->get_dirpath();
				TEST_ASSERT_NOT_NULL(dir);
				rsrc_gen->free_dirpath(&dir);
		}

	  return;
}

static void test_inject_resource_deps(void)
{
	  struct dependency_context *ctx = NULL;
	  struct output_writter *output_writter = get_console_writter();
	  TEST_ASSERT_NOT_NULL(output_writter);

	  struct memory_allocator *mem_allocator = get_default_allocator();
	  TEST_ASSERT_NOT_NULL(mem_allocator);

		ctx = create_dependency_ctx(create_dependency(output_writter, OUTPUT),
																create_dependency(mem_allocator, ALLOCATOR),
															  NULL);
		TEST_ASSERT_NOT_NULL(ctx->array);

    /* We need to inject the utils and crypto modules deps before using the
		  resource module. */
    inject_utils_deps(ctx);
		inject_crypto_deps(ctx);

		struct random_generator *random = get_default_random_generator();
		TEST_ASSERT_NOT_NULL(random);

		struct hasher *hasher = NULL;
		hasher = get_hasher();
		TEST_ASSERT_NOT_NULL(hasher);

		ctx = create_dependency_ctx(create_dependency(output_writter, OUTPUT),
																create_dependency(mem_allocator, ALLOCATOR),
																create_dependency(random, RANDOM_GEN),
																create_dependency(hasher, HASHER),
															  NULL);
		TEST_ASSERT_NOT_NULL(ctx->array);

    inject_utils_deps(ctx);
		inject_resource_deps(ctx);
}

int main(void)
{

	  /* Cleanup tmp before we start so filepath and dirpath tests don't fail. */
		delete_dir_contents("/tmp");

	  test_inject_resource_deps();
    test_resource_generator();

	  _exit(0);
}
