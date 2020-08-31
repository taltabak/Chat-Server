#include <stdio.h>  
#include <stdlib.h>  

void swap(int* a , int* b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

void arraySort(int* arr, int n)
{
    int a = arr[0] , b ,c;
    for(int i =1 ; i<n ; i++ )
    {
        if(arr[i] != a)
        {
            b = arr[i];
            i++;
            for( ; i < n ; i++ )
            {
                if(arr[i] != a && arr[i] != b)
                {
                    c = arr[i];
                    break;
                }
            }
            break;
        }
    }
    if(a > c)
        swap(&a, &c);
    if(a > b)
        swap(&a,&b);
    if(b > c)
        swap(&b,&c);

// NOW a smallest b middle c biggest
    int l = 0, m = 0, r = n-1;
    while(m < n)
    {
        if(arr[m] == a && m != 0)
        {
            swap(&(arr[m]),&(arr[l]));
            m++;
            l++;
        }
        if (arr[m] == b)
        {
            swap(&(arr[m]),&(arr[m+1]));
            m++;
        }
        if (arr[m] == b)
        {
            swap(&(arr[m]),&(arr[r]));
            m++;
            r--;
        }
    }
}

int main ()
{
    int arr[] = {0,1,2,2,1,0,1,0,0,0,0,2,2,1,0,1,2,1,0,2,1,0,1,1,2,2,0,1,0,2};
    arraySort(arr,30);
    for(int i = 0; i<30; i++)
        printf("%d",arr[i]);


    return 0;
}