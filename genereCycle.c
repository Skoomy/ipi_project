#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// génère cycle par échantillonnage de chaque plage

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

// retourne le nb de sec écoulées depuis date de référence. 1 1 1990 à 00:00:00
int to_hms(int coeff, int aref, int y1, int j1, int m1, int h1, int mn1, int s1)
{
    int nbs = 0;
    int a,d;
    
    for (a=aref; a<y1; a++)
    {
        nbs += 365*24*60*60;
        if (bissextile(a)) nbs += 24*60*60;
    }

    for (d=1; d<m1; d++)
    {
        switch (d) {
            case  1: nbs += 31*24*60*60; break;
            case  2: if (bissextile(y1)) nbs += 29*24*60*60; else nbs+=28*24*60*60;break;
            case  3: nbs += 31*24*60*60;break;
            case  4: nbs += 30*24*60*60;break;
            case  5: nbs += 31*24*60*60;break;
            case  6: nbs += 30*24*60*60;break;
            case  7: nbs += 31*24*60*60;break;
            case  8: nbs += 31*24*60*60;break;
            case  9: nbs += 30*24*60*60;break;
            case 10: nbs += 31*24*60*60;break;
            case 11: nbs += 30*24*60*60;break;
            case 12: nbs += 31*24*60*60;break;
        }
    }
    
    nbs += j1*24*60*60;
    
    nbs += h1*60*60;
    
    nbs += mn1*60;
    
    nbs += s1; 
    
    return nbs;
}


// retourne le nb de secondes écoulées entre début et fin
int dec_ligne(char *lg, int *y, int *m, char *lieu)
{
    int j1, m1, y1, h1, mn1, s1;
    int  j2, m2, y2, h2, mn2, s2;
    char ch[10];
    unsigned int ns1, ns2;
    
    //debut
    strncpy(ch, lg, 4);
    ch[4]='\0';
    y1 = atoi(ch);
    
    strncpy(ch, lg+5, 2);
    ch[2]='\0';
    m1 = atoi(ch);
    
    strncpy(ch, lg+8, 2);
    ch[2]='\0';
    j1 = atoi(ch);
    
    strncpy(ch, lg+11, 2);
    ch[2]='\0';
    h1 = atoi(ch);
    
    strncpy(ch, lg+14, 2);
    ch[2]='\0';
    mn1 = atoi(ch);

    strncpy(ch, lg+17, 2);
    ch[2]='\0';
    s1 = atoi(ch);
    
    // fin
    strncpy(ch, lg+20, 4);
    ch[4]='\0';
    y2 = atoi(ch);

    strncpy(ch, lg+5+20, 2);
    ch[2]='\0';
    m2 = atoi(ch);
    
    strncpy(ch, lg+8+20, 2);
    ch[2]='\0';
    j2 = atoi(ch);
    
    strncpy(ch, lg+11+20, 2);
    ch[2]='\0';
    h2 = atoi(ch);
    
    strncpy(ch, lg+14+20, 2);
    ch[2]='\0';
    mn2 = atoi(ch);
    
    strncpy(ch, lg+17+20, 2);
    ch[2]='\0';
    s2 = atoi(ch);

    *lieu = lg[40];
    *y = y1;
    *m = m1;
    
//    fprintf(stderr,"%d %d %d, %d %d %d\n",j1, m1, y1, h1, mn1, s1);
//    fprintf(stderr,"%d %d %d, %d %d %d -- %c\n",j2, m2, y2, h2, mn2, s2, *lieu);
    
    // coeff 1: secondes
    // coeff 60: minutes
    // coeff 3600: heures
    
    ns1 = to_sec(y1, y1, j1, m1, h1, mn1, s1);
    ns2 = to_sec(y1, y2, j2, m2, h2, mn2, s2);
    
//    fprintf(stderr,"%d %d : %dh\n", ns1, ns2, (ns2-ns1)/(60*60));

    return (ns2-ns1);

}


int main(int argc, char **argv)
{
    int ns, nm, y, m, y1, m1; //ns: nbre sec ; nm: nbre min
    char l, o=' ';
    FILE *f;
    char buf[100];
    int d=0,i;
  
    ns = dec_ligne("1997-09-01_00:00:00 1997-09-01_00:00:01 Mer",&y1, &m1, &l);
    FOPEN (f, argv[1], "r" ); //ouverture du fichier (en argument 1)
    while (fgets(buf, 99, f)) { //lecture d'une ligne
        nm = dec_ligne(buf,&y, &m, &l)/60; //nbre de min de la ligne
        if (nm>0)
        {
        if ((o==l)||(o==' '))
            d = d+nm;
        else {
            
            fprintf(stderr,"%2d/%d : %3d h - %c : ",m1,y1,d,o);
            for(i=0;i<d;i++) if (o=='T') fprintf(stderr,"1"); else fprintf(stderr,"0");
            fprintf(stderr,"\n");
            d = nm;
            y1=y;m1=m;
        }
        o=l;
        }
        //fprintf(stderr,"%c",l);
        
    }
    fclose(f);

/*
    FOPEN (f, argv[1], "r" );
    while (fgets(buf, 99, f)) {
        nm = dec_ligne(buf,&y, &m, &l)/60;
        if ((o==l)||(o==' '))
            d = d+nm;
        else {
    //        fprintf(stderr,"%2d/%d : %3d h - %c\n",m,y,d,o);
            for(i=0;i<d;i++)
            
            if (o=='T') fprintf(stderr,"1"); else fprintf(stderr,"0");
            d = nm;
        }
        o=l;
        //fprintf(stderr,"%c",l);
        
    }
    fclose(f);
 */
  
    return 0;
}