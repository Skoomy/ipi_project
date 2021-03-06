#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Male
// accouplement T 5-20 j
// couvaison environ 53 j T 15-20j M 16-19j T idem
// élevage 4 phases M T M T de 5 à 10 j Total 30j
// crèche M

// Femelle
// accouplement T 5-20 j
// couvaison environ 53 j M 15-20j T 16-19j M idem
// élevage 4 phases T M T M de 5 à 10 j Total 30j
// crèche T < 3j M

#define FOPEN(f,nf,mode)\
f = fopen(nf,mode);\
if (f==NULL)\
{\
fprintf(stderr,"erreur ouverture %s en lecture\n",nf);\
}


int bissextile(int a)
{
	int res;
    
	if(a%4 != 0) res = 0;
	else {
		if (a%100 != 0) res = 1;
		else {
			if (a%400 == 0) res = 1;
			else res = 1;
		}
	}
	return res;
}

int duree(int m, int y)
{
    int res;
        switch (m) {
            case  1: res = 31; break;
            case  2: if (bissextile(y)) res=29; else res=28;break;
            case  3: res = 31;break;
            case  4: res = 30;break;
            case  5: res = 31;break;
            case  6: res = 30;break;
            case  7: res = 31;break;
            case  8: res = 31;break;
            case  9: res = 30;break;
            case 10: res = 31;break;
            case 11: res = 30;break;
            case 12: res = 31;break;
        }
    return res;
}

void avance(int y0,int m0,int j0,int dec,int *y1,int *m1,int *j1)
{
    int i;
    *y1 = y0;
    *m1 = m0;
    *j1 = j0;
    for(i=0;i<dec;i++)
    {
        if ((*j1)<duree(*m1,*y1)) (*j1)++;
        else {
            (*j1)=1;
            if ((*m1)<12) (*m1)++;
            else {
                (*m1)=1;
                (*y1)++;
            }

        }
    }
}


int main(int argc, char **argv)
{
    int y0, m0, j0, h0, mn0, s0, t0;
    int y1, m1, j1, h1, mn1, s1, t1;
    int ns1, ns2;

    FILE *f;
    char buf[100];
    int d=0,i;
    int *periodes=NULL;
    int nbl, lg;
    unsigned char *motif=NULL;
    int l;
    char *lieu[2]={"Mer", "Terre"};
    
    y0 = atoi(argv[2]); m0 = atoi(argv[3]); j0 = atoi(argv[4]);
    if (argv[5][0]=='M') {
        l=0;
    } else {
        l=1;
    }
    FOPEN (f, argv[1], "w" );
    fprintf(f,"Début => Fin => Lieu\n");
    for(i=6;i<argc;i++)
    {
        avance(y0,m0,j0,atoi(argv[i]),&y1,&m1,&j1);
        fprintf(f,"%4d-%02d-%02d_00:00:00 %4d-%02d-%02d_00:00:00 %s\n",y0,m0,j0,y1,m1,j1,lieu[l]);
        l = !l;
        y0=y1; m0=m1; j0=j1;
    }
    fclose(f);
     
    return 0;
}
