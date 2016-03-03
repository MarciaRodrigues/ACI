#include<iostream>
#include <stdlib.h>
#include <list>
#include "modbus/src/modbus.h"
#include "modbus/src/modbus-tcp.h"
//#include <modbus.h>
//#include <modbus-tcp.h>
#include <sys/types.h>
#include <time.h>

using namespace std;

//variaveis a ler do isagraf
//estado das maquinas
#define estadoMaC1 1		//maquina A da celula 1 (ocupacao)
#define estadoMaC2 2		//maquina A da celula 2 (ocupacao)
#define estadoMaC3 3		//maquina A da celula 3 (ocupacao)
#define estadoMbC1 4		//maquina B da celula 1 (ocupacao)
#define estadoMcC2 5		//maquina C da celula 2 (ocupacao)
#define estadoMcC3 6		//maquina C da celula 3 (ocupacao)
//estado dos tapetes AT1 e AT2
#define tapeteAT1 9		//sensor do tapete AT1
#define tapeteAT2 10		//sensor do tapete AT2 (entrada do armazem)
//estado da celula de montagem
#define estadoM 0		//ocupacao da celula de montagem	
//estado dos pushers
#define estadoP1 7		//ocupacao do pusher 1
#define estadoP2 8		//ocupacao do pusher 2
//entrada do armazem (AT2)
#define ordemArm 16		//tipo de peca que vai entrar no armazem
#define pecaArm 17		//numero de ordem da peca que vai entrar no armazem
//pecas produzidas/montadas/descarregadas
#define npecasC1 11		//numero de pecas que sairam da celula 1
#define npecasC2 12		//numero de pecas que sairam da celula 2
#define npecasC3 13		//numero de pecas que sairam da celula 3
#define npecasM 14		//numero de pecas que sairam da montagem
#define nPusher 15		//numero de pecas que sairam dos pushers
#define wareOut 18		//valor de warehouseOut no Isagraf
#define tapeteST2 19		//sensor do tapete ST1
#define prontoAc1 20		//fim de maquinaçao da maquina A da celula 1
#define prontoAc2 22		//fim de maquinaçao da maquina A da celula 2
#define prontoAc3 23		//fim de maquinaçao da maquina A da celula 3
#define prontoBc1 21		//fim de maquinaçao da maquina B da celula 1
#define prontoCc2 24		//fim de maquinaçao da maquina C da celula 2
#define prontoCc3 25		//fim de maquinaçao da maquina C da celula 3
#define prontoRobot 26		//robot pegou a peca que estava no transporte
#define prontoDescarga 27	//peca passou do transporte para a celula de descarga


//variaveis a escrever do isagraf (para tapete AT1)
#define tipoPeca 1		//peca a ser colocada no tapete 1 (corresponde a  PO dos grafcets)
#define tempo 0			//tempo de processamento da peca
#define ferramenta 3		//ferramenta a ser utilizada na transformacao
#define chegada 2		//celula de destino da peca a colocar no tapete 1
#define numOrdem 4		//numero de ordem de producao
#define pecaFinal 5		//peca final apos transformacao(corresponde a  PF dos grafcets)
#define maqAc1 6		//indica que enviou peca para maquina A da celula 1 (ocupa a maquina)
#define maqAc2 8		//indica que enviou peca para maquina A da celula 2 (ocupa a maquina)
#define maqAc3 9		//indica que enviou peca para maquina A da celula 3 (ocupa a maquina)
#define maqBc1 7		//indica que enviou peca para maquina B da celula 1 (ocupa a maquina)
#define maqCc2 10		//indica que enviou peca para maquina C da celula 2 (ocupa a maquina)
#define maqCc3 11		//indica que enviou peca para maquina C da celula 3 (ocupa a maquina)
#define tRobot 12		//indica que envioi peca para montagem (ocupa a montagem ate o robot pegar a peca)
#define tDescarga 13		//indica que envioi peca para descarga (ocupa a descarga ate a peca sair do transporte)



struct tm *local;

class pedidos {
  public :
    int ordem;			//numero de ordem de fabrico
    string tipo;		//tipo de pedido
    int quant;			//quantidade pedida
    int Porigem;		//peca original (producao)
    int Pfinal;		//peca final (producao)
    int Pbaixo;		//peca de baixo (montagem)
    int Pcima;			//peca de cima (montagem)
    int tipoP;			//tipo de peca (descarga)
    int destino;		//destino da peca a colocar na linha de producao
    int prontas;		//numero de pecas prontas de pedido correspondente
    string status;  		//estado do pedido
    string hEntrada;		//hora de entrada do pedido
    string hInicio;		//hora de inicio de execucao do pedido
    string hFim; 		//hora de fim de execucao do pedido (pedido completo)
    int enviadas;		//numero de pecas de cada pedido enviadas para a linha de producao
};


class incompletos {
  public :
    int numero;	//numero de ordem de fabrico
    int temp;		//tipo de peca intermedia
};


//variaveis do MES
modbus_t *ctx;

int errno;
int rc;
uint16_t varL[32], varE[32];

//numero de pecas que entraram na linha (carregada)
int carregadas=0;
//tempo temporario de fabrico de cada maquinas
int tempoAc1=0;
int tempoAc2=0;
int tempoAc3=0;
int tempoBc1=0;
int tempoCc2=0;
int tempoCc3=0;
//tempo total de operacao das maquinas
int tempoTAc1=0;
int tempoTAc2=0;
int tempoTAc3=0;
int tempoTBc1=0;
int tempoTCc2=0;
int tempoTCc3=0;

//indicacao interna do estado das maquinas
int ocupadaAc1=0;
int ocupadaAc2=0;
int ocupadaAc3=0;
int ocupadaBc1=0;
int ocupadaCc2=0;
int ocupadaCc3=0;
int ocupadaM=0;
int ocupadaD=0;

//timer
time_t t;

//lista de pedidos
list<pedidos> lista;
list<pedidos>::iterator it;

pthread_mutex_t mutexLista;


//cabecalho das funcoes
int montagem(int Pbaixo, int Pcima);
int descarga(int peca, int local);
int transf_pecas(string caminho);
int prod_pecas(int PO, int PF);
void * gerePedidos(void * args);


int main(int argc, char *argv[]){
  modbus_t *ctx;
  int errno;
  int rc;

  list<incompletos> arm;	//pedidos incompletos que se encontram no armazem
  list<incompletos>::iterator count;  
 
  pedidos ped;
  incompletos incomp;
  string ent; 
  int in;
  int res;
  int i;
  int inc=0;
  int monta=0;
  int aux=0;
  int novo=0;
  int at2=0;
  
  pthread_t thread;  
  
  t=time(NULL);
  local=localtime(&t);
  
  //estabelecer comunicacao
  ctx = modbus_new_tcp("172.30.83.122", 502);
  if (ctx == NULL) {
    cerr << "Unable to allocate libmodbus context" << endl;
    return -1;
  }

  if (modbus_connect(ctx) == -1) {
    cerr << "Connection failed: " << modbus_strerror(errno) << endl;
    modbus_free(ctx);
    return -1;
  }
  else
    cout << "Connection OK" << endl;
    
  struct timeval old_response_timeout;
  struct timeval response_timeout;
  
  /* Save original timeout */
  modbus_get_response_timeout(ctx, &old_response_timeout);
  
//  cout << "old_response_timeout sec: " << old_response_timeout.tv_sec << endl << "old_response_timeout usec: " << old_response_timeout.tv_usec << endl;
  
  /* Define a new and too short timeout! */
  response_timeout.tv_sec = 2;
  response_timeout.tv_usec = 0;
  modbus_set_response_timeout(ctx, &response_timeout);
       
  pthread_mutex_init(&mutexLista, NULL);
    
  pthread_create(&thread, NULL, gerePedidos, NULL);    

  while(1){
    //ler todas as variaveis necessarias do isagraf no inicio do ciclo
    rc = modbus_read_registers(ctx, estadoM, 28, varL);

    if (rc == -1) {    
      cerr << modbus_strerror(errno) << endl;
      exit(-1);
    }
    
    aux=0;
    novo=0;
    if(varL[tapeteST2]==1){
      varE[tipoPeca]=0;
      novo=1;
    }

    //ver ocupacao das maquinas
    if(varL[prontoAc1]==1){		//se ja acabou a transoformacao
      varE[maqAc1]=0;			//desocupar a maquina
      ocupadaAc1=0;			//desocupar a maquina (interno)
      novo=1;				//escrever variaveis (novo ciclo)
      tempoTAc1=tempoTAc1+tempoAc1;
      tempoAc1=0;
    }
    if(varL[prontoAc2]==1){
      varE[maqAc2]=0;
      ocupadaAc2=0;
      novo=1;  
      tempoTAc2=tempoTAc2+tempoAc2;
      tempoAc2=0;
    }
    if(varL[prontoAc3]==1){
      varE[maqAc3]=0;
      ocupadaAc3=0;
      novo=1;
      tempoTAc3=tempoTAc3+tempoAc3;
      tempoAc3=0;
    }
    if(varL[prontoBc1]==1){
      varE[maqBc1]=0; 
      ocupadaBc1=0;
      novo=1;
      tempoTBc1=tempoTBc1+tempoBc1;
      tempoBc1=0;
    }
    if(varL[prontoCc2]==1){
      varE[maqCc2]=0;
      ocupadaCc2=0;
      novo=1;
      tempoTCc2=tempoTCc2+tempoCc2;
      tempoCc2=0;
    }
    if(varL[prontoCc3]==1){
      varE[maqCc3]=0;
      ocupadaCc3=0;
      novo=1;
      tempoTCc3=tempoTCc3+tempoCc3;
      tempoCc3=0;
    }
    
    //ver ocupacao da montagem
    if(varL[prontoRobot]==1){
      varE[tRobot]=0;
      ocupadaM=0;
      novo=1;
    }
    
    //ver ocupacao da descarga
    if(varL[prontoDescarga]==1){
      varE[tDescarga]=0;
      ocupadaD=0;
      novo=1;
    }
    
    //ver se acabou de sair peca do tapete AT2
    if(varL[tapeteAT2]==0)
      at2=0;

    //caso exista uma peca para entrar no armazem. E preciso verificar se essa peca ja esta completamente processada
    if (varL[tapeteAT2]==1 && at2==0){				//se existir peca a entrada do armazem
    at2=1;
    aux=0;
//    cout << "TAPETE AT2" << endl;
      if (varL[pecaArm]!=0 && varL[ordemArm]!=0){	//se a peca vier de uma transformacao
        //vamos escrever na lista de pedidos pendentes do armazem
        incomp.numero=varL[ordemArm];
        incomp.temp=varL[pecaArm];
        arm.push_back(incomp);				//colocar na lista o numero de ordem e o tipo de peca que entrou no armazem

	pthread_mutex_lock(&mutexLista);		//nao pode ler enquanto se esta a escrever nem escrever quando se esta a ler
        for (it=lista.begin(); it!=lista.end(); it++){
          for (count=arm.begin(); count!=arm.end(); count++) {
            if ((*count).numero==(*it).ordem){		//se o numero de ordem de uma pecas que entrou no arm for igual ao numero de ordem de um pedido
              if ((*count).temp==(*it).Pfinal){	//se a peca que entrou no arm for a mesma que o tipo de peca final para o pedido correspondente
                arm.erase(count);			//pedido completo; eliminar da lista de incompletos
                (*it).prontas=(*it).prontas+1;		//incrementar uma unidade a quantidade processada do pedido
                if ((*it).prontas==(*it).quant){	//se ja foram processadas todas as pecas do pedido
                  (*it).status="pronto";
   	          t=time(NULL);
                  local=localtime(&t);
                  (*it).hFim=ctime(&t);
                }
                aux=1;              
                break;  
              }
            }
          }
          if (aux==1)		//se fez break no segundo for tambem tem que fazer no primeiro
            break;
        }
        pthread_mutex_unlock(&mutexLista);
      }
      else /*if(varL[pecaArm]==0 && varL[ordemArm]==0)*/{	//peca vem do tapete CT8 (carregar peca)
        carregadas=carregadas+1;
      }
    }
    aux=0;
    
    //vamos ver se pode atender algum pedido pendente...
    if (varL[tapeteAT1]==0 && novo==0){		//so pode enviar peca se tapete AT1 livre
      pthread_mutex_lock(&mutexLista);
      for (it=lista.begin(); it!=lista.end(); it++){
        for (count=arm.begin(); count!=arm.end(); count++){
          if ((*count).numero==(*it).ordem){		//para incompletos como o mesmo numero de ordem que os pedidos
            res=prod_pecas((*count).temp, (*it).Pfinal);
            if (res!=0){				//se pode atender a um incompleto
              arm.erase(count);				//apagar dos incompletos
              varE[numOrdem]=(*it).ordem;		//escrever num de ordem no primeiro tapete
              aux=1;
              break;
            }
          }
        }
        if (aux==1)
        break;
      }
    pthread_mutex_unlock(&mutexLista);      
    }

    if (aux==0 && novo==0){		//se nao atendeu a nenhum pedido anterior
      if (monta!=0 && ocupadaM==0){	//se houver uma segunda peca (peca de cima) para enviar para a montagem o programa nao atende mais nada
        if (varL[tapeteAT1]==0 && varL[wareOut]==0){		//se o tapete AT1 nao tiver peca pode enviar
          pthread_mutex_lock(&mutexLista);            
          for (it=lista.begin(); it!=lista.end(); it++){
            if ((*it).ordem==monta){
              //enviar peca de cima para o primeiro tapete (escrever)
              varE[chegada]=6;			//dizer que o destino e a celula de montagem
              varE[tipoPeca]=(*it).Pcima;	 	//tipo de peca a enviar
              (*it).prontas=(*it).prontas+1;
              ocupadaM=1;			//ocupa a montagem
              varE[tRobot]=1;
              if ((*it).prontas==(*it).quant){
  	        (*it).status="pronto";
  	        t=time(NULL);
                local=localtime(&t); 
                (*it).hFim=ctime(&t);
	      }
  	      monta=0;
  	      aux=1;
  	      break;
            }
          }
          pthread_mutex_unlock(&mutexLista);
        }
      }
    }
    if(aux==0 && varL[wareOut]==0 && novo==0){
      pthread_mutex_lock(&mutexLista);     
      for(it=lista.begin(); it!=lista.end(); it++){

        if((*it).tipo=="producao" && (*it).enviadas!=(*it).quant){ //enviadas significa o numero de pecas enviadas para producao. Nao pode ser 
								//utilizado o status porque enquanto a peca nao der entrada no armazem nao e atualizado
	  res=prod_pecas((*it).Porigem, (*it).Pfinal);	//retorna a transformacao da peca. A peca e transformada em res
	  if(res!=0){					//pedido satisfeito
	    if((*it).enviadas==0){
	      t=time(NULL);
              local=localtime(&t);
              (*it).hInicio=ctime(&t);
	    }
	    (*it).enviadas=(*it).enviadas+1;
	    varE[numOrdem]=(*it).ordem;			//o tipo de peca em que vai ser transformada (res) ja foi escrito na funcao transf_pecas
	    break;
          }
        }       
        else
          if((*it).tipo=="montagem" && (*it).enviadas!=(*it).quant && ocupadaM==0){
            //funcao de montagem
            res=montagem((*it).Pbaixo, (*it).Pcima);
       	    if (res==1){					//pode enviar para celula de montagem
       	      if((*it).enviadas==0){
	        t=time(NULL);
                local=localtime(&t);
                (*it).hInicio=ctime(&t);
	      }
       	      (*it).enviadas=(*it).enviadas+1;
       	      monta=(*it).ordem;		//ainda temos que enviar a peca de cima correspondente. guarda a peca de cima na variavel monta
       	      ocupadaM=1;			//ocupa a montagem
       	      varE[tRobot]=1;
              break;			//enviou peca de baixo...tem que atualizar o sistema
            }
          }            
          else
            if((*it).tipo=="descarga" && (*it).enviadas!=(*it).quant && ocupadaD==0){
              res=descarga((*it).tipoP, (*it).destino);                  
              if (res==1){
                if((*it).enviadas==0){
	          t=time(NULL);
                  local=localtime(&t);
                  (*it).hInicio=ctime(&t);
	        }
                (*it).enviadas=(*it).enviadas+1;
                (*it).prontas=(*it).prontas+1;
                ocupadaD=1;				//ocupa a descarga
                varE[tDescarga]=1;
                if ((*it).prontas==(*it).quant){
                  (*it).status="pronto";                  
	          t=time(NULL);
                  local=localtime(&t);
                  (*it).hFim=ctime(&t);	          
                }
                break;		//enviou peca...tem que atualizar o sistema
              }              
            }
        }
        pthread_mutex_unlock(&mutexLista);
    }  
    //escrever variaveis no isagraf no final do ciclo     
    rc=modbus_write_registers(ctx, tempo, 14, varE);  
    
    if (rc == -1) {    
      cerr << modbus_strerror(errno) << endl;
      exit(-1);
    }
  }
  
  modbus_close(ctx);
  modbus_free(ctx);

  return 0; 
}


int montagem(int Pbaixo, int Pcima){
  if(varL[tapeteAT1]==0 && varL[wareOut]==0){	//se o tapete 1 nao tiver peca
    if(varL[estadoM]==0){	//se a celula de montagem nao estiver lotada
      varE[chegada]=6;	//dizer que o destino e a celula de montagem
      varE[tipoPeca]=Pbaixo; 	//tipo de peca a enviar

      //ainda falta enviar a peca de cima
      return 1;
    }
  }
  return 0;
}


int descarga(int peca, int local) {
  //verificar se o primeiro tapete tem peca
  if(varL[tapeteAT1]==0 && varL[wareOut]==0){
    if (local==1){	//verificar para que pusher tem que ir a peca
      if (varL[estadoP1]==0){	//ver ocupacao do pusher 1
        varE[chegada]=7;		//destino e o pusher 1

        //escrever qual a peca
        varE[tipoPeca]=peca;
            
        return 1;      
      } 
    }
    else
      if (local==2){  //verificar se tem que ir para o pusher 2
	if(varL[estadoP2]==0){	//ver ocupacao do pusher 2
          varE[chegada]=8;		//destino e o pusher 2

          //escrever qual a peca
          varE[tipoPeca]=peca;
              
          return 1;      
        }       
      }
  }
  return 0;
}


int transf_pecas(string caminho){
  if(caminho=="A1"){		//A1 significa maquina A ferramenta 1
    if(ocupadaAc1==0){		//maquina A da celula 1 livre
      varE[chegada]=1;
      varE[ferramenta]=1;
      varE[tempo]=15;
      varE[tipoPeca]=1;		//peca antes da transoformacao
      varE[pecaFinal]=2;	//peca depois da transformacao
      
      varE[maqAc1]=1;
      ocupadaAc1=1;
      
      tempoAc1=15;
        
      return 2;
    }
    else if(ocupadaAc2==0){		//maquina A da celula 2 livre
      varE[chegada]=2;
      varE[ferramenta]=1;
      varE[tempo]=15;
      varE[tipoPeca]=1;
      varE[pecaFinal]=2;
        
      varE[maqAc2]=1;
      ocupadaAc2=1;
      
      tempoAc2=15;
      
      return 2;
    }
    else if(ocupadaAc3==0){		//maquina A da celula 3 livre
      varE[chegada]=4;
      varE[ferramenta]=1;
      varE[tempo]=15;
      varE[tipoPeca]=1;		
      varE[pecaFinal]=2;		
      
      varE[maqAc3]=1;
      ocupadaAc3=1;
      
      tempoAc3=15;
      
      return 2;
    }
    else			//Se nenhuma maquina A disponivel
      return 0;    
  }
  
  else if(caminho=="A2"){
    if(ocupadaAc1==0){		//maquina A da celula 1 livre
      varE[chegada]=1;
      varE[ferramenta]=2;
      varE[tempo]=30;
      varE[tipoPeca]=3;
      varE[pecaFinal]=5;
      
      varE[maqAc1]=1;
      ocupadaAc1=1;
      
      tempoAc1=30;
      
      return 5;
    }
    else if(ocupadaAc2==0){		//maquina A da celula 2 livre
      varE[chegada]=2;
      varE[ferramenta]=2;
      varE[tempo]=30;
      varE[tipoPeca]=3;
      varE[pecaFinal]=5;
      
      varE[maqAc2]=1;
      ocupadaAc2=1;
      
      tempoAc2=30;
      
      return 5;
    }
    else if(ocupadaAc3==0){		//maquina A da celula 3 livre
      varE[chegada]=4;
      varE[ferramenta]=2;
      varE[tempo]=30;
      varE[tipoPeca]=3;
      varE[pecaFinal]=5;
      
      varE[maqAc3]=1;
      ocupadaAc3=1;
      
      tempoAc3=30;
      
      return 5;
    }    
    else 
      return 0;    
  }
  
   else if(caminho=="A3"){		// significa maquina A ferramenta 3
     if(ocupadaAc1==0){			//maquina A da celula 1 livre
       varE[chegada]=1;
       varE[ferramenta]=3;
       varE[tempo]=10;
       varE[tipoPeca]=3;
       varE[pecaFinal]=4;
       
       varE[maqAc1]=1;
       ocupadaAc1=1;
       
       tempoAc1=10;
       
       return 4;
     }
     else if(ocupadaAc2==0){		//maquina A da celula 2 livre
       varE[chegada]=2;
       varE[ferramenta]=3;
       varE[tempo]=10;
       varE[tipoPeca]=3;
       varE[pecaFinal]=4;
       
       varE[maqAc2]=1;
       ocupadaAc2=1;
       
       tempoAc2=10;
       
       return 4;       
     }
     else if(ocupadaAc3==0){		//maquina A da celula 3 livre
       varE[chegada]=4;
       varE[ferramenta]=3;
       varE[tempo]=10;
       varE[tipoPeca]=3;
       varE[pecaFinal]=4;
       
       varE[maqAc3]=1;
       ocupadaAc3=1;
       
       tempoAc3=10;
       
       return 4;
     }         
     else
       return 0;
   }

   else if (caminho=="B1"){	//significa maquina B ferramenta 1
    if(ocupadaBc1==0 && ocupadaAc1==0){		//maquina B da celula 1 livre e A tambem livre
       varE[chegada]=1;
       varE[ferramenta]=1;
       varE[tempo]=30;
       varE[tipoPeca]=1;
       varE[pecaFinal]=3;      
       
       varE[maqBc1]=1;
       ocupadaBc1=1;
       
       tempoBc1=30;
       
       return 3;
      }
     else 
       return 0;       
   }

   else if (caminho=="B2"){	//significa maquina B ferramenta 2
     if(ocupadaBc1==0 && ocupadaAc1==0){		//maquina B da celula 1 livre
       varE[chegada]=1;
       varE[ferramenta]=2;
       varE[tempo]=15;
       varE[tipoPeca]=2;
       varE[pecaFinal]=3;      
       
       varE[maqBc1]=1;
       ocupadaBc1=1;
       
       tempoBc1=15;
       
       return 3;
      }
     else 
       return 0;       
   }

   else if (caminho=="B3"){	//significa maquina B ferramenta 3
    if(ocupadaBc1==0 && ocupadaAc1==0){		//maquina B da celula 1 livre
       varE[chegada]=1;
       varE[ferramenta]=3;
       varE[tempo]=10;
       varE[tipoPeca]=5;
       varE[pecaFinal]=7;      
       
       varE[maqBc1]=1;
       ocupadaBc1=1;
       
       tempoBc1=10;
       
       return 7;
      }
     else 
       return 0;       
   }

   else if(caminho=="C1"){
    if(ocupadaCc2==0){		//maquina C da celula 2 livre
       varE[chegada]=3;
       varE[ferramenta]=1;
       varE[tempo]=15;
       varE[tipoPeca]=2;
       varE[pecaFinal]=4;
       
       varE[maqCc2]=1;
       ocupadaCc2=1;
       
       tempoCc2=15;
       
       return 4;
     }
     else if(ocupadaCc3==0){		//maquina C da celula 3 livre
       varE[chegada]=5;
       varE[ferramenta]=1;
       varE[tempo]=15;
       varE[tipoPeca]=2;
       varE[pecaFinal]=4;
       
       varE[maqCc3]=1;
       ocupadaCc3=1;
       
       tempoCc3=15;
       
       return 4;
     }         
     else
       return 0;
   }

   else if(caminho=="C2"){
    if(ocupadaCc2==0){		//maquina C da celula 2 livre
       varE[chegada]=3;
       varE[ferramenta]=2;
       varE[tempo]=20;
       varE[tipoPeca]=4;
       varE[pecaFinal]=6;
       
       varE[maqCc2]=1;
       ocupadaCc2=1;
       
       tempoCc2=20;
       
       return 6;
    }
    if(ocupadaCc3==0){		//maquina C da celula 3 livre
       varE[chegada]=5;
       varE[ferramenta]=2;
       varE[tempo]=20;
       varE[tipoPeca]=4;
       varE[pecaFinal]=6;
       
       varE[maqCc3]=1;
       ocupadaCc3=1;
       
       tempoCc3=20;
       
       return 6;
     }    
     else
       return 0;
   }

   else if(caminho=="C3"){
    if(ocupadaCc2==0){		//maquina C da celula 2 livre
       varE[chegada]=3;
       varE[ferramenta]=3;
       varE[tempo]=30;
       varE[tipoPeca]=2;
       varE[pecaFinal]=6;
       
       varE[maqCc2]=1;
       ocupadaCc2=1;
       
       tempoCc2=30;
       
       return 6;
    }
    else if(ocupadaCc3==0){		//maquina C da celula 3 livre
       varE[chegada]=5;
       varE[ferramenta]=3;
       varE[tempo]=30;
       varE[tipoPeca]=2;
       varE[pecaFinal]=6;
       
       varE[maqCc3]=1;
       ocupadaCc3=1;
       
       tempoCc3=30;
       
       return 6;
     }    
     else
       return 0;
   }   
}

int prod_pecas(int PO, int PF){	//retorna 1, se pedido satisfeito
  int feito=0;

  if(PO==1){
    if(PF==2)
      return transf_pecas("A1"); //1-2
          
    else if(PF==3)
      return transf_pecas("B1"); //1-3
    
    else if(PF==4){		//tera de fazer o percurso 1->2->4 ou 1->3->4
      //Comeca-se por experimentar o percurso 1-> 2
      feito=transf_pecas("A1"); //1-2
      if(feito==0)		//percurso 1->2 ocupado
        feito=transf_pecas("B1");
        
      return feito;
    }
    else if (PF==5){		//1-3-5 ou 1-2-3-5
      feito=transf_pecas("B1"); //1-3
      if(feito==0)		//percurso 1-3 ocupado
        feito=transf_pecas("A1");//1-2
        
      return feito;      
    }
    else if(PF==6){	//1-2-6 ou 1-3-4-6; caminho 1-2-4-6 nao interessa porque se nao 1-2-6 tambem nao faz 1-2-4-6 porque o primeiro caminho e igual
      feito=transf_pecas("A1");	//1-2
      if(feito==0)		//1-2 ocupado
        feito=transf_pecas("B1");//1-3
      return feito;
    }
    else if(PF==7){	//1-3-5-7 ou 1-2-3-5-7
      feito=transf_pecas("B1"); //1-3
      if(feito==0)
        feito=transf_pecas("A1"); //1-2
      return feito;
    }
  }
  else if(PO==2){
    if(PF==3)
      return transf_pecas("B2"); //2-3
    
    else if(PF==4)
      return transf_pecas("C1"); //2-4
      
    else if(PF==5)	//2-3-5
      return transf_pecas("B2"); //2-3
      
    else if(PF==6){	//2-6 ou 2-4-6
      feito=transf_pecas("C3"); //2-6
      if(feito==0)
        feito=transf_pecas("C1"); //2-4
      return feito;
    }
    else if(PF==7)	//2-3-5-7
      return transf_pecas("B2");    
  }
  else if(PO==3){
    if(PF==4)
      return transf_pecas("A3"); //3-4
    else if(PF==5)
      return transf_pecas("A2"); //3-5
    else if(PF==6)	//3-4-6
      return transf_pecas("A3"); //3-4
    else if(PF==7)	//3-5-7
      return transf_pecas("A2"); //3-5
  }
  else if(PO==4){
    if(PF==6)
      return transf_pecas("C2"); //4-6
  }
  else if(PO==5){
    if(PF==7)
      return transf_pecas("B3"); //5-7
  }   
}  

void * gerePedidos(void * args){
  //inicializar tempos
  t=time(NULL);
  local=localtime(&t);
  //start-tempo de inicio do programa; now-tempo atual
  time_t start, now;
  time (&start);
  
  int in, inc=0, aux=0;
  string ent;
  pedidos ped;
  cout << "-----COMANDOS-----" <<endl << ">>Producao (p)" << endl << ">>Montagem (m)" << endl << ">>Descarga (d)" << endl << ">>Lista de Ordens (l)" 
       << endl << ">>Estatistica da Maquinas (e)" << endl << ">>Pecas Carregadas (x)" << endl << ">>Comandos (c)" << endl << ">>exit" << endl << endl;
         
  while(1){
    cin >> ent;
    if (ent == "p" || ent == "m" || ent == "d" || ent=="l" || ent=="e" || ent=="x" || ent=="c" || ent=="exit"){
      if (ent == "p"){ // diz a função a fazer
        ped.tipo="producao";
      	      	
      	cout << "Peca de origem: P"; // peca de origem
      	cin >> in;
     	while(in<1 || in>5){
      	  cout << "PO entre 1 e 5!" << endl;
      	  cout << "Peca de origem: P"; // peca de origem
      	  cin >>in; 
      	}
     	ped.Porigem=in;
      	
      	aux=in;
        cout << "Peca final: P"; // peca apos transformacao
        cin >> in;
        if(aux==1){		//PO=1
          while(in<2 || in>7){
            cout << "Para PO=1, PF entre 2 e 7!" << endl;
            cout << "Peca final: P"; // peca apos transformacao
            cin >>in;
          }
        }
        else if(aux==2){	//PO=2
          while(in<3 || in>7){
            cout << "Para PO=2, PF entre 3 e 7!" << endl;
            cout << "Peca final: P"; // peca apos transformacao
            cin >>in;
          }
        }
        else if(aux==3){	//PO=3
          while(in<4 || in>7){
            cout << "Para PO=3, PF entre 4 e 7!" << endl;
            cout << "Peca final: P"; // peca apos transformacao
            cin >>in;
          }
        }
        else if(aux==4){	//PO=4
          while(in!=6){
            cout << "Para PO=4, PF=6!" << endl;
            cout << "Peca final: P"; // peca apos transformacao
            cin >>in;
          }
        }
        else if(aux==5){	//PO=5
          while(in!=7){
            cout << "Para PO=5, PF=7!" << endl;
            cout << "Peca final: P"; // peca apos transformacao
            cin >>in;
          }
        }
      	ped.Pfinal=in;
      	
      	cout << "Quantidade: ";
      	cin >> in;
      	ped.quant=in;
      	
       	ped.Pbaixo = ped.Pcima = ped.tipoP = ped.destino = ped.prontas = ped.enviadas = 0;
      	ped.status=ped.hEntrada=ped.hInicio=ped.hFim="0";
      	inc++;
      	ped.ordem=inc;
      	
      	t=time(NULL);
        local=localtime(&t);
      	ped.hEntrada=ctime (&t);
      
        pthread_mutex_lock(&mutexLista);
        lista.push_back(ped); // adiciona o elemento no fim da lista     	
        pthread_mutex_unlock(&mutexLista);
      }
      
      else
       if (ent == "m") {
         ped.tipo="montagem";
      	
      	 cout << "Peca em baixo: P";
      	 cin >> in;
      	 while(in<1 || in>7){
      	   cout << "Peca em baixo: P";
      	   cin >> in;
      	 }
      	 ped.Pbaixo=in;
      	
      	 cout << "Peca em cima: P";
      	 cin >> in;
      	 while(in<1 || in>7){
      	   cout << "Peca em cima: P";
      	   cin >> in;
      	 }
      	 ped.Pcima=in;
      	
       	 cout << "Quantidade: ";
      	 cin >> in;
      	 ped.quant=in;
      	
      	 ped.Porigem = ped.Pfinal = ped.tipoP = ped.destino = ped.prontas = ped.enviadas = 0;
      	 ped.status=ped.hEntrada=ped.hInicio=ped.hFim="0";
      	 inc++;
      	 ped.ordem=inc;
      	 
      	 t=time(NULL);
         local=localtime(&t);
      	 ped.hEntrada=ctime (&t);
         
         pthread_mutex_lock(&mutexLista);
         lista.push_back(ped);      	
         pthread_mutex_unlock(&mutexLista);
       }
       
       else
         if (ent == "d") {
      	   ped.tipo="descarga";
      	  
      	   cout << "Tipo de peca: P";
      	   cin >> in;
      	   while(in<1 || in>7){
      	     cout << "Tipo de peca: P";
      	     cin >> in;
      	   }
      	   ped.tipoP=in;
      	
      	   cout << "Destino: Pusher";
      	   cin >> in;
      	   while(in<1 || in>2){
      	     cout << "Destino: Pusher";
      	     cin >> in;      	     
      	   }
      	   ped.destino=in;
      	
      	   cout << "Quantidade: ";
      	   cin >> in;
      	   ped.quant=in;
      	
      	   ped.Porigem = ped.Pfinal = ped.Pbaixo = ped.Pcima = ped.prontas = ped.enviadas = 0;
       	   ped.status=ped.hEntrada=ped.hInicio=ped.hFim="0";
      	   inc++;
      	   ped.ordem=inc;
      	   
      	   t=time(NULL);
           local=localtime(&t);
      	   ped.hEntrada=ctime (&t);
      
           pthread_mutex_lock(&mutexLista);
           lista.push_back(ped);      	
           pthread_mutex_unlock(&mutexLista);
         }
       else
         if(ent=="l"){
         cout << "-----Lista de Ordens-----" << endl;
           
           cout << ">>>Producao<<<" <<endl;
           cout << "N  " << "PO  " << "PF  " << "Prod  " << "EmProd   " << "Pend  " << "Ent  \t\t\t" << "Ini  \t\t\t" << "Fim" << endl;
           pthread_mutex_lock(&mutexLista);
           for(it=lista.begin(); it!=lista.end(); it++){
             if((*it).tipo=="producao"){
               cout<<(*it).ordem<<"  "<<(*it).Porigem<<"   "<<(*it).Pfinal<<"   "<<(*it).prontas<<"      "<<(*it).enviadas-(*it).prontas<<"       "
                   <<(*it).quant-(*it).enviadas<<"     "<<(*it).hEntrada<<" "<<(*it).hInicio<<" "<<(*it).hFim<<endl;
             }
           }
           pthread_mutex_unlock(&mutexLista);
           cout<<endl;
           
           cout << ">>>Montagem<<<" <<endl;             
           cout << "N  " << "PB  " << "PC  " << "Prod  " << "EmProd  " << "Pend  " << "Ent  \t\t\t" << "Ini  \t\t\t" << "Fim" << endl;
           pthread_mutex_lock(&mutexLista);
           for(it=lista.begin(); it!=lista.end(); it++){
             if((*it).tipo=="montagem"){
               cout<<(*it).ordem<<"  "<<(*it).Pbaixo<<"   "<<(*it).Pcima<<"   "<<(*it).prontas<<"      "<<(*it).enviadas-(*it).prontas<<"       "
                   <<(*it).quant-(*it).enviadas<<"     "<<(*it).hEntrada<<" "<<(*it).hInicio<<" "<<(*it).hFim<<endl;
             }
           }
           pthread_mutex_unlock(&mutexLista);
           cout<<endl;
           
           cout << ">>>Descarga<<<" <<endl;             
           cout << "N  " << "TP  " << "Prod  " << "EmProd  " << "Pend  " << "Ent  \t\t\t" << "Ini  \t\t\t" << "Fim" << endl;
           pthread_mutex_lock(&mutexLista);
           for(it=lista.begin(); it!=lista.end(); it++){
             if((*it).tipo=="descarga"){
               cout<<(*it).ordem<<"  "<<(*it).tipoP<<"   "<<(*it).prontas<<"     "<<(*it).enviadas-(*it).prontas<<"       "
                   <<(*it).quant-(*it).enviadas<<"     "<<(*it).hEntrada<<" "<<(*it).hInicio<<" "<<(*it).hFim<<endl;
             }
           }
           pthread_mutex_unlock(&mutexLista);             
           cout<<endl;
         }
         else if(ent=="e"){
           time (&now);
           cout << "-----Estatistica das Maquinas-----" << endl;
           cout << ">>>Maquina A da celula 1<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTAc1 << "\t\t  " << (tempoTAc1/difftime(now,start))*100 << "%" << endl;
           
           cout << ">>>Maquina A da celula 2<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTAc2 << "\t\t  " << (tempoTAc2/difftime(now,start))*100 << "%" << endl;

           cout << ">>>Maquina A da celula 3<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTAc3 << "\t\t  " << (tempoTAc3/difftime(now,start))*100 << "%" << endl;

           cout << ">>>Maquina B da celula 1<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTBc1 << "\t\t  " << (tempoTBc1/difftime(now,start))*100 << "%" << endl;

           cout << ">>>Maquina C da celula 2<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTCc2 << "\t\t  " << (tempoTCc2/difftime(now,start))*100 << "%" << endl;

           cout << ">>>Maquina C da celula 3<<<" << endl;
           cout << "Tempo total\t" << "Percentagem" << endl;
           cout << tempoTCc3 << "\t\t  " << (tempoTCc3/difftime(now,start))*100 << "%" << endl;           
         }
         else if(ent=="x"){
           cout << "-----Pecas Carregadas-----" << endl;
           cout << "Numero de pecas que entraram na linha de producao: " << carregadas << endl;
         }
         else if(ent=="c"){
           cout<<"-----COMANDOS-----"<<endl<<">>Producao (p)"<<endl<<">>Montagem (m)"<<endl<<">>Descarga (d)"<<endl<<">>Lista de Ordens (l)" 
               <<endl<<">>Estatistica da Maquinas (e)"<<endl<<">>Pecas Carregadas (x)"<<endl<<">>Comandos (c)"<<endl<<">>exit"<<endl<<endl;
         }
         else if(ent=="exit"){
           cout << endl << "A encerrar...Ate breve!" <<endl <<endl;
           exit(-1);         
         }
    }
  }
}
