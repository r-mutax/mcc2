int main(){
    struct abc{
        short a;
        char b;
    };

    struct abc c;
    c.a = 12;

    return c.a; 
}