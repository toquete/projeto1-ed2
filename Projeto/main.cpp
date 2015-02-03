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
int tam1 = 0, tam2 = 0;

int Menu();
void AbreArquivos(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
int PerguntaCodigo(FILE **AP2);
void CadastraVacina(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
void CadastraCachorro(FILE **AP2);
void AtualizaInfoIndice(char status, FILE **arq);
int ExisteCachorro(int codigo, FILE **AP2);
int ProcuraEspacoVazio(FILE **AP1, int tam_reg);
char PerguntaOpcao();
void AlteraDados(FILE **AP1, FILE **AP2);
void PegaCampo(FILE **AP2, char *campo, int pos);
void AlteraCachorro(FILE **AP2);
int ExigeRecriaIndice(FILE **arq);
void RecriaIndicePrim(FILE **AP1);
void QuickSortInd1(indice1 aux[], int left, int right);
int ParticionarQSInd1(indice1 aux[], int left, int right, int pivo);
void TrocaQSInd1(indice1 aux[], int i, int j);
void QuickSortInd2(indice2 aux[], int left, int right);
int ParticionarQSInd2(indice2 aux[], int left, int right, int pivo);
void TrocaQSInd2(indice2 aux[], int i, int j);
void CarregaIndice(FILE **arq, int tipo);
void GravaIndices(FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
void RemoveVacina(FILE **AP1, int pos, int cod_controle);
int RetornaPosicao(int codigo);
void MenuRemoveVacina(FILE **AP1);
void PesquisaCodPrim(FILE **AP1, FILE **AP2);
void AtualizaListaEspacosVazios(FILE **AP1, int pos);
void Compacta (FILE **AP1);
void InsereIndiceSecundario(FILE **IndSec2, int cod_controle, char *vacina);
void RecriaIndiceSec(FILE **AP1, FILE **IndSec2);
int RetornaOffset(char *vacina);
void PesquisaVacinaSec(FILE **AP1, FILE **AP2, FILE **IndSec2);

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
	        case 1: CadastraCachorro(&AP2); 
                    break;
	        case 2: CadastraVacina(&AP1, &AP2, &IndPrim, &IndSec1, &IndSec2); 
                    break;
			case 3: AlteraCachorro(&AP2);
                    break;
			case 4: AlteraDados(&AP1, &IndPrim);
                    break;
            case 5: MenuRemoveVacina(&AP1);
                    AtualizaInfoIndice('!', &IndPrim);
                    break;
            case 6: PesquisaCodPrim(&AP1, &AP2);
                    break;
            case 7: PesquisaVacinaSec(&AP1, &AP2, &IndSec2);
                    break;
            case 8: Compacta(&AP1);
                    break;
	        case 0: printf("\nSaindo do Programa...");
	                if ((ExigeRecriaIndice(&IndPrim)) || (ExigeRecriaIndice(&IndSec1)))
                      GravaIndices(&IndPrim, &IndSec1, &IndSec2);
					//fecha arquivos principais  
        	        fclose(AP1); 
					fclose(AP2);
					//fecha �ndices 
                    fclose(IndPrim);
					fclose(IndSec1);
					fclose(IndSec2);
                    getch(); 
                    return 0; 
                    break;
	        default: printf("\nOpcao invalida!"); 
                     getch();
                     break;
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
    printf("***********************************\n");
    printf("*               MENU              *\n");
    printf("***********************************\n");
    printf("\n 1 - Cadastra Cachorro");
    printf("\n 2 - Cadastra Vacina");
	printf("\n 3 - Altera Cachorro");
	printf("\n 4 - Alterar dados de Vacina");
	printf("\n 5 - Remove Vacina");
	printf("\n 6 - Pesquisa por Codigo de Controle");
	printf("\n 7 - Pesquisa por Nome da Vacina");
	printf("\n 8 - Compactar o arquivo");
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
    {   //abre pela primeira vez (cria)
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
           RecriaIndiceSec(AP1, IndSec2);
           QuickSortInd2(INDEX2, 0, tam2);   
        }
        else
        {
           CarregaIndice(IndPrim, 1);
    	   CarregaIndice(IndSec1, 2); 
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
	fwrite(&reg, sizeof(registro), 1, *AP2); //escreve no fim do arquivo.
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
	
	fseek(*AP2, 0, SEEK_SET);
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
PAR�METROS: AP - Arquivo Principal 2
RETORNOS: C�digo do cachorro
*/
int PerguntaCodigo(FILE **AP2)
{
    int cod, aux = 0;
    system("CLS");
    printf("\n Digite o codigo do cachorro <-1 para cadastrar um cachorro>: ");
    scanf("%d", &cod);
    if(cod == -1)
    {   
        CadastraCachorro(AP2);
        aux = 1;
    }
    while (!ExisteCachorro(cod, AP2))
    {
        if (!aux)
            printf("\n Cachorro inexistente. Digite novamente!");
        else
        {
            printf("\n Nova busca..." );
            aux = 0;
        }
        getch(); 
        system("CLS");  
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
void CadastraVacina(FILE **AP1, FILE **AP2, FILE **IndPrim, FILE **IndSec1, FILE **IndSec2)
{
    int cod_controle, cod_cachorro, tam_reg, posicao, aux, header;
    char verificador = '*', vacina[30], data[6], respo[100], registro[255];
    
    cod_cachorro = PerguntaCodigo(AP2);
    system("CLS");
    cod_controle = INDEX1[tam1 - 1].codigo + 1; //pegar do INDEX1 ordenado;
    printf("\n Codigo de controle: %d", cod_controle);
    fflush(stdin);
    printf("\n Codigo do cachorro: %d", cod_cachorro);
    fflush(stdin);
    printf("\n Nome da vacina: ");
    gets(vacina);
    printf(" Data de vacinacao <MM/AA>: ");
    gets(data);
    printf(" Responsavel pela aplicacao: ");
    gets(respo);
    
    sprintf(registro, "%d|%d|%s|%s|%s|", cod_controle, cod_cachorro, vacina, data, respo);
    tam_reg = strlen(registro);
    posicao = ProcuraEspacoVazio(AP1, tam_reg);
    if (posicao != -1)
      fseek(*AP1, posicao, SEEK_SET);
    else
    {
      fseek(*AP1, 0, SEEK_END);
      posicao = -1;  
    }
      
    INDEX1[tam1].codigo = cod_controle;
    INDEX1[tam1].offset = ftell(*AP1);
    QuickSortInd1(INDEX1, 0, tam1);
    tam1++;
    AtualizaInfoIndice('!', IndPrim);
    
    if (posicao != -1)
    {
      AtualizaListaEspacosVazios(AP1, posicao);
      fseek(*AP1, posicao, SEEK_SET);  
    }
    
    InsereIndiceSecundario(IndSec2, cod_controle, vacina);
          
    fwrite(&tam_reg, sizeof(int), 1, *AP1);
    fputc(verificador, *AP1);
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
    
    fseek(*AP1, 0, SEEK_SET);
    fread(&offset, sizeof(int), 1, *AP1);
    if (offset == -1)
        return -1;
    else
    {
        fseek(*AP1, offset, SEEK_SET);
        pos = ftell(*AP1);
        while (fread(&tam, sizeof(int), 1, *AP1))
        {
            if (tam > tam_reg)
              return pos;
            fseek(*AP1, sizeof(char), SEEK_CUR); //pula char verificador
            fread(&offset, sizeof(int), 1, *AP1);
            if (offset == -1)
              return -1;
            fseek(*AP1, offset, SEEK_SET);     
        }   
    }
}

/*
DESCRI��O: Pergunta o campo do qual ser� alterado.
PAR�METROS: 
*/
char PerguntaOpcao()
{
    printf("\n\n CAMPOS:\n a - Codigo do Cachorro");
    printf("\n b - Nome da Vacina");
    printf("\n c - Data da Vacinacao");
    printf("\n d - Responsavel Pela Aplicacao");
    printf("\n\n Digite o campo a ser alterado: ");
    return getche();
}

/*
DESCRI��O: Altera os dados de uma cadastro de vacina j� exitente
PAR�METROS: AP1 - Arquivo Principal 1
            IndPrim - �ndice prim�rio (para ser atualizado caso haja exclus�o)
*/
void AlteraDados(FILE **AP1, FILE **IndPrim)
{
    char opcao, registro[255], *CodCo, *CodCa, *vacina, *data, *respo;
    char verificador = '*', NCodCo[5], NCodCa[5], Nvacina[100], Ndata[6], Nrespo[150];
    int cod, pos, tam_reg, Ntam_reg;
    
    system("CLS");
    fseek(*AP1, 0, SEEK_SET);
    printf("Codigo da vacina a ser alterada: ");
    scanf("%d", &cod);
    pos = RetornaPosicao(cod);
    if (pos == -1)
    {
        printf("\n Codigo da vacina nao encontrado!");
        getch();
        system("cls");
        return;
    }
    fseek(*AP1, pos, SEEK_SET);
     
    fread(&tam_reg, sizeof(int), 1, *AP1);
    fseek(*AP1, sizeof(char), SEEK_CUR);
    fread(&registro, sizeof(char), tam_reg + 1, *AP1);
    
    CodCo = strtok(registro, "|");
    CodCa = strtok(NULL, "|");
    vacina = strtok(NULL, "|");
    data = strtok(NULL, "|");
    respo = strtok(NULL, "|");
    
    strcpy(NCodCo, CodCo);
    strcpy(NCodCa, CodCa);
    strcpy(Nvacina, vacina);
    strcpy(Ndata, data);
    strcpy(Nrespo, respo);
    
    opcao = PerguntaOpcao();
    switch (opcao)
    {
        case 'a': printf("\n\n Codigo do cachorro: %s", CodCa);
                  fflush(stdin);
                  printf("\n\n Digite o novo codigo do cachorro: ");
                  gets(NCodCa);
                  break;
        case 'b': printf("\n\n Nome da vacina: %s", vacina);
                  fflush(stdin);
                  printf("\n\n Digite o novo nome da vacina: ");
                  fflush(stdin);
                  gets(Nvacina);
                  break;
        case 'c': printf("\n\n Data (MM/AA): %s", data);
                  fflush(stdin);
                  printf("\n\n Digite a nova data (MM/AA): ");
                  gets(Ndata);
                  break;
        case 'd': printf("\n\n Responsavel pela aplicacao: %s", respo);
                  fflush(stdin);
                  printf("\n\n Digite o nome do novo responsavel pela aplicacao: ");
                  gets(Nrespo);
                  break;
         default: system("CLS");
                  printf("\n Opcao ivalida! Digite novamente...\n"); 
                  PerguntaOpcao();
                  break;
    }
    
    sprintf(registro, "%s|%s|%s|%s|%s|", NCodCo, NCodCa, Nvacina, Ndata, Nrespo);
    Ntam_reg = strlen(registro);
    if (Ntam_reg > tam_reg)
    {
        RemoveVacina(AP1, pos, atoi(CodCo));
        pos = ProcuraEspacoVazio(AP1, tam_reg);
        if (pos != -1)
          fseek(*AP1, pos, SEEK_SET);
        else
          fseek(*AP1, 0, SEEK_END);
          
        INDEX1[tam1].codigo = atoi(CodCo);
        INDEX1[tam1].offset = ftell(*AP1);
        QuickSortInd1(INDEX1, 0, tam1);
        tam1++;
        AtualizaInfoIndice('!', IndPrim);
        fwrite(&Ntam_reg, sizeof(int), 1, *AP1);
        fputc(verificador, *AP1);
        fwrite(registro, sizeof(char), Ntam_reg, *AP1);     
    }
    else
    {
        fseek(*AP1, pos, SEEK_SET);
        fwrite(&Ntam_reg, sizeof(int), 1, *AP1);
        fputc(verificador, *AP1);
        fwrite(registro, sizeof(char), Ntam_reg, *AP1);    
    }  
}

/*
DESCRI��O: Pega o campo em AP1
PAR�METROs: AP1 - arquivo principal 1
            Campo de retorno
            Posi��o inicial do campo
*/
void PegaCampo(FILE **AP1, char *campo, int pos)
{
    char ch;
    int i = 0, cont = 0;
    campo[i] = '\0';
    
    while (fread(&ch,sizeof(char),1,*AP1))
    {
        if (ch == '|' || ch == '*')
            break;
        else
        {
            cont++;
            campo[i] = ch;
        }
        
        if (cont == 1)
            pos = ftell(*AP1);
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
    int op, cod, i = 0, aux = 0;
    char raca[30], nome[100];
    registro reg;
    
    system("CLS");
    cod = PerguntaCodigo(AP2);

    fseek(*AP2, 0, SEEK_SET);
	while (fread(&reg, sizeof(reg), 1, *AP2))
	{
		if (reg.codigo == cod)
		  break;
		i++;
	}
	
    system("CLS");
    printf("Qual campo deseja alterar");
    printf("\n\n 1 - Raca");
    printf("\n 2 - Nome");
    printf("\n\n Escolha a opcao: ");
    scanf("%d", &op);
    while ((op != 1) && (op != 2))
    {
       system("CLS");
       printf("Opcao invalida! Digite novamente...");
       getch();
       system("CLS");
       printf("Qual campo deseja alterar");
       printf("\n\n 1 - Raca");
       printf("\n 2 - Nome");
       printf("\n\n Escolha a opcao: ");
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
	
	fseek(*arq, 0, SEEK_SET);
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
				fread(&INDEX1[tam1].codigo, sizeof(int), 1, *AP1);
				tam1++;
			}
			fseek(*AP1, 0, aux);
			fread(&deslocamento, sizeof(int), 1, *AP1);
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

void TrocaQSInd2(indice2 aux[], int i, int j) 
{
    indice2 t = aux[i];
    aux[i] = aux[j];
    aux[j] = t;
}
 
int ParticionarQSInd2(indice2 aux[], int left,int right,int pivo)
{
    int pos, i;
    TrocaQSInd2(aux, pivo, right);
    pos = left;
    for(i = left; i < right; i++)
    {
        if (strcmp(aux[i].vacina, aux[right].vacina) < 0)// aux[i].codigo < aux[right].codigo)
        {
            TrocaQSInd2(aux, i, pos);
            pos++;
        }
    }
    TrocaQSInd2(aux, right, pos);
    return pos;
}
 
void QuickSortInd2(indice2 aux[], int left, int right)
{
	int pivo, pos;
	
    if (left < right)
    {
        pivo = (left + right) / 2;
        pos = ParticionarQSInd2(aux,left,right,pivo);
        QuickSortInd2(aux, left, pos - 1);
        QuickSortInd2(aux, pos + 1, right);
    }
}

void CarregaIndice(FILE **arq, int tipo)
{
	indice1 ind1;
	indice2 ind2;
	
	fseek(*arq, sizeof(char), SEEK_SET);
	if (tipo == 1)
	{
		while (fread(&ind1, sizeof(indice1), 1, *arq))
		{
    		INDEX1[tam1] = ind1;
    		tam1++;
		}
	}
	else
	{
	    while (fread(&ind2, sizeof(indice2), 1, *arq))
		{
    		INDEX2[tam2] = ind2;
    		tam2++;
		}   
    }
}

void GravaIndices(FILE **IndPrim, FILE **IndSec1, FILE **IndSec2)
{
    fclose(*IndPrim);
    remove("IndPrim.bin");
    *IndPrim = fopen("IndPrim.bin", "w+b");
    fseek(*IndPrim, 0, SEEK_SET);
    AtualizaInfoIndice('*', IndPrim);
    for(int i = 0; i < tam1; i++)
      fwrite(&INDEX1[i], sizeof(INDEX1[i]), 1, *IndPrim);
    fputc(EOF, *IndPrim);
      
    fclose(*IndSec1);
    remove("IndSec1.bin");
    *IndSec1 = fopen("IndSec1.bin", "w+b");
    fseek(*IndSec1, 0, SEEK_SET);
    AtualizaInfoIndice('*', IndSec1);
    for(int i = 0; i < tam2; i++)
      fwrite(&INDEX2[i], sizeof(INDEX2[i]), 1, *IndSec1);
    fputc(EOF, *IndSec1);
      
    /*fclose(*IndSec2);
    remove("IndSec2.bin");
    *IndSec2 = fopen("IndSec2.bin", "r+b");
    fseek(*IndSec2, 0, SEEK_SET);
    AtualizaInfoIndice('*', IndSec2);
    for(int i = 0; i < tam2; i++)
      fwrite(&INDEX2[i], sizeof(INDEX2[i]), 1, *IndSec1);*/
      
}

void MenuRemoveVacina(FILE **AP1)
{
    int cod_controle, pos;
    
    system("CLS");
    printf(" Digite o codigo da vacina a ser removida: ");
    scanf("%d", &cod_controle);
    
    pos = RetornaPosicao(cod_controle);
    RemoveVacina(AP1, pos, cod_controle);
    printf(" \nVacina Removida com sucesso!");
    getch();
}

void RemoveVacina(FILE **AP1, int pos, int cod_controle)
{
    int header;
    char verificador = '!';
    
    fseek(*AP1, 0, SEEK_SET);
    fread(&header, sizeof(int), 1, *AP1);//pegou o header atual
    fseek(*AP1, pos + sizeof(int), SEEK_SET);//foi para o registro que ser� removido
    fwrite(&verificador, sizeof(char), 1, *AP1);
    //subescreveu para indicar que o registro est� inativo '!'
    fwrite(&header, sizeof(int), 1, *AP1);//escreve a pr�xima posi��o dispon�vel
    //se '-1', n�o h� registros livres (insere no final) 
    fseek(*AP1, 0, SEEK_SET);
    fwrite(&pos, sizeof(int), 1, *AP1);
    //escreve no in�cio do arquivo a posi��o do registro que foi removido
    
    for(int i = 0; i< tam1; i++)
    {
        if (cod_controle == INDEX1[i].codigo)
        {
            INDEX1[i] = INDEX1[tam1 - 1];
            tam1--;
            break;
        }
    }
    QuickSortInd1(INDEX1, 0, tam1);
}

int RetornaPosicao(int codigo)
{
    for(int i = 0; i< tam1; i++)
      if(codigo == INDEX1[i].codigo)
        return INDEX1[i].offset;
    return -1;
}

void PesquisaCodPrim(FILE **AP1, FILE **AP2)
{
    int cod_controle, cod_cachorro, offset, tam_reg;
    char temp[255], *campo;
    registro reg;
    
    system("CLS");
    printf(" Digite o codigo de controle de vacina: ");
    scanf("%d", &cod_controle);
    
    offset = RetornaPosicao(cod_controle);
    fseek(*AP1, offset, SEEK_SET);
    fread(&tam_reg, sizeof(int), 1, *AP1);
    fseek(*AP1, sizeof(char), SEEK_CUR);
    fread(temp, sizeof(char), tam_reg+1, *AP1);
    
    campo = strtok(temp, "|"); //c�digo do controle
    campo = strtok(NULL, "|"); //c�digo do cachorro
    cod_cachorro = atoi(campo);    
    fseek(*AP2, 0, SEEK_SET);
	while (fread(&reg, sizeof(registro), 1, *AP2))
	{
		if (reg.codigo == cod_cachorro)
			break;
    }
    
    system("CLS");
    printf("INFORMACOES DO CACHORRO");
    printf("\n\n Codigo: %d", cod_cachorro);
    printf("\n\n Raca: ");
    puts(reg.raca);
    printf("\n Nome: ");
    puts(reg.nome);
    
    printf("\n\nINFORMACOES DA VACINA");
    printf("\n\n Codigo de Controle: %d", cod_controle);
    printf("\n\n Codigo do Cachorro: %d", cod_cachorro);
    campo = strtok(NULL, "|");
    printf("\n\n Nome da Vacina: ");
    puts(campo);
    campo = strtok(NULL, "|");
    printf("\n Data de Vacinacao: ");
    puts(campo);
    campo = strtok(NULL, "|");
    printf("\n Responsavel pela Aplicacao: ");
    puts(campo);
    getch();   
}

void AtualizaListaEspacosVazios(FILE **AP1, int pos)
{
    int i = 0, aux, offset, posicao;
    
    fseek(*AP1, 0, SEEK_SET);
    fread(&offset, sizeof(int), 1, *AP1);
    fseek(*AP1, offset + sizeof(int) + sizeof(char), SEEK_SET);
    posicao = ftell(*AP1);
    while (fread(&offset, sizeof(int), 1, *AP1))
    {
        if ((offset == -1) && (i == 0))
        {
            fseek(*AP1, 0, SEEK_SET);
            fwrite(&offset, sizeof(int), 1, *AP1);
            return;
        }
        else if (offset == pos)
        {
            fseek(*AP1, offset + sizeof(int) + sizeof(char), SEEK_SET);
            fread(&aux, sizeof(int), 1, *AP1);
            break;      
        }
        fseek(*AP1, offset + sizeof(int) + sizeof(char), SEEK_SET);
        posicao = ftell(*AP1);
        i++;
    }
    fseek(*AP1, posicao, SEEK_SET);
    fwrite(&aux, sizeof(int), 1, *AP1);
}

void Compacta (FILE **AP1)
{
    char *resto, status, *cod_controle;
    //resto = resto do arquivo para ser deslocado
    ////status = status('!' ou '*') do pr�ximo registro, ou seja, registro depois do dispon�vel
    ////cod_controle = c�digo de controle do pr�ximo registro
    int cont = 0, cont_aux = 0, tam_reg, dispo, pos, pos_, cc; 
    //cont = contador para verificar a primeira entrada no la�o
    ////cont_aux = contador para alterar os �ndices certos
    //tam_reg = tamanho do registro livre (anteriormente removido)
    //dispo = header (v� se tem um registro dispon�vel)
    //pos = posi��o do registro dispon�vel
    ////pos_ = posi��o do proximo elemento (depois do registro dispon�vel)
    ////cc = cod_controle convertido em int
    system("CLS");
    printf("\n Compactando...");
    fseek(*AP1, 0, SEEK_SET);
    fread(&dispo, sizeof(int), 1, *AP1);
    do
    {
        //dispo recebeu header para come�ar a lista de registros dispon�veis
        cont++; //contador para verificar a primeira entrada no la�o
        if(dispo == -1 && cont == 1)
        {//se dispo == '-1' na primeira vez
            printf("\n Arquivo ja compactado!");
            getch();
            return;
        }
        fseek(*AP1, dispo, SEEK_SET);//vai para o registro removido
        pos = ftell(*AP1);//pega a posi��o inicial do registro que est� indispon�vel
        fread(&tam_reg, sizeof(int), 1, *AP1);//pega o tamanho do registrador
        fseek(*AP1, sizeof(char), SEEK_CUR);//pula o status, que � '!' 
        fread(&dispo, sizeof(int), 1, *AP1);//pega o pr�ximo registro dispon�vel
        /*fseek(*AP1, tam_reg, pos);
        do //l�gica para alterar �ndices
        {
            fseek(*AP1, sizeof(char), SEEK_CUR);
            pos_ = ftell(*AP1);
            fseek(*AP1, sizeof(int), SEEK_CUR);
            fread(&status, sizeof(char), 1, *AP1);//verifica se o registro est� sendo usado
            if(status == '*')//se o registro est� em uso
            {//atualiza os �ndices
                PegaCampo(AP1, cod_controle, pos_);
                cc = atoi(cod_controle);
                cont_aux = 0;
                for(int i = 0; i<tam1; i++)
                {
                    if (cc == INDEX1[i].codigo)
                        cont_aux++;
                    if (cont_aux>0)    
                        INDEX1[i].offset = INDEX1[i].offset - tam_reg;
                }
            }
        }while(status != '!');*/
        
        fseek(*AP1, tam_reg, pos);
        //ERRO AQUI
        fread(resto, sizeof(char), 999, *AP1);//pegou o resto do arquivo
        fseek(*AP1, 0, pos);
        fwrite(resto, sizeof(char), strlen(resto), *AP1);
        AtualizaListaEspacosVazios(AP1, pos);
        RecriaIndicePrim(AP1);
        
        /*//atualiza o header
        fseek(*AP1, 0, SEEK_SET);
        fwrite(dispo, sizeof(int), 1, *AP1);
        
        //fseek(*AP1, 0, SEEK_SET);
        //fread(&dispo, sizeof(int), 1, *AP1);*/
    }while(dispo != -1);
    printf("\n Arquivo compactado com sucesso!");
    getch();
    fseek(*AP1, 0, SEEK_SET);
}

void InsereIndiceSecundario(FILE **IndSec2, int cod_controle, char *vacina)
{
    int offset, final = -1;
    
    for (int i = 0; i < tam2; i++)
    {
        if (strcmp(INDEX2[i].vacina, vacina) == 0)
        {
            fseek(*IndSec2, 0, SEEK_END);
            offset = ftell(*IndSec2);
            fwrite(&cod_controle, sizeof(int), 1, *IndSec2);
            fwrite(&INDEX2[i].offset, sizeof(int), 1, *IndSec2);
            INDEX2[i].offset = offset;
            return;
        }
    }
    
    fseek(*IndSec2, 0, SEEK_END);
    offset = ftell(*IndSec2);
    fwrite(&cod_controle, sizeof(int), 1, *IndSec2);
    fwrite(&final, sizeof(int), 1, *IndSec2);
    INDEX2[tam2].offset = offset;
    strcpy(INDEX2[tam2].vacina, vacina);
    tam2++;
    //QuickSortInd2(INDEX2, 0, tam2);
}

void RecriaIndiceSec(FILE **AP1, FILE **IndSec2)
{
    int deslocamento, aux, cod_controle, tam_reg;
	char ch, vacina[30], buffer[255], *campo;
	
	fseek(*AP1, sizeof(int), SEEK_SET); //header
	aux = ftell(*AP1);
	ch = fgetc(*AP1);
	if (ch == EOF)
		return;
	else
	{
		fseek(*AP1, sizeof(int), SEEK_SET); //primeiro registro
		fread(&tam_reg, sizeof(int), 1, *AP1);
		ch = fgetc(*AP1);
		while (ch != EOF)
		{
			if (ch != '!')
			{
			    fread(buffer, sizeof(char), tam_reg+1, *AP1);
			    campo = strtok(buffer, "|");
			    cod_controle = atoi(campo);
			    campo = strtok(NULL, "|");
			    campo = strtok(NULL, "|");
			    strcpy(vacina, campo);
			    InsereIndiceSecundario(IndSec2, cod_controle, vacina);
			}
			fseek(*AP1, 0, aux);
			fread(&deslocamento, sizeof(int), 1, *AP1);
			aux += deslocamento;
			fseek(*AP1, sizeof(int), aux);
			ch = fgetc(*AP1);
		}	
	}
}

void PesquisaVacinaSec(FILE **AP1, FILE **AP2, FILE **IndSec2)
{
    int cod_controle, cod_cachorro, offset, offset_AP, tam_reg;
    char vacina[30], temp[255], *campo;
    registro reg;
    indice1 ind;
    
    system("CLS");
    fflush(stdin);
    printf(" Digite o nome da vacina: ");
    gets(vacina);
    
    offset = RetornaOffset(vacina);
    while (offset == -1)
    {
        printf("\n\n Vacina inxistente! Digite novamente.");
        getch();
        system("CLS");
        printf(" Digite o nome da vacina: ");
        gets(vacina);
        offset = RetornaOffset(vacina);
    }
    system("CLS");
    fseek(*IndSec2, offset, SEEK_SET);
    while (fread(&ind, sizeof(indice1), 1, *IndSec2))
    {
        offset_AP = RetornaPosicao(ind.codigo);
        fseek(*AP1, offset_AP, SEEK_SET);
        fread(&tam_reg, sizeof(int), 1, *AP1);
        fseek(*AP1, sizeof(char), SEEK_CUR);
        fread(temp, sizeof(char), tam_reg+1, *AP1);
        campo = strtok(temp, "|"); //c�digo do controle
        cod_controle = atoi(campo);
        campo = strtok(NULL, "|"); //c�digo do cachorro
        cod_cachorro = atoi(campo);    
        fseek(*AP2, 0, SEEK_SET);
        while (fread(&reg, sizeof(registro), 1, *AP2))
        {
        	if (reg.codigo == cod_cachorro)
        		break;
        }
        
        printf("--------------------------------------------\n");
        printf("INFORMACOES DO CACHORRO");
        printf("\n\n Codigo: %d", cod_cachorro);
        printf("\n\n Raca: ");
        puts(reg.raca);
        printf("\n Nome: ");
        puts(reg.nome);
        
        printf("\n\nINFORMACOES DA VACINA");
        printf("\n\n Codigo de Controle: %d", cod_controle);
        printf("\n\n Codigo do Cachorro: %d", cod_cachorro);
        campo = strtok(NULL, "|");
        printf("\n\n Nome da Vacina: ");
        puts(campo);
        campo = strtok(NULL, "|");
        printf("\n Data de Vacinacao: ");
        puts(campo);
        campo = strtok(NULL, "|");
        printf("\n Responsavel pela Aplicacao: ");
        puts(campo);
        fflush(stdin);
        if (ind.offset == -1)
          break;
        fseek(*IndSec2, ind.offset, SEEK_SET);
    }
    getch();   
}

int RetornaOffset(char *vacina)
{
    for(int i = 0; i < tam2; i++)
      if(strcmp(vacina, INDEX2[i].vacina) == 0)
        return INDEX2[i].offset;
    return -1;   
}
