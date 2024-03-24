#define TEST_MACRO_2 1
int main(){
        int pp_ans = 0;
#define TEST_DEF 1
#if defined TEST_DEF
    assert(1, 0);
#endif
    //assert(pp_ans, 20);
    return pp_ans;
}