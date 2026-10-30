#include "unity.h"
#include "utils.h"
#include "string.h"
#include <direct.h>
#include <stdio.h>

void setUp(void){
    _mkdir("tests/tmp");
}

void tearDown(void){
    remove("tests/tmp/A/A.txt");
    remove("tests/tmp/A/B/B.txt");
    remove("tests/tmp/A/B/C/C.txt");
    remove("tests/tmp/dir/test.txt");

    _rmdir("tests/tmp/A/B/C");
    _rmdir("tests/tmp/A/B");
    _rmdir("tests/tmp/A");

    _rmdir("tests/tmp/dir");
    
    _rmdir("tests/tmp");
    
}

void test_lowercasing(void){
    char s1[] = "PeGglE";
    char s2[] = "peggle";

    to_lower_string(s1);

    int same = strcmp(s1,s2);
    TEST_ASSERT_EQUAL(0,same);
}

void test_identical(void){
    char s1[] = "halo";
    char s2[] = "halo";

    int dist = Lev_Distance(s1,s2);
    TEST_ASSERT_EQUAL(0,dist);

}

void test_blank(void){
    char s1[] = "";
    char s2[] = "";
    char s3[] = "abc";

    int dist = Lev_Distance(s1,s2);
    TEST_ASSERT_EQUAL(0,dist);

    dist = Lev_Distance(s2,s3);
    TEST_ASSERT_EQUAL(3,dist);

    dist = Lev_Distance(s3,s2);
    TEST_ASSERT_EQUAL(3,dist);
}

void test_single_edit(void){
    char s1[] = "cat";
    char s2[] = "cats";
    char s3[] = "cut";

    int dist = Lev_Distance(s1,s2);
    TEST_ASSERT_EQUAL(1,dist);

    dist = Lev_Distance(s2,s1);
    TEST_ASSERT_EQUAL(1,dist);

    dist = Lev_Distance(s1,s3);
    TEST_ASSERT_EQUAL(1,dist);
}

void test_mix_edit(void){
    char s1[] = "kitten";
    char s2[] = "sitting";

    int dist = Lev_Distance(s1,s2);
    TEST_ASSERT_EQUAL(3,dist);
}

void test_empty(void){
    char path[] = "tests/tmp";
    long long size = get_dir_size(path);

    TEST_ASSERT_EQUAL_INT64(size,0);
}

void test_nested(void)
{
    TEST_ASSERT_EQUAL_INT(0, _mkdir("tests/tmp/A"));
    TEST_ASSERT_EQUAL_INT(0, _mkdir("tests/tmp/A/B"));
    TEST_ASSERT_EQUAL_INT(0, _mkdir("tests/tmp/A/B/C"));

    const char *paths[] = {
        "tests/tmp/A/A.txt",
        "tests/tmp/A/B/B.txt",
        "tests/tmp/A/B/C/C.txt"
    };

    const char *contents[] = {
        "A",
        "BBBB",
        "CC"
    };

    for (int i = 0; i < 3; i++) {
        FILE *file = fopen(paths[i], "wb");
        TEST_ASSERT_NOT_NULL_MESSAGE(file, paths[i]);

        size_t expected_bytes = strlen(contents[i]);
        size_t written_bytes =
            fwrite(contents[i], 1, expected_bytes, file);

        int close_result = fclose(file);

        TEST_ASSERT_EQUAL_MESSAGE(
            expected_bytes, written_bytes, paths[i]);
        TEST_ASSERT_EQUAL_INT_MESSAGE(
            0, close_result, paths[i]);
    }

    char root[] = "tests/tmp";
    long long total = get_dir_size(root);

    TEST_ASSERT_EQUAL_INT64(7, total);
}

void test_type(void){

    char p1[] = "tests/tmp/dir";
    char p2[] = "tests/tmp/dir/test.txt";
    char p3[] = "this/is/fake/nope.txt";


    TEST_ASSERT_EQUAL_INT(0, _mkdir("tests/tmp/dir"));
    int t1 = get_type(p1);
    TEST_ASSERT_EQUAL_INT(1,t1);

    
    FILE *file = fopen(p2, "w");
    fclose(file);
    int t2 = get_type(p2);
    TEST_ASSERT_EQUAL_INT(2,t2);

    int t3 = get_type(p3);
    TEST_ASSERT_EQUAL_INT(0,t3);
}

int main(void)
{
    UNITY_BEGIN();
    
    RUN_TEST(test_identical);
    RUN_TEST(test_blank);
    RUN_TEST(test_single_edit);
    RUN_TEST(test_mix_edit);
    RUN_TEST(test_lowercasing);
    RUN_TEST(test_empty);
    RUN_TEST(test_nested);
    RUN_TEST(test_type);

    
    return UNITY_END();   
}