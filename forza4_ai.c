/* 
 * File:   forza4_ai.c
 * Author: Franco Piras
 * Created on 30 gennaio 2017, 19.25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Variabili matrice campo di gioco */

const int MR = 9 ;           /* numero colonne matrice*/
const int MC = 10 ;           /* numero righe matrice*/

/* Variabili di gioco */

const char G1 = 'X' ;        /* Simbolo usato dal giocatore 1 nel gioco */
const char G2 = 'O' ;        /* Simbolo usato dal giocatore 1 nel gioco o dal computer */ 
const char NOTHING = ' ' ;   /* Simbolo per riempire il campo di gioco ad inizio partita */
const char WIN = '#'  ;      /* La sequenza che determina la vittoria verra' sovrascritta con questo simbolo */
const int  GVSG = 1 ;        /* Selezione di gioco min */
const int  GVSCPU = 3 ;      /* Selezione di gioco max */

/* Funzione di pulizia del terminale.
   E' costruita con un IF per consentirne l'uso sia con su terminali Windows che Unix */

#ifdef WIN32
#define CLEARSCREEN ;system("cls")
#else
#define CLEARSCREEN ;system("clear")
#endif

/* Prototipi procedure */

void titolo(void) ;
void game(char player1[] ,char player2[] , char check[][(MR*MC)+1] , int select) ;
void game_end(char matrix[MR][MC] , char player[40] , int seq[(MR*MC)+1] , int turno ,  int m , int f , int mode) ;
void stampa_matrix(char matrix[MR][MC]) ;
void memory(int seq[(MR*MC)+1] , int m) ;
void read(char check[200][(MR*MC)+1]) ;
int player_mode(char matrix[MR][MC] , char player[] , char ps) ;
int pc_mode(char matrix[MR][MC] , char check[200][(MR*MC)+1] , int seq[(MR*MC)+1] , int m) ;
int mossa(char matrix[MR][MC] , int s , int r) ;
int check_win(char matrix[MR][MC] , char ps) ;
int check_ai(char matrix[MR][MC] , char s) ;


/* ############################ Root ############################ */

int main() 
{
    
    int select , err ;
    char player1[40] , player2[40] , r , check[200][(MR*MC)+1] ;
    
    titolo() ;
    printf ("Benvenuto/i giocatore/i!\n") ; 
    printf ("Questa è una piccola app per giocare al nostro gioco preferito!") ;
    
    do
    {
        printf ("\n\n\nPremere 1 se volete giocare in due.\n") ;
        printf ("Premere 2 se vuol tentar la sorte e provare a sfidarmi!\n") ;
        printf ("Premere 3 se hai cambiato idea e vuoi chiudere il gioco\n\n") ;
        printf ("Scelta: ") ;
        err = scanf  ("%d" , &select) ; // Inserimento selezione modalità di gioco
        
		
        while (err==0 || select<GVSG || select>GVSCPU)  // Verifico se la selezione è nel range consetito
        {
            getchar() ; // pulisco input precedente
            (err==0)? printf("\n\nCarattere non consentito! Ritenta:"):printf ("Selezione errata! Ritenta: ") ;
            err = scanf  ("%d" , &select) ;
        }
	
        getchar() ; // Intercetto il "\n" dato in selezione e non pulito da "scanf" per poter usare correttamente le "fgets"	
        switch(select) // Inserimento nomi giocatori 
        {
            case 1: // 1 VS 1 
                    printf ("\nGiocatore 1 scegli il tuo nome nel gioco: ") ;
					
                    // Fgets(), consente di acquisire una stringa senza spazi e di prevenire eventuali buffer overflow rispetto a "scanf"
					
                    fgets(player1, sizeof(player1), stdin) ; 
                    printf ("Giocatore 2 scegli il tuo nome nel gioco: ") ;
                    fgets(player2, sizeof(player2), stdin) ;
                    break ;
            case 2: // 1 VS Computer 
                    printf ("\nScegli il tuo nome nel gioco: ") ;
                    fgets(player1, sizeof(player1), stdin) ;
                    strcpy(player2,"cpu"); // Rinomino il nome del secondo giocare con "CPU"
                    read(check) ;
                    break ;
            case 3:
                //read(check);
                return (EXIT_SUCCESS);                
        }
        
        game(player1 , player2 , check , select) ; // Avvio la sessione di gioco
            
        printf("\n\nFacciamo una nuova partita? (s/n)"); // Chiedo all'utente se vuole fare un'altra partita prima di terminare il programma
        scanf (" %c", &r) ;
        getchar() ;
        CLEARSCREEN ;
      }while(r =='s'|| r == 'S'); // Il programma termina se seleziono qualsiasi cosa che non sia "s" o "S"
    
    return (EXIT_SUCCESS);
}

/* ######################## Procedure di gestione della schermata di gioco ###################### */

/* Gestore della sessione di gioco */

void game(char player1[] , char player2[] , char check[][(MR*MC)+1] , int mode)
{
    int s , m=0 , turno=2 , f=0 , i , j , seq[(MR*MC)+1] ; 
    char ps , player[40] , matrix[MR][MC] ;
    
    for (i=0; i<MR; i++) 
    {  
        for (j=0; j<MC; j++)
        {
           matrix[i][j]= NOTHING ; // Inizializzo matrice con carattere neutro
        }
    }
    
    // Effettuo un ciclo do-while così effettuare almeno un'esecuzione
	
    
    do
    {
        CLEARSCREEN ; // Pulisco la schermata ad ogni cambio turno
        titolo() ;    // Stampo titolo
        printf ("Mosse effettuate finora: %d\n\n" , m) ; // Conteggio mosse effettuate e quindi cambi turno. 
        m++ ;
        
        memset(player,0,40) ;  // Resetto l'array della stringa player nel caso in cui il successivo nome copiato sia più corto
        
        // Copio la stringa contentente il nome del giocatore di turno su una variabile univoca per le successive chiamate
        // E cambio turno di gioco e simbolo da scrivere sulla matrice
	
        if (turno==2)
	{
            for (i=0 ; player1[i]!='\0' && player1[i]!='\n' ; i++) player[i] = player1[i] ;
            turno-- ;
            ps= G1 ;
	}
	else
	{
            for (i=0 ; player2[i]!='\0' && player2[i]!='\n' ; i++) player[i] = player2[i] ;
            turno++ ;
            ps= G2 ;
        }
        
        switch(mode) // Gestore modalità di gioco 
        {
            case 1: // 1 VS 1
                printf ("E' il tuo turno %s\n\n\n" , player) ;
                stampa_matrix(matrix) ;
                seq[m-1] = (player_mode(matrix , player , ps) +1) ;  // chiede una mossa al giocatore e la salva nell'array delle sequenze di gioco              
                break ;
            case 2:  // 1 VS Computer
                if (turno==1)
                {
                    printf ("E' il tuo turno %s\n\n\n" , player) ;
                    stampa_matrix(matrix) ;
                    if(m>=3)
                    {
                        printf ("\nHo inserito un gettone nella colonna %d\n\n" , seq[m-2]) ;
                    }
                    seq[m-1] = (player_mode(matrix , player , ps) +1) ;
                }
                else
                {
                    seq[m-1] = (pc_mode(matrix , check , seq , m)+1) ; // Salva nell'array delle sequenze di gioco la mossa della cpu
                }
                break ;
        }
        if (check_win(matrix , ps)) // Verifica vittoria giocatore a seguito scelta mossa
        {
            f=1 ; // Code case vittoria
            seq[MR*MC] = turno ; // Salvo nell'ultima cella della riga delle matrice chi ha vinto questa volta
        }

        if (f==0 && m>(MR*MC)) // Check matrice piena, prima di proseguire, basandosi sul numero di mosse effettuate.
        {
            f=2; // Code case patta
            seq[MR*MC] = 3 ; // Se patta salvo 3
        }
    }while(f==0);
    game_end(matrix , player , seq , turno , m , f , mode) ;
}

/* Gestione schemarta di fine partita e del salvataggio della sequenza di gioco */

void game_end(char matrix[MR][MC] , char player[40] , int seq[(MR*MC)+1] , int turno ,  int m , int f , int mode)
{
    memory(seq , m) ;
    
    CLEARSCREEN ;
    titolo() ; 
    printf ("Mosse effettuate per vincere: %d\n\n" , m) ;  // Stampo il numero di mosse effettuate
    stampa_matrix(matrix);
                
    switch(f)
    {
        case 1: // Vittoria di un giocatore o della cpu
            if (mode==2 && turno==2)
            {
                printf ("Ho vinto io, umano!") ;
            }
            else if (mode==2 && turno==1)
            {
                printf ("Complimenti! Hai vinto %s" , player) ;
            }
            else
            {
                printf ("Complimenti! Ha vinto %s" , player) ;
            }
            break ;
        case 2: // Patta
            printf ("Ohibò! Abbiamo pareggiato!") ;
            break;
    }
}

/* Titolo del gioco */

void titolo()
{
    printf (" #############################################\n") ;
    printf (" ################## FORZA 4 ##################\n") ;
    printf (" #############################################\n\n\n") ;
}

/* Stampa la matrice di gioco */

void stampa_matrix(char matrix[MR][MC])
{
    int i , j ;
    
    for(j=0 ;j<MC ; j++) j<9? printf(" ---"):printf(" ----"); // riga piena all'inizio della matrice. Si allarga con numeri a cifra doppia.
    printf("\n");
    
    /* stampo parte centrale della matrice */
    
    for (i=0; i<MR ; i++) // salto alla riga succesiva della matrice dopo aver popolato la precedente nel for annidato
    {
        for (j=0; j<MC ; j++) // popolo riga matrice in orizzontale
        {
            j<9? printf ("| %c " , matrix [i][j]):printf ("| %c  " , matrix [i][j]) ;
        }
        printf ("|\n") ; // a capo, ad ogni riga della matrice + riga verticale della tabella
        for(j=0 ; j<MC ; j++) j<9? printf("'---"):printf("'----"); // separatore tra una riga e l'altra.
        printf("'\n");
    }
    
    for(j=0; j<MC ; j++) printf("| %d " , j+1) ; // intestazione colonne
    printf("|\n");
    for(j=0; j<MC; j++) j<9? printf("'---"):printf("'----");  // riga piena alla fine della matrice
    printf("'\n");    
    printf ("\n\n\n\n") ;
}

/* ######################## Procedure di controllo delle routine di gioco ###################### */



/* Verifica l'eventuale vittoria di uno dei giocatori e modifica
   l'output della sequenza vincente con un simbolo adeguato      */

int check_win(char matrix[MR][MC] , char ps)
{
    int i , j , z, a;
    
    // Controllo righe matrice
    
    for (i=MR-1 , z=0 ; z<MR ; i-- , z++)
    {
        for (j=0 ; j<MC-3 ; j++)
        {
            if (matrix[i][j]==ps && matrix[i][j+1]==ps && matrix[i][j+2]==ps && matrix[i][j+3]==ps)
            {
                matrix[i][j]= WIN ;  // Cambio sequenza vincente con un simbolo distintivo
                matrix[i][j+1]= WIN ;
                matrix[i][j+2]= WIN ;
                matrix[i][j+3]= WIN ;
                return 1 ;   
            }
        }
    }
    
        // Controllo colonne matrice
    
    for (j=0 ; j<MC ; j++)
    {
        for (i=MR-1 ; i>2 ; i--)
        {
            if (matrix[i][j]==ps && matrix[i-1][j]==ps && matrix[i-2][j]==ps && matrix[i-3][j]==ps)
            {
                matrix[i][j]= WIN ; // Cambio sequenza vincente con un simbolo distintivo
                matrix[i-1][j]= WIN ;
                matrix[i-2][j]= WIN ;
                matrix[i-3][j]= WIN ;
                return 1 ;   
            }
        }
    }

    // Controllo diagonali matrice
    
    for (i=MR-1 ; i>3 ; i--)
    {
        // Verso destra /
        
        for (j=0 ; j<MC-3 ; j++)
        {
            if (matrix[i][j]==ps && matrix[i+1][j+1]==ps && matrix[i+2][j+2]==ps && matrix[i+3][j+3]==ps)
            {
                matrix[i][j]= WIN ;  // Cambio sequenza vincente con un simbolo distintivo
                matrix[i+1][j+1]= WIN ;
                matrix[i+2][j+2]= WIN ;
                matrix[i+3][j+3]= WIN ;
                return 1 ;   
            }
        }
        
        // Verso sinistra \
        
        for (j=MC-1 ; j>2 ; j--)
        {
            if (matrix[i][j]==ps && matrix[i+1][j-1]==ps && matrix[i+2][j-2]==ps && matrix[i+3][j-3]==ps)
            {
                matrix[i][j]= WIN ;  // Cambio sequenza vincente con un simbolo distintivo
                matrix[i+1][j-1]= WIN ;
                matrix[i+2][j-2]= WIN ;
                matrix[i+3][j-3]= WIN ;
                return 1 ;   
            }
        }        
    }
    return 0 ;
}

/* Si occupa di far scegliere al giocatore la sua mossa e verifica errori di input */

int player_mode(char matrix[MR][MC] , char player[] , char ps)
{
    int s , err , r= MR-1;

    printf ("%s inserisci una pedina, selezionando il numero della colonna che desideri: " , player);
    err= scanf  ("%d" , &s ) ; // Scelta mossa da effettuare. 
                               // Assegno valore di scanf alla variabile err per il controllo caratteri non consentiti
    
    while (err==0 || s<1 || s>MC) // Check scelta colonna fuori dal range della matrice
    {
        getchar();
        (err==0)? printf("\nCarattere non consentito! Ritenta:"):printf ("\nSelezione errata, riprovare: ") ;
        err = scanf  ("%d" , &s) ;
    }
    while (err==0 || mossa(matrix , s-1 , r)==(MC+1)) // controllo colonna scelta piena
    {
        getchar() ;
        if(err==0)
        {
            printf("\nCarattere non consentito! Ritenta:") ;
        }
        else if (s<1 || s>MC)
        {
            printf ("\nSelezione errata, riprovare: ") ;
        }
        else
        {
            printf ("\nColonna piena, sceglierne un'altra: ") ;  
        }
        err = scanf  ("%d" , &s) ;
    }
    
    s= s-1 ;
    matrix[mossa(matrix , s , r)][s] = ps ;
    return (s) ;
}

/* Verifica, in base alla colonna scelta se questa è piena o fuori range e, in caso non lo sia,
   restituisce la prima riga libera di quella colonna */

int mossa(char matrix[MR][MC] , int s , int r)
{    
    if(matrix[0][s]!=NOTHING) //Verifico da subito se la colonna scelta è piena o fuori range
    {
        r = (MC+1) ;
        return (r) ;
    }
    else if (matrix[r][s]!=NOTHING) // Se non è piena, cerco la prima riga vuota da fornire con una funzione ricorsiva
    {
        mossa(matrix , s , r-1) ;
    }
    else
    {
        return (r) ; // valore della prima riga della colonna scelta
    }
}


/* #########################  Gestione AI del computer ############################*/



/* Gestisce tutta l'intelligenza artificiale sulla quale il computer si basa per scegliere la sua mossa */

int pc_mode(char matrix[MR][MC] , char check[200][(MR*MC)+1] , int seq[(MR*MC)+1] , int m)
{
    int i , j , w, s , r= MR-1 , l= MR*MC , y=l+2;
    time_t t ; // istanzio una variabile time_t (valore temporale predefinito dal sistema)
    
    srand((unsigned) time(&t)); // inizializzo la base da cui rand() creerà numeri casuali con il time del pc usato dal programma
    
    if (check_ai(matrix , G2)!=(MC+1)) // Vedo se posso vincere
    {
        s = check_ai(matrix , G2); // Se posso vincere, istanzio s con la colonna suggerita da check_ai 
    }
    else if (check_ai(matrix , G1)!=(MC+1))  // Vedo se sto per perdere
    {
        s = check_ai(matrix , G1); // Se sto per perdere, istanzio s con la colonna suggerita da check_ai
    }
    else  // faccio usare al computer le partite già svolte come una sorta di 'esperienza'
    {
        for(i=0 ; y!=m-1 && i<200 ; i++) // confronto ogni riga della memoria in cerca di una corrispondenza
        {
            for(j=0 ; y!=m-1 && j<l ; j++)
            {
                if(check[i][j] == (seq[j]+'0'))
                {
                    if(y==l+2)
                    {
                        y=0;
                    }
                    y++ ;
                }
                else
                {
                    break ;
                }
            }            
        }
       
        if(y!=0) // se la trovo, uso la mossa usata in precedenza, se aveva vinto la cpu o anticipo quella avversaria, nel caso opposto
        {
            w = check[i-1][l] ;
            switch(w)
            {
                case 1:
                   s= (check[i-1][m] -1) ;                   
                   break;
                case 2:
                   s= (check[i-1][m-1] -1);
                   break;
                case 3:
                   break;
            }
            while (mossa(matrix , s , r)==(MC+1))
            {
                s= rand() % MC ; // % MC limita il range di rand() alle sole colonne della matrice
            }
        }
        else
        {
            do
            {    
                s= rand() % MC ; // % MC limita il range di rand() alle sole colonne della matrice
            }while (mossa(matrix , s , r)==(MC+1));
        }
    }
    
    matrix[mossa(matrix , s , r)][s] = G2 ; 
    return (s) ; // mossa del computer
}


/* Verifica le possibilità di vincita/perdita del computer per le combinazioni di 3 caselle piene ed una vuota */

int check_ai(char matrix[MR][MC] , char s)
{
    int i , j , z , a , b , r=MR-1 , w ;
    char three[16] = {NOTHING,s,s,s,s,NOTHING,s,s,s,s,NOTHING,s,s,s,s,NOTHING} ;  // array possibili sequenze vincenti
    
        // Controllo diagonali matrice
    
    for (i=MR-1 ; i>2 ; i--)
    {
        // Verso destra /
        
        for (j=0 ; j<MC-3 ; j++)
        {
            for (a=0 , b=0 ; b<17 ; a++ , b=(b+4)) // scorro l'array delle possibili posizioni dello spazio vuoto su 4 elementi
            {
                if (matrix[i][j]==three[b] && matrix[i-1][j+1]==three[b+1] && matrix[i-2][j+2]==three[b+2] && matrix[i-3][j+3]==three[b+3])
                {
                    if(mossa(matrix , j+a , r)==(i-a)) // verifico che la posizione colonna individuata permetta 
                    {                                  // un inserimento nella cella intercettata,
                        w =j+a ;                       // altrimenti potrei dare un assist all'avversario
                        return (w) ;
                    }
                }
            }
        }
        
        // Verso sinistra \
        
        for (j=MC-1 ; j>2 ; j--)
        {
            for (a=0 , b=0 ; b<17 ; a++ , b=(b+4))
            {    
                if (matrix[i][j]==three[b] && matrix[i-1][j-1]==three[b+1] && matrix[i-2][j-2]==three[b+2] && matrix[i-3][j-3]==three[b+3])
                {
                    if(mossa(matrix , j-a , r)==(i-a))
                    {
                        w =j-a ;
                        return (w) ;
                    }  
                }
            }
        }        
    }

    // Controllo righe matrice
    
    for (i=MR-1 , z=0 ; z<MR ; i-- , z++)
    {
        for (j=0 ; j<MC-3 ; j++)
        {
            for (a=0 , b=0 ; b<17 ; a++ , b=(b+4))
            {
                if (matrix[i][j]==three[b] && matrix[i][j+1]==three[b+1] && matrix[i][j+2]==three[b+2] && matrix[i][j+3]==three[b+3])
                {
                    if(mossa(matrix , j+a , r)==i)
                    {
                        w = j+a ;
                        return (w) ;
                    }
                }
            }
        }
    }
    

    // Controllo colonne matrice. Verifico solo la sequenza s,s,s,NOTHING
    
    for (j=0 ; j<MC ; j++)
    {
        for (i=MR-1 ; i>2 ; i--)  // qui non devo scorrere l'array sequenze, avendone una sola possibile.
        {
            if (matrix[i][j]==three[12] && matrix[i-1][j]==three[13] && matrix[i-2][j]==three[14] && matrix[i-3][j]==three[15])
            {
                w = j ;
                return (w) ;
            }
        }
    }
    
    w = (MC+1) ;
    return (w) ;
}


/* Legge e scrive la sequenza di mosse fatte per ogni partita e chi ha vinto (1=Giocatore1 , 2=Giocatore2 , 3=Patta).
   Chi ha vinto è scritto nell'ultima posizione, dopo uno spazio.
   Con la variabile read_write, chiedo con '1' la scrittura del file e con 2 la lettura.
   In lettura, verrà popolata una matrice per i confronti con la sequenza presente in realtime.
   Questa è la memoria del gioco delle partite già fatte.
   Possiamo chiamarla "esperienza base della macchina". */

void memory(int seq[(MR*MC)+1] , int m) 
{
    FILE *mem ;
    int i , l= MR*MC ;
    
    mem=fopen("memory.txt", "a"); //apro file in scrittura in append mode

    if( mem==NULL ) // Intercetto eventuale errore in apertura del file
    {
        exit ;
    }
    else
    {
        fprintf (mem , "%d " , m) ;
        for (i=0 ; i<m ; i++)
        {
            fprintf (mem , "%d" , seq[i]) ;
        }
        fprintf (mem , " %d\n" , seq[l]) ;
        fclose(mem);
    }
    
}
    
void read(char check[200][(MR*MC)+1])    
{
    FILE *mem ;
    int i , j , err , l = (MR*MC) , waste ;
    char riga[l+3] , temp , string[l];
    
    for (i=0; i<200; i++) 
    {  
        for (j=0; j<l+1; j++)
        {
            check[i][j]= NOTHING ; // Inizializzo matrice con carattere neutro
        }
    }
    
    mem=fopen("memory.txt", "r") ; //apro file in lettura
    
    if( mem==NULL ) // Intercetto eventuale errore in apertura del file
    {
        exit ;
    }
    else
    {
        i=0 ;
        while(fgets(riga, (l+4), mem)!=NULL)
        {
            sscanf(riga, "%d %s %c", &waste, string , &temp) ;
            check[i][l] = temp ;
            for(j=0 ; j<waste ; j++)
            {
                check[i][j] = string[j] ;
            }
            i++ ;
        }
        fclose(mem) ;
    }
}