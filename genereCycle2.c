#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// génère cycle par échantillonage de toute la période
// et recherche de la plage correspondante

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

// retourne le nb de sec écoulées depuis date de référence. 1 1 ref à 00:00:00
int to_sec(int aref, int y1, int m1, int j1, int h1, int mn1, int s1)
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


// retourne les dates de début et fin, et le caractère Lieu
void dec_ligne(char *lg, int y0, int ns0, int *ns1, int *ns2, int *l)
{
    int j1, m1, y1, h1, mn1, s1;
    int  j2, m2, y2, h2, mn2, s2;
    char ch[10];
    
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
    
    if (lg[40]=='T') *l = 1; else *l = 0;
    
    //fprintf(stderr,"%d %d %d %d %d %d\n",y1, m1, j1, h1, mn1, s1);
    //fprintf(stderr,"%d %d %d %d %d %d\n",y2, m2, j2, h2, mn2, s2);
    *ns1 = to_sec(y0, y1, m1, j1, h1, mn1, s1) - ns0;
    *ns2 = to_sec(y0, y2, m2, j2, h2, mn2, s2) - ns0;
    
    return ;
}


int recherche_periode(int t,int periodes[],int deb,int fin)
{
    int res = -1;
    int i = deb;
    int cont = 1;

    while (cont==1) {
        if (i>=fin)cont = 0;
        else if ((periodes[3*i]<=t) && (t<=periodes[3*i+1])) {
            res = i; cont = 0; 
        }
        else i = i+1;
    }
    return res;
}

int main(int argc, char **argv)
{
    int ns, nm, y, m, t, p, e,deb,l;
    int y0, m0, j0, h0, mn0, s0, t0;
    int y1, m1, j1, h1, mn1, s1, t1;
    int ns1, ns2;
    int step=3600*24;
    FILE *f;
    char buf[100];
    int d=0,i;
    int *periodes=NULL;
    int nbl, lg;
    unsigned char *motif=NULL;
        
    y0 = atoi(argv[2]); m0 = atoi(argv[3]); j0 = atoi(argv[4]); h0 = atoi(argv[5]); mn0 = atoi(argv[6]); s0 = atoi(argv[7]);
    y1 = atoi(argv[8]); m1 = atoi(argv[9]); j1 = atoi(argv[10]); h1 = atoi(argv[11]); mn1 = atoi(argv[12]); s1 = atoi(argv[13]);
    step = atoi(argv[14]);
    
    // comptage des lignes
    FOPEN (f, argv[1], "r" );
    nbl = -1; // la 1ère ligne est bidon
    while (fgets(buf, 99, f)) {
        if (buf[4]=='-') nbl++;
    }
    fclose(f);

    t0 = to_sec(1990, y0, m0, j0, h0, mn0, s0);
    t1 = to_sec(1990, y1, m1, j1, h1, mn1, s1);

    periodes = (int *) malloc(nbl*3*sizeof(int));
    lg = (t1-t0)/step + 1;
   
    motif = (unsigned char *) malloc(lg);
    FOPEN (f, argv[1], "r" );
    fgets(buf, 99, f);
    nbl = 0;
    while (fgets(buf, 99, f)) {
        if (buf[4]!='-') continue;
        dec_ligne(buf, 1990, 0, &ns1, &ns2, &l);
        periodes[3*nbl] = ns1;
        periodes[3*nbl+1] = ns2;
        periodes[3*nbl+2] = l;
        //fprintf(stderr,"%d %d %d\n",periodes[3*nbl],periodes[3*nbl+1],periodes[3*nbl+2]);
        nbl++;
    }
    fclose(f);
    
    fprintf(stderr,"ok 2 lg=%d t0=%d t1=%d\n",lg,t0,t1);
    if (t0<periodes[0]) {
        fprintf(stderr,"Date départ antérieure aux données\n");
        exit (-1);
    }
    if (t1>periodes[3*(nbl-1)+1]) {
        fprintf(stderr,"Date fin postérieure aux données\n");
        exit (-1);
    }

    // boucler depuis date départ jusque date de fin avec pas d'échantillonage variable
    FOPEN (f, argv[15], "w" );

    i = 0;
    deb=0;
    for (t=t0;t<=t1;t+=step)
    {
        p  = recherche_periode(t,periodes,deb,nbl);
        deb = p;
        e = periodes[p*3+2];
        motif[i]= ( char)e;
        //fprintf(stderr,"(%d:%d)%d ",t, p, e);
        fprintf(stderr,"%d",e);

        //fprintf(f,"%d",e);
        if (e==0) fprintf(f,"o"); else fprintf(f,"i");
        i = i+1;
    }
    fclose(f);
    fprintf(stderr,"\n");

     
    return 0;
}