#define TEST_MACRO_2 1
int main(){
        int pp_ans = 0;
// test #if - #elif - #endif directive.
#if 0
    assert(1, 0);
#elif 0
    assert(1, 0);
#elif 1
    pp_ans = 20;
#endif
    //assert(pp_ans, 20);
    return pp_ans;
}