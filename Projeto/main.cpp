#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Cria��o da estrutura para AP2
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

indice1 INDEX1[50];
indice2 INDEX2[50];
indice1 INDEX3[50];
int tam1 = 0, tam2 = 0, tam3 = 0; 

int Menu();
void AbreArquivos(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
int PerguntaCodigo(FILE **AP2);
void CadastraVacina(FILE **AP1, FILE **AP2);
void CadastraCachorro(FILE **AP2);
void AtualizaInfoIndice(char status, FILE **arq);
int ExisteCachorro(int codigo, FILE **AP2);
int ProcuraEspacoVazio(FILE **AP1, int tam_reg);
char PerguntaOpcao(int cod);
void AlteraDados(FILE **AP1, FILE **AP2, int cod);
void PegaCampo(FILE **AP2, char *campo);
void AlteraCachorro(FILE **AP2);
int ExigeRecriaIndice(FILE **arq);
void RecriaIndicePrim(FILE **AP1);
void QuickSortInd1(indice1 aux[], int left, int right);
int ParticionarQSInd1(indice1 aux[], int left, int right, int pivo);
void TrocaQSInd1(indice1 aux[], int i, int j);
void CarregaIndice(FILE **arq, int tipo);
//void EscreveIndices(FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);

int main() 
{
    int opcao;
	FILE *AP1, *AP2, *IndPrim, *IndSec1, *IndSec2;
	
	AbreArquivos(&AP1, &AP2, &IndPrim, &IndSec1, &IndSec2);
	opcao = Menu();
	while (1)
	{
	    switch(opcao)
	    {
	        case 1: CadastraCachorro(&AP2); break;
	        case 2: CadastraVacina(&AP1, &AP2); break;
			case 3: AlteraCachorro(&AP2);
                    break;
			case 4: AlteraDados(&AP1, &AP2, PerguntaCodigo(&AP2)); break;
	        case 0: printf("\nSaindo do Programa..."); 
        	        fclose(AP1); fclose(AP2); //fecha arquivos principais
                    fclose(IndPrim); fclose(IndSec1); fclose(IndSec2); //fecha �ndices
                    getch(); 
                    return 0; 
                    break;
	        default: printf("\nOpcao invalida!"); getch(); break;
        }
        opcao = Menu();
    }
    return 0;
}

/*
DESCRI��O: Exibe as op��es do menu.
RETORNO: O n�mero da op��o escolhida pelo usu�rio
*/
int Menu()
{
	int opcao;
	
	system("CLS");
    printf("\n 1 - Cadastra Cachorro");
    printf("\n 2 - Cadastra Vacina");
	printf("\n 3 - Altera Cachorro");
	printf("\n 4 - Alterar dados de Vacina");
	printf("\n 0 - Sair");
	printf("\n\nEscolha a opcao: ");
    scanf("%d", &opcao);
	
	return opcao;
}

/*
DESCRI��O: Verifica se os arquivos j� foram criados.
           Se n�o, cria-os.
PAR�METROS: AP1 - Arquivo Principal 1
            AP2 - Arquivo Principal 2
	        IndPrim - Arquivo de �ndice (busca por chave prim�ria)
			IndSec1 - Arquivo de �ndice 1 (busca por chave secund�ria)
			IndSec2 - Arquivo de �ndice 2 (busca por chave secund�ria)
*/
void AbreArquivos(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2)
{
    int header = -1;
    /*No primeiro caso, ele entra no if pois o arquivo ainda n�o existe 
    (Com o uso do r+b o arquivo tem que existir). 
    A� ent�o ele cria o arquivo com w+b*/
    if ((*AP1 = fopen("AP1.bin", "r+b")) == NULL) //se o arquivo n�o exisitr
    {
        *AP1 = fopen("AP1.bin", "w+b"); //cria um novo arquivo vazio (AP1)
    	fwrite(&header, sizeof(int), 1, *AP1);
    	*IndPrim = fopen("IndPrim.bin", "w+b");
    	AtualizaInfoIndice('!', IndPrim);
    	*IndSec1 = fopen("IndSec1.bin", "w+b");
    	AtualizaInfoIndice('!', IndSec1);
    	*IndSec2 = fopen("IndSec2.bin", "w+b");
    	AtualizaInfoIndice('!', IndSec2);
	}
	else 
	{  
	    *IndPrim = fopen("IndPrim.bin", "r+b");
    	*IndSec1 = fopen("IndSec1.bin", "r+b");
    	*IndSec2 = fopen("IndSec2.bin", "r+b");
    	if (ExigeRecriaIndice(IndPrim))
    	{
    	   RecriaIndicePrim(AP1);
		   QuickSortInd1(INDEX1, 0, tam1);   
        }
        else
        {
           CarregaIndice(IndPrim, 1);
    	   CarregaIndice(IndSec1, 2);
    	   CarregaIndice(IndSec2, 1);  
        }
	}
    	
    if ((*AP2 = fopen("AP2.bin", "r+b")) == NULL) //se o arquivo n�o existir
        *AP2 = fopen("AP2.bin", "w+b"); //cria um novo arquivo vazio (AP2)
}

/*
DESCRI��O: Cadastra informa��es de um cachorro no Arquivo Principal 2
PAR�METROS: AP2 - Arquivo Principal 2
*/
void CadastraCachorro(FILE **AP2)
{
	registro reg;
	
	system("CLS");
	printf(" Codigo: ");
	scanf("%d", &reg.codigo);
	while (ExisteCachorro(reg.codigo, AP2))
	{
		system("CLS");
		printf("\nCodigo ja cadastrado. Digite novamente!");
		getch(); system("CLS");
		printf(" Codigo: ");
        scanf("%d", &reg.codigo);
	}
	fflush(stdin);
	printf(" Raca: ");
	gets(reg.raca);
	printf(" Nome do Cachorro: ");
	gets(reg.nome);

	
	fseek(*AP2, 0, SEEK_END); //seta a posi��o para o fim do arquivo.
	fwrite(&reg, sizeof(reg), 1, *AP2); //escreve no fim do arquivo.
}

/*
DESCRI��O: Atualiza o header do �ndice com o status de atualiza��o
PAR�METROS: status - '*' - �ndice atualizado
                     '!' - �ndice desatualizado
            arq - �ndice a ser atualizado
*/
void AtualizaInfoIndice(char status, FILE **arq)
{
	fseek(*arq, 0, SEEK_SET);
	fputc(status, *arq);	
}

/*
DESCRI��O: Verifica se o c�digo j� existe no arquivo.
PAR�METROS: codigo - C�digo a ser verificado
            AP2 - Arquivo Principal 2
RETORNOS: 0 - N�o existe um cachorro com o c�digo passado por par�metro
          1 - Existe um cachorro com o c�digo passado por par�metro
*/
int ExisteCachorro(int codigo, FILE **AP2)
{
	registro reg;
	
	rewind(*AP2);
	while (fread(&reg, sizeof(registro), 1, *AP2))
	{
		if (reg.codigo == codigo)
		{
			return 1;
			break;
		}	
	}
    return 0;	
}

/*
DESCRI��O: Pergunta o c�digo do cachorro para posteriormente fazer uma opera��o.
PAR�METROS: AP - Arquivo Principal 1 ou 2
RETORNOS: C�digo do cachorro
*/
int PerguntaCodigo(FILE **AP2)
{
    int cod, aux = 0;
    system("CLS");
    printf("\n Digite o codigo do cachorro <-1 para cadastrar um cachorro>: ");
    scanf("%d", &cod);
    while (!ExisteCachorro(cod, AP2))
    {
        if (!aux)
            printf("\n Cachorro inexistente. Digite novamente!");
        else
        {
            printf("\n Nova busca..." );
            aux = 0;
        }
        getch(); system("CLS");  
        printf("\n Digite o codigo do cachorro <-1 para cadastrar um cachorro>: ");
        scanf("%d", &cod);
        if (cod == -1)
        {
            CadastraCachorro(AP2);
            aux = 1;
        }
    }
    return cod;
}

/*
DESCRI��O: Realiza o cadastro de vacinas
PAR�METROS: AP1 - Arquivo principal 1
            AP2 - Arquivo principal 2
*/
void CadastraVacina(FILE **AP1, FILE **AP2)
{
    int cod_controle = 0, cod_cachorro, tam_reg, posicao, aux = 0;
    char verificador = '*', vacina[30], data[6], respo[100], registro[255];
    
    cod_cachorro = PerguntaCodigo(AP2);
    system("CLS");
    cod_controle++; //cod_controle = pegar do INDEX1 ordenado;
    printf("\n Codigo do cachorro: %d", cod_cachorro);
    fflush(stdin);
    printf("\n\n Nome da vacina: ");
    gets(vacina);
    printf(" Data de vacinacao <MM/AA>: ");
    gets(data);
    //data[6] = '\0';
    printf(" Responsavel pela aplicacao: ");
    gets(respo);
    
    sprintf(registro, "%d|%d|%s|%s|%s|", cod_controle, cod_cachorro, vacina, data, respo);
    tam_reg = strlen(registro);
    posicao = ProcuraEspacoVazio(AP1, tam_reg);
    if (posicao != -1)
      fseek(*AP1, posicao, SEEK_SET);
    else
      fseek(*AP1, 0, SEEK_END);
    fwrite(&tam_reg, sizeof(int), 1, *AP1);
    fwrite(&verificador, sizeof(char), 1, *AP1);
    fwrite(registro, sizeof(char), tam_reg, *AP1);
}

/*
DESCRI��O: Retorna o espa�o vazio encontrado no arquivo
PAR�METROS: AP1 - Arquivo principal 1
            tam_reg - tamanho do registro a ser inserido
RETORNO: posi��o livre para ser escrita
*/
int ProcuraEspacoVazio(FILE **AP1, int tam_reg)
{
    int offset, tam, pos;
    char ch;
    
    rewind(*AP1);
    fread(&offset, sizeof(int), 1, *AP1);
    if (offset == -1)
        return -1;
    else
    {
        fseek(*AP1, offset, SEEK_SET);
        pos = ftell(*AP1);
        while (fread(&tam, sizeof(int), 1, *AP1))
        {
            if (tam == -1)
            {
                return -1;
                break;
            }
            else
            {
                ch = fgetc(*AP1);
                if ((tam > tam_reg) && (ch == '!')) //se achou um registro vazio
                {
                   return pos;
                   break;                 
                }
                else
                {
                    fread(&offset, sizeof(int), 1, *AP1);
                    fseek(*AP1, offset, SEEK_SET);
                }   
            }     
        }   
    }
}

/*
DESCRI��O: Pergunta o campo do qual ser� alterado.
PAR�METROS: 
*/
char PerguntaOpcao(int cod)
{
    char opcao;
    printf("\n Cdigo do cachorro: %d.", cod);
    printf("\n\n CAMPOS:\n a - Nome da Vacina");
    printf("\n b - Data da Vacinacao");
    printf("\n c - Responsavel Pela Aplicacao");
    printf("\n\n  Digite o campo a ser alterado: ");
    opcao = getche();
    return opcao;
}

/*
DESCRI��O: Altera os dados de uma cadastro de vacina j� exitente
PAR�METROS: AP1 - Arquivo Principal 1
            AP2 - Arquivo Principal 2 (serve para a busca do c�digo, apenas)
*/
void AlteraDados(FILE **AP1, FILE **AP2, int cod)
{
    rewind(*AP1);
    int cont = 0, i = 0, converteu[9999];
    char opcao, *offset, *cod_ca, *vacina, *data, *respo;
    
    cod = PerguntaCodigo(AP2); //encontra o c�digo do cachorro em que os dados da vacina ser�o alterados
    system("CLS");
    opcao = PerguntaOpcao(cod); //pergunta qual campo vai alterar
    opcao = tolower(opcao);
    //preciso encontrar o codigo do cachorro em AP1
    
    while(1) //L�gica para parar AP1 na posi��o do registro a ser alterado
    {
        PegaCampo(AP1, offset);
        cont = 0;
        while (offset[cont] != '!' && offset[cont] != '*')
            cont++;
        offset[cont] = '\0';
        converteu[i] = atoi(offset);
        offset[0] = '\0';
        if (converteu[i] == -1 || converteu[i] == cod)
            break;
        PegaCampo(AP1, cod_ca);
        PegaCampo(AP1, vacina);
        PegaCampo(AP1, data);
        PegaCampo(AP1, respo);
        i++;
        
        //zerando as vari�veis para uso posterior
        cod_ca[0] = '\0';
        vacina[0] = '\0';
        data[0] = '\0';
        respo[0] = '\0';
    }
    
    switch (opcao)
    {
        case 'a': break;
        case 'b': break;
        case 'c': break;
        default: system("CLS");
                 printf("\n Opcao Ivalida! Digite novamente...\n"); 
                 PerguntaOpcao(cod); break;
    }    
}

void PegaCampo(FILE **AP1, char *campo)
{
    char ch;
    int i = 0;
    campo[i] = '\0';
    
    while (fread(&ch,sizeof(char),1,*AP1))
    {
       if (ch == '|')
              break;
       else
           campo[i] = ch;
       i++;
    }
    
    campo[i] = '\0';
}

/*
DESCRI��O: Altera dados de um cachorro
PAR�METRO: AP2 - arquivo principal 2
*/
void AlteraCachorro(FILE **AP2)
{
    int op, cod, i = 0;
    char raca[30], nome[100];
    registro reg;
    
    system("CLS");
    printf("Digite o codigo do cachorro: ");
    scanf("%d", &cod);
    while (!ExisteCachorro(cod, AP2))
    {
        system("CLS");   
        printf("\n\nCachorro inexistente. Digite novamente!");
        getch();
        system("CLS");
        printf("\n\nDigite o codigo do cachorro: ");
        scanf("%d", &cod);   
    }
    
    rewind(*AP2);
	while (fread(&reg, sizeof(reg), 1, *AP2))
	{
		if (reg.codigo == cod)
		  break;
		i++;
	}
	
    system("CLS");
    printf("Qual campo deseja alterar");
    printf("\n\n1 - Raca");
    printf("\n2 - Nome");
    printf("\n\nEscolha a opcao: ");
    scanf("%d", &op);
    while ((op != 1) && (op != 2))
    {
       system("CLS");
       printf("Opcao invalida. Digite novamente!");
       getch();
       system("CLS");
       printf("Qual campo deseja alterar");
       printf("\n\n1 - Raca");
       printf("\n2 - Nome");
       printf("\n\nEscolha a opcao: ");
       scanf("%d", op); 
    }
    
    fflush(stdin);
    system("CLS");
    switch(op)
    {
        case 1: printf("Digite a nova raca: ");
                gets(raca);
                strcpy(reg.raca, raca);
                break;
        case 2: printf("Digite o novo nome: ");
                gets(nome);
                strcpy(reg.nome, nome);
                break;
    }
    
    fseek(*AP2, sizeof(reg)*i, SEEK_SET);
	fwrite(&reg, sizeof(reg), 1, *AP2);
}

/*
DESCRI��O: Verifica se � necess�rio recriar um �ndice a partir de um arquivo
PAR�METRO: ind - �ndice a ser verificado
RETORNOS: 0 - N�o � necess�rio recriar o �ndice
          1 - � necess�rio recriar o �ndice
*/
int ExigeRecriaIndice(FILE **arq)
{
	char ch;
	
	rewind(*arq);
	ch = fgetc(*arq);
	if (ch == '!')
	  return 1;
	else
	  return 0;
}

/*
DESCRI��O: Recria o �ndice prim�rio a partir do arquivo e carrega em mem�ria principal
PAR�METRO: AP1 - Arquivo Principal 1
*/
void RecriaIndicePrim(FILE **AP1)
{
	int deslocamento, aux;
	char ch;
	
	fseek(*AP1, sizeof(int), SEEK_SET); //header
	aux = ftell(*AP1);
	ch = fgetc(*AP1);
	if (ch == EOF)
		return;
	else
	{
		fseek(*AP1, 2*sizeof(int), SEEK_SET); //primeiro registro
		ch = fgetc(*AP1);
		while (ch != EOF)
		{
			if (ch != '!')
			{
				INDEX1[tam1].offset = ftell(*AP1);
				fseek(*AP1, INDEX1[tam1].offset, SEEK_SET);
				fscanf(*AP1, "%d", INDEX1[tam1].codigo);
				tam1++;
			}
			fseek(*AP1, 0, aux);
			fread(&deslocamento, sizeof(int), 1, *AP1);
			//fscanf(*AP1, "%d", deslocamento);
			aux += deslocamento;
			fseek(*AP1, sizeof(int), aux);
			ch = fgetc(*AP1);
		}	
	}
}

//QuickSort
void TrocaQSInd1(indice1 aux[], int i, int j) 
{
    indice1 t = aux[i];
    aux[i] = aux[j];
    aux[j] = t;
}
 
int ParticionarQSInd1(indice1 aux[], int left,int right,int pivo)
{
    int pos, i;
    TrocaQSInd1(aux, pivo, right);
    pos = left;
    for(i = left; i < right; i++)
    {
        if (aux[i].codigo < aux[right].codigo)
        {
            TrocaQSInd1(aux, i, pos);
            pos++;
        }
    }
    TrocaQSInd1(aux, right, pos);
    return pos;
}
 
void QuickSortInd1(indice1 aux[], int left, int right)
{
	int pivo, pos;
	
    if (left < right)
    {
        pivo = (left + right) / 2;
        pos = ParticionarQSInd1(aux,left,right,pivo);
        QuickSortInd1(aux, left, pos - 1);
        QuickSortInd1(aux, pos + 1, right);
    }
}

void CarregaIndice(FILE **arq, int tipo)
{
	indice1 ind1;
	indice2 ind2;
	
	if (tipo != 2)
	{
		rewind(*arq);
		while (fgetc(*arq) != EOF)
		{
			fread(&ind1, sizeof(indice1), 1, *arq);
			if (tipo == 1)
			{
				INDEX1[tam1] = ind1;
				tam1++;	
			}
			else
			{
				INDEX3[tam3] = ind1;
				tam3++;
			}
			
		}
	}
}

