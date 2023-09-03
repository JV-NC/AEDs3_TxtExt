#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <ctype.h> //usado para remover pontuação e transformar em minuscula
#define TAM 36 //maior valor encontrado no arquivo exemplo
#define ARQUIVO "arquivo_texto.txt" //nome do arquivo que será aberto

typedef struct avl{ //struct de elementos da arvore avl
	char word[TAM];
	int rep;
	struct avl *esq;
	struct avl *dir;
	int fatbal;
}avl;

typedef struct{ //struct de uma palavra com frequencia
	char word[TAM];
	int rep;
}elemento;

typedef struct{ //struct da fila com ponteiro de elemento que sofrerá alocação dinâmica
	elemento *v;
	int q;
}fila;

//#FUNÇÕES AVL#
avl* criar_avl(); //cria avl, raiz nula
int altura(avl *r); //calcula altura avl
avl* rotdir(avl *r); //rotaciona raizes para direita
avl* rotesq(avl *r); //rotaciona raizes para esquerda
avl* balancear(avl *r); //balanceia avl usando rotacoes para dir e esq
avl* inserir_avl(avl *r,char str[],int n); //insere palavra na avl e balanceia se necessario
avl* remover_avl(avl *r,elemento *aux,char str[], int n); //remove palavras da avl
avl* buscar(avl *r,char str[],int n); //buscar palavra especifica na avl (não foi usado)
void in_ordem(avl *r); //imprime avl em ordem alfabetica (não foi usado)

//#FUNÇÕES FILA PRIORIDADE#
fila* criar_fp(int n);	//cria fila e define prioridades para -1
int cheia(fila fp,int n); //verifica se fila está cheia
int vazia(fila fp); //verifica se fila está vazia
void vrfy_sobe(fila *fp,int n); //reposiciona elementos após inserção
int inserir_heap(fila *fp, int n, elemento aux); //insere palavra na heap com a sua frequencia
void vrfy_desce(fila *fp,int r); //reposiciona elementos após remoção
int remover_heap(fila *fp,elemento *aux); //remove primeiro elemento da fila (FIFO)
void imprimir_heap(fila *fp, int n); //imprimir heap como vetor (não foi usado)

//FUNÇÕES DE ARQUIVO E TRATAMENTO#
void remover_pont(char str[],int n); //remove as pontuações das strs
void minuscula(char str[],int n); //transforma as letras em minuscula
avl* escanear(avl *r); //escaneia o arquivo, separa as palavras e joga na avl
int quanti(avl *r);	//calcula, com pos_ordem, a quantidade de nos da avl
void transfer(avl *r, fila *fp, int n); //transfere n palavras da avl para heap
void novo_arq(fila *fp,int n); //cria novo arquivo com as frequencias em ordem decrescente

int main(){ //João Vitor Neves Costa
	setlocale(LC_ALL,"Portuguese");
	
	avl *r=criar_avl();
	fila *fp;
	int n;
	
	r=escanear(r);
	n=quanti(r);
	printf("%d palavras distintas encontradas!\n",n);
	
	fp=criar_fp(n);
	transfer(r,fp,n);
	printf("palavras transferidas da AVL para FP com sucesso!\n");
	
	novo_arq(fp,n);
	
	return 0;
}

avl* criar_avl(){
	avl *r=NULL;
	return r;
}

int altura(avl *r){
	int esq,dir;
	if(r==NULL){
		return -1;
	}
	else{
		esq=altura(r->esq);
		dir=altura(r->dir);
		if(esq>dir){
			return esq+1;
		}
		else{
			return dir+1;
		}
	}
}

avl* rotdir(avl *r){
	avl *temp=r;
	r=temp->esq;
	temp->esq=r->dir;
	r->dir=temp;
	return r;
}

avl* rotesq(avl *r){
	avl *temp=r;
	r=temp->dir;
	temp->dir=r->esq;
	r->esq=temp;
	return r;
}

avl* balancear(avl *r){
	if(r->fatbal>1 && r->esq>=0){
		r=rotdir(r);
	}
	else if(r->fatbal<-1 && r->dir->fatbal<=0){
		r=rotesq(r);
	}
	else if(r->fatbal>1 && r->esq->fatbal<0){
		r->esq=rotesq(r->esq);
		r=rotdir(r);
	}
	else if(r->fatbal<-1 && r->dir->fatbal>0){
		r->dir=rotdir(r->dir);
		r=rotesq(r);
	}
	return r;
}

avl* inserir_avl(avl *r,char str[],int n){
	if(r==NULL){
		avl *novo=(avl*)malloc(sizeof(avl));
		strcpy(novo->word,str);
		novo->rep=1;
		novo->esq=NULL;
		novo->dir=NULL;
		novo->fatbal=0;
		r=novo;
		return r; //adicionando mais esse outro return o codigo parece funcionar finalmente
	}
	else if(strcmp(r->word,str)==0){
		r->rep++;
		return r; //adicionando esta linha durou mais a repetição de escanear
	}
	else if(strcmp(r->word,str)>0){
		r->esq=inserir_avl(r->esq,str,n);
		r->fatbal=altura(r->esq)-altura(r->dir);
	}
	else{
		r->dir=inserir_avl(r->dir,str,n);
		r->fatbal=altura(r->esq)-altura(r->dir);
	}
	r=balancear(r);
	return r;
}

avl* remover_avl(avl *r,elemento *aux,char str[],int n){
	if (r == NULL) {
		printf("Pifou!\n");
		return r;
	}
	if(strcmp(r->word,str)==0){
		avl *temp;
		int h;
		if(r->esq==NULL && r->dir==NULL){	//folha
			strcpy(aux->word,r->word);
			aux->rep=r->rep;
			r=NULL;
		}
		else if(r->esq!=NULL && r->dir==NULL){	//1 filho à esquerda
			strcpy(aux->word,r->word);
			aux->rep=r->rep;
			r=r->esq;
			//free(aux);
		}
		else if(r->esq==NULL && r->dir!=NULL){	//1 filho à direita
			strcpy(aux->word,r->word);
			aux->rep=r->rep;
			r=r->dir;
			//free(aux);
		}
		else{	//2 filhos
			temp=r->dir;	//procura o mais a esquerda na direita
			while(temp->esq!=NULL){
				temp=temp->esq;
			}
			strcpy(r->word,temp->word);	//troca os valores e remove o valor novamente
			h=r->rep;
			r->rep=temp->rep;
			strcpy(temp->word,str);
			temp->rep=h;
			r->dir=remover_avl(r->dir,aux,str,n);
		}
	}
	else if(strcmp(r->word,str)>0){
		r->esq=remover_avl(r->esq,aux,str,n);
	}
	else if(strcmp(r->word,str)<0){
		r->dir=remover_avl(r->dir,aux,str,n);
	}
	//avl não é rebalanciada, para diminuir a complexidade e impedir trava no codigo
	return r;
}

avl* buscar(avl *r,char str[],int n){
	if(r==NULL){
		printf("Falhou\n");
		return r;
	}
	else if(strcmp(r->word,str)==0){
		printf("%d %d\n",r->rep,r->fatbal);
		return r;
	}
	else if(strcmp(r->word,str)>0){
		r->esq=buscar(r->esq,str,n);
	}
	else{
		r->dir=buscar(r->dir,str,n);
	}
}

void in_ordem(avl *r){
	if(r->esq!=NULL){
		in_ordem(r->esq);
	}
	printf("%s\t%d\t%d\n",r->word,r->rep,r->fatbal);
	if(r->dir!=NULL){
		in_ordem(r->dir);
	}
}

fila* criar_fp(int n){
	int i;
	fila *fp=(fila*)malloc(sizeof(fila));
	fp->v=(elemento*)malloc(n*sizeof(elemento));
	fp->q=0;
	
	for(i=0;i<n;i++){
		fp->v[i].rep=-1;
	}
	return fp;
}

int cheia(fila fp,int n){
	if(fp.q>=n){
		return 1; //fila cheia
	}
	return 0;
}

int vazia(fila fp){
	if(fp.q==0){
		return 1;	//fila vazia
	}
	return 0;
}

void vrfy_sobe(fila *fp,int n){
	int p=(n-1)/2; //calcula pai a partir do filho
	elemento aux;
	while(n>0 && fp->v[p].rep<fp->v[n].rep){
		aux=fp->v[p];
		fp->v[p]=fp->v[n];
		fp->v[n]=aux;
		n=p;
		p=(n-1)/2;
	}
}

int inserir_heap(fila *fp,int n, elemento aux){
	if(fp!=NULL && cheia(*fp,n)==0){
		fp->v[fp->q]=aux;
		vrfy_sobe(fp,fp->q);
		fp->q++;
		return 1;
	}
	return 0;
}

void vrfy_desce(fila *fp,int r){
	int s=2*r+1; //calculando filho
	elemento aux;
	while(s<fp->q){	//enquanto filho ainda estiver dentro de quantidade
		if(s<fp->q-1){ //se indice filho ainda está disponivel
			if(fp->v[s].rep<fp->v[s+1].rep){ //se filho é menor que seu irmão
				s++;
			}
		}
		if(fp->v[r].rep>=fp->v[s].rep){ //se a prioridade do pai for maior ou igual a prioridade do filho
			break;
		}
		aux=fp->v[r];
		fp->v[r]=fp->v[s];
		fp->v[s]=aux;
		r=s;
		s=2*r+1;
	}
}

int remover_heap(fila *fp,elemento *aux){
	if(fp!=NULL && vazia(*fp)!=1){
		fp->q--;
		*aux=fp->v[0];	//salvar prioridade removida
		fp->v[0]=fp->v[fp->q];
		fp->v[fp->q].rep=-1;	//redefine posição vazia como -1
		vrfy_desce(fp,0);
		return 1;
	}
	return 0;
}

void imprimir_heap(fila *fp, int n){
	int i;
	for(i=0;i<n;i++){
		printf("'%s' : %d\n",fp->v[i].word,fp->v[i].rep);
	}
	printf("%d\n",i);
}

void remover_pont(char str[],int n){
	int i,j;
	for(i=0,j=0;i<strlen(str);i++){ //loop até o tamanho da str
		if(isalpha(str[i])){ //verifica se caracter é uma letra
			str[j]=str[i];
			j++;
		}
	}
	str[j]='\0'; //define fim da nova palavra
}

void minuscula(char str[],int n){
	int i;
	for(i=0;i<strlen(str);i++){	//percorre palavra
		str[i]=tolower(str[i]); //função transforma todas as letras em minuscula
	}
}

avl* escanear(avl *r){
	FILE *arq=fopen(ARQUIVO,"r");
	char str[TAM];
	if(arq==NULL){
		printf("Falha em abrir o arquivo!\n");
		exit(1);
	}
	else{
		printf("Arquivo aberto com sucesso!\n");
	}
	
	while(fscanf(arq,"%s",str)!=EOF){ //faz o tratamento da palavra e inseri na avl
		remover_pont(str,TAM);
		minuscula(str,TAM);
		r=inserir_avl(r,str,TAM);
	}
	fclose(arq);
	return r;
	
}

int quanti(avl *r){
	int n=1;
	if(r->esq!=NULL){
		n+=quanti(r->esq);
	}
	if(r->dir!=NULL){
		n+=quanti(r->dir);
	}
	return n;
}

void transfer(avl *r, fila *fp, int n){
	int i,t;
	elemento aux;
	char str[TAM];
	
	for(i=0;i<n;i++){
		t=strlen(r->word);	//passa valores para função para evitar recursividade
		strcpy(str,r->word);
		r=remover_avl(r,&aux,str,t); //remove raiz principal da avl
		inserir_heap(fp,n,aux); //insere aux na heap
	}
}

void novo_arq(fila *fp,int n){
	elemento aux;
	FILE *novo=fopen("frequencia.txt","w");
	if(novo==NULL){
		printf("Falha em criar arquivo de frequência!\n");
		exit(1);
	}
	else{
		printf("Arquivo 'frequência' criado com sucesso!\n");
	}
	int i;
	for(i=0;i<n;i++){
		remover_heap(fp,&aux);
		fprintf(novo,"'%s' : %d\n",aux.word,aux.rep);
	}
	fclose(novo);
}

