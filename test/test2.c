#define ASSERT(X, Y)    ((X) == (Y) ? 0 : 1)

int t(){
    int data; data = 10;
    int* a; a = &data;
    // ASSERT(*a, 10);
    // ASSERT(a, &data);
    *a = 15;
    //ASSERT(data, 15);
    //ASSERT(sizeof a, 8);

    return 0;
}

int main(){
    t();
    return 0;
}