#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

typedef struct
{
	int codigo;
	char raca[30], nome[100];
} registro;

typedef struct
{
	int codigo, offset;
} indice1;

typedef struct
{
	char vacina[30];
	int offset;
} indice2;

int Menu();
void AbreArquivos(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
void CadastraCachorro(FILE **AP2);
void AtualizaInfoIndice(char status, FILE **arq);
int ExisteCachorro(int codigo, FILE **AP2);

int main() 
{
    int opcao;
	FILE *AP1, *AP2, *IndPrim, *IndSec1, *IndSec2;
	
	AbreArquivos(&AP1, &AP2, &IndPrim, &IndSec1, &IndSec2);
	opcao = Menu();
	while (opcao != 0)
	{
	    switch(opcao)
	    {
	        case 1: CadastraCachorro(&AP2);
        }
        opcao = Menu();
    }
	
	fclose(AP1);
	fclose(AP2);
	fclose(IndPrim);
	fclose(IndSec1);
	fclose(IndSec2);
	return 0;
}

int Menu()
{
	int opcao = -1;
	
	while ((opcao != 1) && (opcao != 0))
	{
		system("CLS");
        printf("\n1 - Cadastra Cachorro");
		printf("\n0 - Sair");
		printf("\n\nEscolha a opcao: ");
        scanf("%d", &opcao);
    }
	return opcao;
}

/*
DESCRIÇÃO: Verifica se os arquivos já foram criados.
           Se não, cria-os.
PARÂMETROS: AP1 - Arquivo Principal 1
            AP2 - Arquivo Principal 2
	        IndPrim - Arquivo de Índice (busca por chave primária)
			IndSec1 - Arquivo de Índice 1 (busca por chave secundária)
			IndSec2 - Arquivo de Índice 2 (busca por chave secundária)
*/
void AbreArquivos(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2)
{
    if ((*AP1 = fopen("AP1.bin", "r+b")) == NULL)
    {
    	*AP1 = fopen("AP1.bin", "w+b");
    	fprintf(*AP1, "%d", -1);
    	*IndPrim = fopen("IndPrim.bin", "w+b");
    	AtualizaInfoIndice('!', IndPrim);
    	*IndSec1 = fopen("IndSec1.bin", "w+b");
    	AtualizaInfoIndice('!', IndSec1);
    	*IndSec2 = fopen("IndSec2.bin", "w+b");
    	AtualizaInfoIndice('!', IndSec2);
	}
	/*else if (ExigeRecriaIndice(IndPrim))
	{
		RecriaIndicePrim(AP1);
		QuickSortInd1(INDEX1, 0, tam1);
	}
	else
	{
		IndPrim = fopen("IndPrim.bin", "r+b");
		CarregaIndice(IndPrim, 1);
    	IndSec1 = fopen("IndSec1.bin", "r+b");
    	CarregaIndice(IndSec1, 2);
    	IndSec2 = fopen("IndSec2.bin", "r+b");
    	CarregaIndice(IndSec2, 1);
	}*/
    	
    if ((*AP2 = fopen("AP2.bin", "r+b")) == NULL)
        *AP2 = fopen("AP2.bin", "w+b");
    
}

/*
DESCRIÇÃO: Cadastra informações de um cachorro no Arquivo Principal 2
PARÂMETROS: AP2 - Arquivo Principal 2
*/
void CadastraCachorro(FILE **AP2)
{
	registro reg;
	
	system("CLS");
	printf("\nCodigo: ");
	scanf("%d", &reg.codigo);
	while (ExisteCachorro(reg.codigo, AP2))
	{
		system("CLS");
		printf("\nCodigo ja cadastrado. Digite novamente!");
		getch();
		system("CLS");
		printf("\nCodigo: ");
		scanf("%d", &reg.codigo);
	}
	fflush(stdin);
	printf("Raca: ");
	gets(reg.raca);
	printf("Nome do Cachorro: ");
	gets(reg.nome);

	
	fseek(*AP2, 0, SEEK_END); //seta a posição para o fim do arquivo.
	fwrite(&reg, sizeof(reg), 1, *AP2); //escreve no fim do arquivo.
}

/*
DESCRIÇÃO: Atualiza o header do índice com o status de atualização
PARÂMETROS: status - '*' - Índice atualizado
                     '!' - Índice desatualizado
            arq - Índice a ser atualizado
*/
void AtualizaInfoIndice(char status, FILE **arq)
{
	fseek(*arq, 0, SEEK_SET);
	fputc(status, *arq);	
}

/*
DESCRIÇÃO: Verifica se o código já existe no arquivo.
PARÂMETROS: codigo - Código a ser verificado
            AP2 - Arquivo Principal 2
RETORNOS: 0 - Não existe um cachorro com o código passado por parâmetro
          1 - Existe um cachorro com o código passado por parâmetro
*/
int ExisteCachorro(int codigo, FILE **AP2)
{
	int i = 0;
	registro reg;
	
	rewind(*AP2);
	while (fread(&reg, sizeof(registro)*i, 1, *AP2) >= 1)
	{
		if (reg.codigo == codigo)
		{
			return 1;
			break;
		}
		i++;		
	}
    return 0;	
}

