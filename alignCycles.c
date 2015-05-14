#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//./align01 110010101011 100101001

// definition (eventuelle) du plus grand entier (signe)
#ifndef MAXINT
#define MAXINT ((2<<31)-1)
#endif

void affMat(int mat[], int nl, int nc)
{
	int i, j;
	
	for(i=0; i<nl; i++) {
		for(j=0; j<nc; j++)
			printf("%3d ",mat[i*nc+j]);
		printf("\n");
	}
}


int lettre(char c)
{
    return((c>='a')&&(c<='z'));
}

// -----------------------------------------------------------------------------
// Calcul du score élémentaire d'alignement de caractères
// 1 si caractères identiques, 0 sinon.
// -----------------------------------------------------------------------------

int getScore(char c1, char c2)
{
	return (c1 == c2);
}

// -----------------------------------------------------------------------------
// Calcul de la matrice de scores :
// alignements des chaines de caracteres 2 à 2
// -----------------------------------------------------------------------------
int * matScore(char *s1,char *s2,int l1,int l2)
{
	int i, j;
	int *ms=NULL;
    
	// allocation dynamique de la matrice de scores
	ms = calloc(l1*l2, sizeof(int));
    
	for(i=0; i<l1; i++)
		for(j=0; j<l2; j++)
			ms[i*l2+j] = getScore(s1[i], s2[j]);
	return ms;
}

// -----------------------------------------------------------------------------
// Calcul du score élémentaire d'alignement de caractères
// 1 si caractères identiques, 0 sinon.
// -----------------------------------------------------------------------------
int getScorePds(char c1, char c2, int poids1, int poids2)
{
    int r = 4;
    /*if (c1==c2)
    {
        if ((r*poids1<poids2)||(r*poids2<poids1) ) return 0;
        else return 1;
    }
    else return 0;
    */return (c1 == c2)*(poids1+poids2);
	return (c1 == c2);
}

// -----------------------------------------------------------------------------
// Calcul de la matrice de scores :
// alignements des chaines de caracteres 2 à 2
// -----------------------------------------------------------------------------
int * matScorePds(char *s1,char *s2,int l1,int l2, int *lesPoids1, int *lesPoids2)
{
	int i, j;
	int *ms=NULL;

	// allocation dynamique de la matrice de scores
	ms = calloc(l1*l2, sizeof(int));

	for(i=0; i<l1; i++)
		for(j=0; j<l2; j++)
			ms[i*l2+j] = getScorePds(s1[i], s2[j], lesPoids1[i], lesPoids2[i]);
	return ms;
}

// affecte un poids à chaque échantillon : la longueur de la période à laquelle il appartient

void calculeLePoid(char *p1, int lg1, int *pds1)
{
    int i;
    int *tmp = (int *) calloc(lg1, sizeof(int));
    int j, k, v;
    
    j=0;
    tmp[0]=1;
    for(i=1;i<lg1;i++)
    {
        if(p1[i]!=p1[i-1]) j++;
        tmp[j]++; // longueur de plage
    }
    j=0;k=0;v=1;v=tmp[0]/2;
    for(i=0;i<lg1;i++)
    {
        pds1[i]=tmp[j]/2+v;
        pds1[i]=tmp[j];
        if (k<tmp[j]/2)v--;
        else v++;
        k++;
        if (k==tmp[j])
        {
            v=1;
            k=0;
            j++;
        }
        
    }
#if 0
    for(i=0;i<lg1;i++)
    {
        fprintf(stderr, "%d ", pds1[i]);
    }
    fprintf(stderr, "\n");
#endif
    
}

void calculeLesPoids(char *p1, char *p2, int lg1, int lg2, int *pds1, int *pds2)
{
    calculeLePoid(p1, lg1, pds1);
    calculeLePoid(p2, lg2, pds2);
}


// -----------------------------------------------------------------------------
// Calcul de la transformee d'un element
// Algo : mt[i][j] = ms[i][j] + max(ms[x][y])
// avec i<x<=m et y=j+1
// ou   x=i+1  et j<y<=n
// d'ou recherche du maximum dans ligne plus bas (partie vers la droite)
// et colonne à droite (partie vers le bas)
// -----------------------------------------------------------------------------
int transfo(int i, int j, int m, int n, int ms[], int mt[])
{
	int x, y, max=0;
	
	y=j+1; // colonne a droite
	for(x=i+1; x<m; x++) // jusqu'en bas 
		if (mt[x*n+y]>max) max = mt[x*n+y];
	x=i+1; // ligne en dessous
	for(y=j+1; y<n; y++) // jusqu'a droite
		if (mt[x*n+y]>max) max = mt[x*n+y];
	return ms[i*n+j]+max;
}

// -----------------------------------------------------------------------------
// Calcul de la matrice transformee
// dans l'ordre inverse de l'ordre habituel :
// du bas vers le haut, et de droite a gauche
// la derniere ligne et derniere colonnes sont simplement recopiees
// -----------------------------------------------------------------------------
void matriceTransformee(char s1[],char s2[],int l1,int l2,int ms[],int mt[])
{
	int i, j;

	// recopie de la deniere ligne
	i=l1-1;
	for(j=0; j<l2; j++)
		mt[i*l2+j] = ms[i*l2+j];
	// recopie de la derniere colonne
	j=l2-1;
	for(i=0; i<l1; i++)
		mt[i*l2+j] = ms[i*l2+j];
	
	//printf("Matrice transformÈe étape 1\n");
	//affMat(mt,l1,l2);
	
	
	
	// calcul de la matrice transformee
	for(i=l1-2; i>=0; i--)
		for(j=l2-2; j>=0; j--)
			mt[i*l2+j] = transfo(i, j, l1, l2, ms, mt);
}

int aligneChaines(char * p1, char * p2, int lg1, int lg2, int matT[], char **ch1, char **ch2)
{
	int res=0;
	int vmax ; // plus grande valeur de la matrice transformée
	int i, j, di, dj;
	int l=0; // longueur du resultat 
	int depi, depj ; // coordonnées de "depart"
	char *res1, *res2; // les chaines après insertion/suppression 
	int t=0; // mettre à 1 si on travaille sur des mots, 0 sur des chaines binaires.
	
	vmax = matT[0]; depi=0; depj=0; // on la suppose en (0,0)

	// on regarde si la 1ere colonne contient une valeur plus grande
	for (i=1; i<lg1; i++) {
		if (matT[i*lg2]>vmax) {
			vmax = matT[i*lg2];
			depi = i; depj = 0;
		}
	}
	for (j=1; j<lg2; j++) {
		if (matT[j]>vmax) {
			vmax = matT[j];
			depi = 0; depj = j;
		}
	}

	//printf("val max : %d (%d,%d)\n",vmax, depi, depj);
	
	// allocation : à revoir. 
	res1=(char *)calloc(lg1+lg2+1, 1);
	res2=(char *)calloc(lg1+lg2+1, 1);
	*ch1 = res1;
	*ch2 = res2;
	
	// ici ajouter les insertions necessaires si le debut n'est pas en (0,0)
	// fin ajout insertions necessaires si le debut n'est pas en (0,0)
	
	if ((depi==0)&&(depj==0))
	{
		res1[l]=p2[0];
		res2[l]=p1[0];
		if (res1[l]!=res2[l])
		{	res++;
            if (lettre(p2[0])) t=1; else t=0;
			res1[l]=p2[0]+t*('A'-'a');
            if (lettre(p1[0])) t=1; else t=0;
			res2[l]=p1[0]+t*('A'-'a');
		}
		l++;
		i=0; j=0;
	}
	else {
		for (i=0; i<depi; i++) {
			res1[l]='-';
            if (lettre(p1[i])) t=1; else t=0;
			res2[l]=p1[i]+t*('A'-'a');
			res++; l++;
		}
		for (j=0; j<depj; j++) {
            if (lettre(p2[j])) t=1; else t=0;
			res1[l]=p2[j]+t*('A'-'a'); 
			res2[l]='-';
			res++; l++;
		
		}
		i=depi-1;j=depj-1;
	}
	
	// parcours de la "diagonale" de la matrice
    while ((i<lg1-1) && (j<lg2-1)) {
		
		
		// on regarde dans les 3 directions possibles diagonale bas/droite, bas, et a droite
		// pour determiner ou se trouve la valeur max.
		di=1; dj=1; // diagonale
		if ((i<lg1-1)&&(j<lg2-1)) {
			vmax = matT[(i+1)*lg2+j+1] ; di=1; dj=1;
		}
		if ((i<lg1-1)&&(j<lg2)) { // vers le bas
			if (matT[(i+1)*lg2+j]>vmax) {
				vmax = matT[(i+1)*lg2+j] ; di=1; dj=0;
			}
		}
		if ((i<lg1)&&(j<lg2-1)) { // vers la droite
			if (matT[(i)*lg2+j+1]>vmax) {
				vmax = matT[(i)*lg2+j+1] ; di=0; dj=1;
			}
		}
	
		i+=di;
		j+=dj;
		
		// progression dans les chaines 
		if (dj==0) res1[l]='-'; 
		else res1[l]=p2[j];
		if (di==0) res2[l]='-';
		else res2[l]=p1[i];
		//printf("(%d,%d) (%d,%d)",i,j,di,dj);
		//printf("(%c,%c)\n",res1[l],res2[l]);
		if (res1[l]!=res2[l])
		{	res++;
            if (lettre(res1[l])) t=1; else t=0;
			if ((res1[l]!='-')) res1[l]=res1[l]+t*('A'-'a');
            if (lettre(res2[l])) t=1; else t=0;
			if ((res2[l]!='-')) res2[l]=res2[l]+t*('A'-'a');
		}
		l++;
	}
	
	// si on ne finit pas sur la diagonale, complêter avec des '-'

	while (j<lg2-1){
		res2[l]='-';
        if (lettre(p2[j+1])) t=1; else t=0;
		res1[l]=p2[j+1]+t*('A'-'a');
		l++;res++;
		j++;
	}
	while (i<lg1-1)
	{
		res1[l]='-';
        if (lettre(p1[i+1])) t=1; else t=0;
		res2[l]=p1[i+1]+t*('A'-'a');
		l++;res++;
		i++;
	}
	return res;
	
}





// -----------------------------------------------------------------------------
// Programme principal : les chaines sont passees en argument 
// de la ligne de commande. On ne teste pas leur validite (0 ou 1)
// exemple : align01 00011001010  1011001011
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	int *matS=NULL, *matT=NULL;
	int lg1, lg2,ld;
	char *p1, *p2;
	char *ch1,*ch2;
	int *pds1, *pds2;
    
	if (argc<4) {
		printf("Pas assez d'arguments ch1 ch2 n/p\n");
		exit(2);
	}

	//printf("%s %s \n",argv[1],argv[2]);
		   

	p1 = argv[1];
	p2 = argv[2];
	lg1 = strlen(p1);
	lg2 = strlen(p2);
	
    if (argv[3][0]=='p') {
    
    pds1 = (int *) calloc(lg1, sizeof(int));
    pds2 = (int *) calloc(lg2, sizeof(int));

    calculeLesPoids(p1, p2, lg1, lg2, pds1, pds2);
    
	// Allocation et calcul de la matrice des scores
	matS = matScorePds(p1, p2, lg1, lg2, pds1, pds2);
    }
    else 
	
    matS = matScore(p1, p2, lg1, lg2);
    
	// Affichage de la matrice de scores
	//printf("Matrice de scores\n");
    // affMat(matS,lg1,lg2);

	// allocation dynamique de la matrice transformee
	matT = calloc(lg1*lg2, sizeof(int));
	// Calcul de la matrice transformee
	matriceTransformee(p1, p2, lg1, lg2, matS, matT);

	// Affichage de la matrice transformee
	 //printf("Matrice transformÈe\n");
	 //affMat(matT,lg1,lg2);
	
	ld =  aligneChaines(p1, p2, lg1, lg2, matT, &ch1, &ch2);
	//printf("%s\n%s\n\n",p1,p2);
    printf("%s\n%s\ndistance de Levenhstein=%d (%.2lf%%)\n",ch2,ch1,ld,ld*100.0/strlen(ch1));

	return 0;
}

