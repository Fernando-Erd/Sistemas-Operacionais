#include <stdio.h>
using namespace std;
int n,i, h,m,a;
int main () {
    scanf ("%d", &n);
    for (i = 0; i < n; i++) {
        scanf ("%d %d %d", &h, &m, &a);
        if (a == 1)
            if (h == 0)
                printf ("00:%d - A porta abriu!\n",m);
            else if (m == 0)
                printf ("%d:00 - A porta abriu!\n", h);
            else
                printf ("%d:%d - A porta abriu!\n", h,m);
        else 
            if (h == 0)
                printf ("00:%d - A porta fechou!\n",m);
            else if (m == 0)
                printf ("%d:00 - A porta fechou!\n", h);
            else
                printf ("%d:%d - A porta fechou!\n", h, m);
    }
}
