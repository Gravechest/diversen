#include <stdio.h>

void main(){
    int n1 = 3;
    int n2 = 2;
    long long priem = 1;

    while(1)
    {
        while(n1 > n2)
        {
            if(n1 % n2 == 0)
            {
                priem = 0;
            }
            n2++;
        }
        if(priem)
        {
            printf("%i\n",n1);
        }
        n1++;
        n2 = 2;
        priem = 1;
    }
}
