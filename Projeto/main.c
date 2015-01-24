#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

typedef struct
{
	int codigo;
	char raca[30], nome[100];
} registro;

typedef struct
{
	int codigo, offset;
} indiceprim;

void AbreArquivos(FILE *AP1, FILE *AP2, FILE *IndPrim, FILE *IndSec1, FILE *IndSec2);
int ExisteCachorro(int codigo, FILE *AP2);
void CadastraCachorro(FILE *AP2);
int ExigeRecriaIndice(FILE *arq);
void AtualizaInfoIndice(char status, FILE *arq);
void RecriaIndices();

int main()
{
	FILE *AP1, *AP2, *IndPrim, *IndSec1, *IndSec2;
    
    
    AbreArquivos(AP1, AP2, IndPrim, IndSec1, IndSec2);
    getch();
    return 0;
}

/*DESCRIÇÃO: Verifica se os arquivos já foram criados.
             Se não, cria-os.
  PARÂMETROS: AP1 - Arquivo Principal 1
              AP2 - Arquivo Principal 2
			  IndPrim - Arquivo de Índice (busca por chave primária)
			  IndSec1 - Arquivo de Índice 1 (busca por chave secundária)
			  IndSec2 - Arquivo de Índice 2 (busca por chave secundária) */
void AbreArquivos(FILE *AP1, FILE *AP2, FILE *IndPrim, FILE *IndSec1, FILE *IndSec2)
{
    if ((AP1 = fopen("AP1.bin", "r+b")) == NULL)
    {
    	AP1 = fopen("AP1.bin", "w+b");
    	fprintf(AP1, "%d", -1);
    	//AtualizaInfoIndice('*', IndPrim);
    	//AtualizaInfoIndice('*', IndSec1);
    	//AtualizaInfoIndice('*', IndSec2);			
	}
	/*else if (ExigeRecriaIndice(IndPrim))
	{
		
	}*/
    	
    if ((AP2 = fopen("AP2.bin", "r+b")) == NULL)
        AP2 = fopen("AP2.bin", "w+b");
}

/*DESCRIÇÃO: Verifica se o código já existe no arquivo.
  PARÂMETROS: codigo - Código a ser verificado
              AP2 - Arquivo Principal 2
  RETORNOS: 0 - Não existe um cachorro com o código passado por parâmetro
  			1 - Existe um cachorro com o código passado por parâmetro*/
int ExisteCachorro(int codigo, FILE *AP2)
{
	int i = 0;
	registro reg;
	
	return 0;
	rewind(AP2);
	while (fread(&reg, sizeof(reg)*i, 1, AP2) >= 1)
	{
		if (reg.codigo == codigo)
		{
			return 1;
			break;
		}
		i++;		
	}	
}

/*DESCRIÇÃO: Cadastra informações de um cachorro no Arquivo Principal 2
  PARÂMETROS: AP2 - Arquivo Principal 2*/
void CadastraCachorro(FILE *AP2)
{
	registro reg;
	
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
	printf("Raca: ");
	gets(reg.raca);
	printf("Nome do Cachorro: ");
	gets(reg.nome);
	
	fseek(AP2, 0, SEEK_END); //seta a posição para o fim do arquivo.
	fwrite(&reg, sizeof(reg), 1, AP2); //escreve no fim do arquivo.
}

/*DESCRIÇÃO: Verifica se é necessário recriar um índice a partir de um arquivo
  PARÂMETRO: ind - Índice a ser verificado
  RETORNOS: 0 - Não é necessário recriar o índice
            1 - É necessário recriar o índice*/
int ExigeRecriaIndice(FILE *arq)
{
	char ch;
	
	fseek(arq, 0, 0);
	ch = fgetc(arq);
	if (ch == '!')
	  return 1;
	else
	  return 0;
}

/*DESCRIÇÃO: Atualiza o header do índice com o status de atualização
  PARÂMETROS: status - '*' - Índice atualizado
                       '!' - Índice desatualizado
			  arq - Índice a ser atualizado*/
void AtualizaInfoIndice(char status, FILE *arq)
{
	fseek(arq, 0, 0);
	fputc(status, arq);	
}















