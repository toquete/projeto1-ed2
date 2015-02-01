#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Criação da estrutura para AP2
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
void CadastraVacina(FILE **AP1, FILE **AP2, FILE **IndPrim);
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
void CarregaIndice(FILE **arq, int tipo);
void GravaIndices(FILE **IndPrim, FILE **IndSec1, FILE **IndSec2);
void RemoveVacina(FILE **AP1, int pos, int cod_controle);
int RetornaPosicao(int codigo);
void MenuRemoveVacina(FILE **AP1);
void PesquisaCodPrim(FILE **AP1, FILE **AP2, FILE **IndPrim);
void AtualizaListaEspacosVazios(FILE **AP1, int pos);

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
	        case 2: CadastraVacina(&AP1, &AP2, &IndPrim); 
                    break;
			case 3: AlteraCachorro(&AP2);
                    break;
			case 4: AlteraDados(&AP1, &IndPrim);
                    break;
            case 5: MenuRemoveVacina(&AP1);
                    AtualizaInfoIndice('!', &IndPrim);
                    break;
            case 6: PesquisaCodPrim(&AP1, &AP2, &IndPrim);
                    break;
	        case 0: printf("\nSaindo do Programa...");
	                if (ExigeRecriaIndice(&IndPrim))
                      GravaIndices(&IndPrim, &IndSec1, &IndSec2);
					//fecha arquivos principais  
        	        fclose(AP1); 
					fclose(AP2);
					//fecha índices 
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
DESCRIÇÃO: Exibe as opções do menu.
RETORNO: O número da opção escolhida pelo usuário
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
	printf("\n 6 - Pesquisa Vacina");
	printf("\n 0 - Sair");
	printf("\n\nEscolha a opcao: ");
    scanf("%d", &opcao);
	
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
    int header = -1;
    /*No primeiro caso, ele entra no if pois o arquivo ainda não existe 
    (Com o uso do r+b o arquivo tem que existir). 
    Aí então ele cria o arquivo com w+b*/
    if ((*AP1 = fopen("AP1.bin", "r+b")) == NULL) //se o arquivo não exisitr
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
        }
        else
        {
           CarregaIndice(IndPrim, 1);
    	   CarregaIndice(IndSec1, 2);
    	   CarregaIndice(IndSec2, 1);  
        }
	}
    	
    if ((*AP2 = fopen("AP2.bin", "r+b")) == NULL) //se o arquivo não existir
        *AP2 = fopen("AP2.bin", "w+b"); //cria um novo arquivo vazio (AP2)
}

/*
DESCRIÇÃO: Cadastra informações de um cachorro no Arquivo Principal 2
PARÂMETROS: AP2 - Arquivo Principal 2
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

	
	fseek(*AP2, 0, SEEK_END); //seta a posição para o fim do arquivo.
	fwrite(&reg, sizeof(registro), 1, *AP2); //escreve no fim do arquivo.
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
DESCRIÇÃO: Pergunta o código do cachorro para posteriormente fazer uma operação.
PARÂMETROS: AP - Arquivo Principal 2
RETORNOS: Código do cachorro
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
DESCRIÇÃO: Realiza o cadastro de vacinas
PARÂMETROS: AP1 - Arquivo principal 1
            AP2 - Arquivo principal 2
*/
void CadastraVacina(FILE **AP1, FILE **AP2, FILE **IndPrim)
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
      fseek(*AP1, 0, SEEK_END);
      
    INDEX1[tam1].codigo = cod_controle;
    INDEX1[tam1].offset = ftell(*AP1);
    QuickSortInd1(INDEX1, 0, tam1);
    tam1++;
    AtualizaInfoIndice('!', IndPrim);
    
    if (posicao != -1)
      AtualizaListaEspacosVazios(AP1, posicao);
    
    fwrite(&tam_reg, sizeof(int), 1, *AP1);
    fputc(verificador, *AP1);
    fwrite(registro, sizeof(char), tam_reg, *AP1);
}

/*
DESCRIÇÃO: Retorna o espaço vazio encontrado no arquivo
PARÂMETROS: AP1 - Arquivo principal 1
            tam_reg - tamanho do registro a ser inserido
RETORNO: posição livre para ser escrita
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
DESCRIÇÃO: Pergunta o campo do qual será alterado.
PARÂMETROS: 
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
DESCRIÇÃO: Altera os dados de uma cadastro de vacina já exitente
PARÂMETROS: AP1 - Arquivo Principal 1
            IndPrim - Índice primário (para ser atualizado caso haja exclusão)
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
DESCRIÇÃO: Pega o campo em AP1
PARÂMETROs: AP1 - arquivo principal 1
            Campo de retorno
            Posição inicial do campo
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
DESCRIÇÃO: Altera dados de um cachorro
PARÂMETRO: AP2 - arquivo principal 2
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
DESCRIÇÃO: Verifica se é necessário recriar um índice a partir de um arquivo
PARÂMETRO: ind - Índice a ser verificado
RETORNOS: 0 - Não é necessário recriar o índice
          1 - É necessário recriar o índice
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
DESCRIÇÃO: Recria o índice primário a partir do arquivo e carrega em memória principal
PARÂMETRO: AP1 - Arquivo Principal 1
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

void CarregaIndice(FILE **arq, int tipo)
{
	indice1 ind1;
	indice2 ind2;
	
	if (tipo != 2)
	{
		fseek(*arq, sizeof(char), SEEK_SET);
		while (fread(&ind1, sizeof(indice1), 1, *arq))
		{
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

void GravaIndices(FILE **IndPrim, FILE **IndSec1, FILE **IndSec2)
{
    fseek(*IndPrim, 0, SEEK_SET);
    AtualizaInfoIndice('*', IndPrim);
    for(int i = 0; i < tam1; i++)
      fwrite(&INDEX1[i], sizeof(INDEX1[i]), 1, *IndPrim);
    fputc(EOF, *IndPrim);
      
    //rewind(*IndSec1);
    //AtualizaInfoIndice('*', IndSec1);
    //fseek(IndPrim, sizeof(char), SEEK_SET);
    //for(int i = 0; i < tam2; i++)
    //  fwrite(&INDEX2[i], sizeof(INDEX2[i]), 1, *IndSec1);
      
    //rewind(*IndSec2);
    //AtualizaInfoIndice('*', IndSec2);
    //fseek(IndPrim, sizeof(char), SEEK_SET);
    //for(int i = 0; i < tam2; i++)
    //  fwrite(&INDEX3[i], sizeof(INDEX3[i]), 1, *IndSec2);
      
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
    fread(&header, sizeof(int), 1, *AP1);
    fseek(*AP1, pos + sizeof(int), SEEK_SET);
    fwrite(&verificador, sizeof(char), 1, *AP1);
    fwrite(&header, sizeof(int), 1, *AP1);
    fseek(*AP1, 0, SEEK_SET);
    fwrite(&pos, sizeof(int), 1, *AP1); 
    
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

void PesquisaCodPrim(FILE **AP1, FILE **AP2, FILE **IndPrim)
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
    
    campo = strtok(temp, "|"); //código do controle
    campo = strtok(NULL, "|"); //código do cachorro
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
    int aux, header, offset, posicao;
    
    fseek(*AP1, sizeof(char), SEEK_CUR);
    fread(&aux, sizeof(int), 1, *AP1);
    if (aux == -1)
    {
        fseek(*AP1, 0, SEEK_SET);
        fwrite(&aux, sizeof(int), 1, *AP1);
    }
    else
    {
        fseek(*AP1, 0, SEEK_SET);
        fread(&header, sizeof(int), 1, *AP1);
        if ((pos - header) == 0)
        {
           fseek(*AP1, 0, SEEK_SET);
           fwrite(&aux, sizeof(int), 1, *AP1); 
        }
        else
        {
            fseek(*AP1, 0, SEEK_SET);
            fread(&offset, sizeof(int), 1, *AP1);
            fseek(*AP1, offset + sizeof(int) + sizeof(char), SEEK_SET);
            posicao = ftell(*AP1);
            while (fread(&offset, sizeof(int), 1, *AP1))
            {
                if (offset == pos)
                  break;
                fseek(*AP1, offset + sizeof(int) + sizeof(char), SEEK_SET);
            }
            fseek(*AP1, posicao, SEEK_SET);
            fwrite(&aux, sizeof(int), 1, *AP1);
        }
    }
}

void Compacta (FILE **AP1)
{
    char ch, *aux;
    int offset, i, pos, pos_fim;
    rewind(*AP1);
    
    while(fread(&ch, sizeof(char), 1, *AP1))
    {
        if(ch == '!')
        {
            fseek(*AP1, -sizeof(int), SEEK_CUR);
            pos = ftell(*AP1);
            fread(&offset, sizeof(int), 1, *AP1);
            fseek(*AP1, offset, SEEK_CUR);
            for (i=0; i != EOF; i++)
                aux[i] = fgetc(*AP1);
            //pos_fim = ftell(*AP1);
            //pos_fim = pos_fim - offset;
            //fseek(*AP1, 0, pos_fim);
            //fwrite(EOF, sizeof(int), 1, *AP1);
            aux[i+1] = '\0';
            fseek(*AP1, 0, pos);
            fwrite(aux, strlen(aux), 1, *AP1);
        }
    }
}
