/*
 * Copyright (c) 2016, Harrison Bowden, Minneapolis, MN
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright notice
 * and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY å
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "unity.h"
#include "utils/utils.h"
#include "crypto/crypto.h"
#include "memory/memory.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>

static uint32_t number_of_extensions = 11;

static uint32_t size_array[] = { 3, 3, 3, 3, 3, 3, 3, 1, 1, 3, 2};

/* Array of extensions. */
static char *ext[] = { "txt", "mp3", "mp4", "m4v", "jpg",
                       "pdf", "dmg", "c", "h", "png", "go" };

/* The number of ascii to binary comparison test. */
static uint32_t binary_test = 6;

/* Array of strings to covert to binary. */
static char *string_array[] = { "a", "b", "c", "d", "e", "f", NULL };

/* Array of binary strings to compare against. */
static char *binary_array[] = { "01100001", "01100010", "01100011", "01100100",
                                "01100101", "01100110", NULL };

static void test_run_syscall(void)
{
    int32_t rtrn = 0;
    rtrn = run_syscall(SYS_write, 1, "TEST", 4);
    TEST_ASSERT(rtrn == 4);
}

static void test_delete_directory(void)
{
    int32_t rtrn = 0;
    char *dir auto_free = NULL;
    char *name auto_free = NULL;

    rtrn = generate_name(&name, ".txt", DIR_NAME);
    TEST_ASSERT(rtrn == 0);

    rtrn = asprintf(&dir, "/tmp/%s\n", name);
    TEST_ASSERT(rtrn > -1);

    /* Let's make a directory in /tmp for testing delete_directory(). */
    rtrn = mkdir(dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    TEST_ASSERT(rtrn == 0);

    uint32_t total_files = 0;

    /* Pick a random amount of files to create. */
    rtrn = rand_range(25, &total_files);
    TEST_ASSERT(rtrn == 0);

    uint32_t i;

    /* Loop and create a bunch of random text files. */
    for(i = 0; i < total_files; i++)
    {
        uint64_t size = 0;
        char *path auto_free = NULL;

        /* Create random file. */
        rtrn = create_random_file(dir, ".txt", &path, &size);
        TEST_ASSERT(rtrn == 0);
    }

    /* Now that were done setting up a test directory let's start
    testing delete_directory(). Pass a NULL path and make sure delete_directory
    returns an error. */
    char *dir_path = NULL;

    rtrn = delete_directory(dir_path);
    TEST_ASSERT(rtrn < 0);

    char *file_path auto_free = NULL;
    uint64_t size = 0;

    /* Create a file path. */
    rtrn = create_random_file("/tmp", ".txt", &file_path, &size);
    TEST_ASSERT(rtrn == 0);

    /* If we pass delete_directory() a file path then it should fail. */
    rtrn = delete_directory(file_path);
    TEST_ASSERT(rtrn < 0);

    /* Create a directory path that does not exist. */
    char *random_path auto_free = NULL;

    rtrn = generate_name(&random_path, NULL, DIR_NAME);
    TEST_ASSERT(rtrn == 0);

    rtrn = delete_directory(random_path);
    TEST_ASSERT(rtrn < 0);

    /* Now let's try deleting the directory we made. */
    rtrn = delete_directory(dir);
    TEST_ASSERT(rtrn == 0);

    struct stat buf;

    /* Stat should fail on the directory. */
    TEST_ASSERT(stat(dir, &buf) < 0);

    return;
}

static void test_check_root(void)
{
	int32_t rtrn = 0;
    uid_t uid = getuid();

    rtrn = check_root();

    if(uid != 0)
    {
        TEST_ASSERT(rtrn != 0);
    }
    else
    {
        TEST_ASSERT(rtrn == 0);
    }

	return;
}

static void test_get_file_size(void)
{
    uint32_t i;
    int32_t fd = 0;
    int32_t rtrn = 0;
    char *path = NULL;
    uint64_t size = 0;
    uint64_t file_size = 0;

    for(i = 0; i < 100; i++)
    {
        rtrn = create_random_file("/tmp", ".txt", &path, &size);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT_NOT_NULL(path);

        fd = open(path, O_RDONLY);
        TEST_ASSERT(fd > -1);

        rtrn = get_file_size(fd, &file_size);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT(size == file_size);

        /* Clean up. */
        unlink(path);
    }

    return;
}

static void test_ascii_to_binary(void)
{
    uint32_t i;
    int32_t rtrn = 0;

    char *string = "dn39r93ho*(#98893rnf@N(EHM(EI";
    char *binary_string = NULL;
    uint64_t size = 0;

    /* When we pass a zero size, ascii_to_binary should fail. */
    rtrn = ascii_to_binary(string, &binary_string, 0, &size);
    TEST_ASSERT(rtrn < 0);
    TEST_ASSERT(binary_string == NULL);
    TEST_ASSERT(size == 0);

    /* Should work now that we have a non zero length argument. */
    rtrn = ascii_to_binary(string, &binary_string, strlen(string), &size);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(binary_string != NULL);
    TEST_ASSERT(size > 0);

    /* Clean up. */
    mem_free((void **)&binary_string);

    /* Loop and compare conversions
    to known good values to validate ascii_to_binary(); */
    for(i = 0; i < binary_test; i++)
    {
        /* Convert the string in  */
        rtrn = ascii_to_binary(string_array[i], &binary_string, strlen(string_array[i]), &size);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT(binary_string != NULL);
        TEST_ASSERT(size > 0);
        TEST_ASSERT(strncmp(binary_array[i], binary_string, 8) == 0);
        mem_free((void **)&binary_string);
    }

    return;
}

static void test_get_core_count(void)
{
    int32_t rtrn = 0;
    uint32_t count = 0;

    rtrn = get_core_count(&count);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(count > 0);

    /* A second call should have the same result. */
    uint32_t second_count = 0;

    rtrn = get_core_count(&second_count);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(second_count > 0);
    TEST_ASSERT(count == second_count);

    return;
}

static void test_get_extension(void)
{
    uint32_t i;
    int32_t rtrn = 0;
    char *path = NULL;
    uint64_t size = 0;
    char *extension = NULL;

    for(i = 0; i < number_of_extensions; i++)
    {
        /* Create a random file. */
        rtrn = create_random_file("/tmp", ext[i], &path, &size);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT(path != NULL);

        /* Check the extension of the file against the extension it should be. */
        rtrn = get_extension(path, &extension);
        TEST_ASSERT(rtrn == 0);
        TEST_ASSERT(extension != NULL);
        TEST_ASSERT(strncmp(extension, ext[i], size_array[i]) == 0);

        mem_free((void **)&path);
    }

    return;
}

static void test_create_random_directory(void)
{
    struct stat sb;
    int32_t rtrn = 0;
    char *path = NULL;
    rtrn = create_random_directory("/tmp", &path);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT_NOT_NULL(path);

    rtrn = stat(path, &sb);
    TEST_ASSERT(rtrn == 0);
    TEST_ASSERT(sb.st_mode & S_IFDIR);

    /* A second call to create_random_directory */

    return;
}

int main(void)
{
    /* We have to setup the crypto module before using
     some utils module api/functions. */
    setup_crypto_module(CRYPTO);

    /* Delete the contents of temp before testing.
    If we don't clear the contents some test may fail. */
    delete_dir_contents("/tmp");

	  test_check_root();
    test_get_file_size();
    test_get_core_count();
    test_get_extension();
    test_ascii_to_binary();
    test_delete_directory();
    test_create_random_directory();
    delete_dir_contents("/tmp");
    test_run_syscall();

    _exit(0);
}
