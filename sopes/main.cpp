#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#include<time.h>
#include<stdlib.h>

using namespace std;
string encabezado = "";
int jugador =0;
int posiciones[5]={0,0,0,0,0};
int intentos[5]={0,0,0,0,0};
int registro =0;
char *Memoria = NULL;
key_t Clave;
int Id_Memoria;
bool jugando = false;
int espera =0;
void menu();
void conectar();
void pedirpalabra();
void dibujar_tablero();
void dibujar_tablero_rival();
void verificar_palabra(string palabra);
void verificar_letra(int posicion);
bool verificar_posicion(int posicion);
void esperando();
void rendirse();
void verificar_ganar();

int main()
{
    conectar();

    return 0;
}

void conectar(){


    Clave = ftok ("/bin/ls", 48);
    if (Clave == -1)
    {
        cout << "Imposible conectar, vuelva a intentar en un momento " << endl;
        exit(0);
    }

    Id_Memoria = shmget (Clave, sizeof(char)*50, 0777 | IPC_CREAT);
    if (Id_Memoria == -1)
    {
        cout<< "Imposible conectar, vuelva a intentar en un momento"<<endl;
        exit(0);
    }

    Memoria =(char *) shmat (Id_Memoria, (char *)0, 0);
    if (Memoria == NULL)
    {
        cout << "Imposible conectar, vuelva a intentar en un momento" << endl;
        exit (0);
    }
    if(Memoria[1]=='1'){
        cout << "Ya existen 2 Jugadores" << endl;
       // exit (0);

    }else if(Memoria[0]=='1'){
        encabezado ="Jugador 2";
        Memoria[1]='1';
        Memoria[2]='1';
        jugador =2;
    }else{
        for(int i=0;i<51;i++){
            Memoria[i]='?';
        }
        Memoria[0]='1';
        Memoria[2]='1';
        jugador=1;
        encabezado="Jugador1";

    }

    cout << encabezado <<endl;
    pedirpalabra();
    shmdt ((char *)Memoria);
    shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);


}

void menu(){
int orden;
int pos=1;
int pos3=50;
if(jugador==2){
    pos=0;
    pos3=49;
}
string palabra;
int pos2;
while(jugando && Memoria[pos]=='1'&& Memoria[pos3]=='1'){
cout   << "Ingrese el numero de la orden que desea realizar"<< endl;
cout   << "1. Ingrese Palabra" << endl;
cout   << "2. Buscar Letra" << endl;
cout   << "3. Rendirse" << endl;

cin >> orden;
if(jugador==1){
    pos2='2';
}else{
    pos2='1';
}
while(Memoria[2]==pos2){
    esperando();
    sleep(1);
    continue;
}
verificar_ganar();

switch (orden) {
case 1:
    cout   << "Ingrese la palabra" << endl;
    cin >> palabra;
    verificar_palabra(palabra);
    break;
case 2:
    cout   << "Ingrese una posicion de 0 a 15" << endl;
    cin >> pos;
    if(pos>15){
        cout <<"Valor invalido"<<endl;
        menu();
        return;
    }
    verificar_letra(pos);
    break;

default:
    rendirse();
    break;
}
Memoria[2]=pos2;
pos=1;
pos3=50;
if(jugador==2){
   pos=0;
   pos3=49;
}
continue;
}

while(jugando && (Memoria[pos3]=='?'||Memoria[pos]=='?')){
    esperando();
    sleep(1);
}
menu();
return;
}

void pedirpalabra(){
    string Palabra;
    srand(time(NULL));
    int posicion=0;
    int letras=0;
    cout << "Ingrese una palabra de 4 o 5 letras" <<endl;
    cin >> Palabra;
    if(Palabra.length()>5 || Palabra.length()<4){
        cout << "Palabra Invalida" <<endl;
        pedirpalabra();
    }
    int i;
    bool bandera =false;
    while(letras!=Palabra.length()){
        if(jugador==1){
           posicion = 13+rand()%(29-13);
        } else{
            posicion = 29+rand()%(45-29);
        }
       //verificar si el numero ya existe el arreglo de posiciones
        bandera = false;
        for(i=0;i<5;i++){

            if(posiciones[i]==posicion){
               bandera = true;
            }
        }
        if(!bandera){
          posiciones[letras]=posicion;
          letras++;
        }
    }


    for(i=0;i<Palabra.length();i++){
        Memoria[posiciones[i]]=Palabra[i];
    }

    if(jugador==1){

        for(i=3;i<(3+Palabra.length());i++){
            Memoria[i]=Palabra[i-3];
        }

    }else{
        for(i=8;i<(8+Palabra.length());i++){
            Memoria[i]=Palabra[i-8];
        }

    }

    jugando = true;
    if(jugador==1){
       Memoria[49]='1';
    }else{
        Memoria[50]='1';
    }
    dibujar_tablero();
    menu();
}

void dibujar_tablero(){

    if(jugador==1){
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                cout <<"|"<<Memoria[(i*4)+(j+13)]<<"|";
            }
            cout<<" "<<endl;
        }

    }else{
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                cout <<"|"<<Memoria[(i*4)+(j+29)]<<"|";
            }
            cout<<" "<<endl;
        }

    }

}

void dibujar_tablero_rival(){
    char valor ='*';
    if(jugador==2){
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                valor = Memoria[(i*4)+(j+13)];
                if(!verificar_posicion((i*4)+(j+13))){
                    if(valor!='*'){
                    valor='?';}
                }
                cout <<"|"<<valor<<"|";
            }
            cout<<" "<<endl;
        }

    }else{
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                valor = Memoria[(i*4)+(j+29)];
                if(!verificar_posicion((i*4)+(j+29))){
                    if(valor!='*'){
                    valor='?';}
                }
                cout <<"|"<<valor<<"|";
            }
            cout<<" "<<endl;
        }

    }

}

void verificar_letra(int posicion){
bool bandera = false;
    if(jugador==2){
        posicion=13+posicion;
    }else{
        posicion = 29+posicion;
    }
    if(Memoria[posicion]!='?'&&Memoria[posicion]!='*'){


            if(intentos[0]==posicion||intentos[1]==posicion||intentos[2]==posicion||intentos[3]==posicion||intentos[4]==posicion){
                bandera==true;
            }

        if(!bandera){
           intentos[registro]=posicion;
           registro++;
        }
    }else if(Memoria[posicion]=='?') {
         Memoria[posicion]='*';
    }
    dibujar_tablero_rival();

}

bool verificar_posicion(int posicion){

    for(int i=0;i<5;i++){
        if(intentos[i]==posicion)
            return true;
    }

    return false;
}

void esperando(){
    if(espera==3){
        espera =0;
    } else{espera++;}

    switch (espera) {
    case 0:
        cout << "|"<<endl;
        break;
    case 1:
        cout<< "/"<<endl;
        break;
    case 2:
        cout<< "-"<<endl;
        break;
    case 3:
        cout<< "\\"<<endl;
        break;

    default:
        break;
    }
    cout<<"\e[A";
    //cout<<"\e[A";

    //
}

void rendirse(){
    for(int i=0;i<45;i++){
      Memoria[i]='?';
    }
    if(jugador==2){
      Memoria[48]='1';

    }else{
     Memoria[47]='1';
   }

    exit(0);

}

void verificar_ganar(){
    char val;
    if(jugador==1){
        if(Memoria[46]=='1'){
            cout << "El jugador 2 ha ganado"<<endl;
            Memoria[2]='1';
            for(int i=3;i<51;i++){
              Memoria[i]='?';
            }
            sleep(1.5);
            system("clear");
            cout << encabezado << endl;
            pedirpalabra();

        }else if(Memoria[48]=='1'){
            system("clear");
            cout << "El jugador 2 ha abandonado la partida"<<endl;
            cout <<"Felicidades ha ganado la partida"<<endl;
            shmdt ((char *)Memoria);
            shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
            exit(0);
        }



    }else{
        if(Memoria[45]=='1'){
            cout << "El jugador 1 ha ganado"<<endl;
            Memoria[2]='1';
            for(int i=3;i<51;i++){
              Memoria[i]='?';
            }
            sleep(1);
            system("clear");
            cout << encabezado << endl;
            pedirpalabra();

        }else if(Memoria[47]=='1'){
            system("clear");
            cout << "El jugador 1 ha abandonado la partida"<<endl;
            cout <<"Felicidades ha ganado la partida"<<endl;

            shmdt ((char *)Memoria);
            shmctl (Id_Memoria, IPC_RMID, (struct shmid_ds *)NULL);
            exit(0);
        }


    }



}

void verificar_palabra(string palabra){
    int contador=0;
    if(palabra.length()>5||palabra.length()<4){
        cout << "Palabra invalida"<<endl;
        cout   << "Ingrese la palabra" << endl;
        cin >> palabra;
        verificar_palabra(palabra);
    }else{
    int i=3;
    if (jugador==1){
        i=8;
    }

    if(Memoria[i+4]=='?'&&palabra.length()==5){
        cout << "Palabra incorrecta" << endl;
        return;
    }else if(Memoria[i+4]!='?'&&palabra.length()==4){
        cout << "Palabra incorrecta" << endl;
        return;
    }

    for(int j=i;j<i+palabra.length();j++){
        char val1 =palabra[j-i];
        char val2 = Memoria[j];
        if(palabra[j-i]!=Memoria[j])
            contador=1;
    }

    if(contador==0){
         cout << "Felicidades ha ganado" <<endl;
         sleep(1.5);
        if(jugador=1){
           Memoria[45]='1';
           Memoria[50]='?';
           system("clear");
           cout<< encabezado<<endl;
           Memoria[2]='2';
           pedirpalabra();


        }else{
             Memoria[46]='1';
             Memoria[45]='1';
             Memoria[50]='?';
             system("clear");
             cout<< encabezado<<endl;
             Memoria[2]='1';
             pedirpalabra();

        }

    }else{
        cout << "Palabra incorrecta" << endl;
        return;
    }

  }
}




