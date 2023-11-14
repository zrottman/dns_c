# include <assert.h>
# include <stdio.h>
# include "../src/dns.h"


void test_assert_is_true(void){
    assert(1 == 1);
    assert(2 == 2);
}

void test_decode_dns_name(){
    char res[20] = {0};  //<- 03www07example03com
    size_t size = encode_dns_name("www.example.com", res);
    // NOTE: we have discovered that we don't know what to do with the output of this funciton. So we aren't going to test it right now
    assert(strcmp(res,"\03www\07example\03com") == 0);
}

// void test_somthing_complicated(pointer, input){
//     assert(somethingWeKnowIsTrue)
// }

int main(int argc, char const *argv[])
{
    printf("testing assert\n");
    test_assert_is_true();
    printf("passed!\n");
    printf("testing decode dns name\n");
    test_decode_dns_name();
    printf("passed!\n");
    return 0;
}
