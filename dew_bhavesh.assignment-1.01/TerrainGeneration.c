#include<stdio.h>
#include<time.h>
#include<string.h>
#include<stdlib.h>

#define Path '#'
#define PokeMart 'M'
#define PokeCenter 'C'
#define TallGrass ':'
#define Water '~'
#define Boulder '%'
#define Clearing '.'
#define Tree '^'



int Width = 80;
int Height = 21;
char Map[24][80];

void printMap(){
    for (int i = 0; i < Height; i++) {
        for (int j = 0; j < Width; j++) {
            printf("%c", Map[i][j]);
            if((j+1) % Width == 0){
                printf("\n");
            }
        }
    }    
}

void createMap(){
    for(int i = 0; i < Height; i++){
        for(int j = 0; j < Width; j++){
            if(i == 0 || i == Height-1){
                Map[i][j] = Boulder;
            }else if(j == 0 || j == Width-1){
                Map[i][j] = Boulder;
            }else{
                Map[i][j] = ' ';
            }
        }
    }
    

}

void createPaths(){
    int NExit = rand()%40+40;
    if(NExit <= 5){
        NExit+=5;
    }else if(NExit >75){
        NExit -= 5;
    }
    int WExit = rand()%10+10;
    if(WExit <= 5){
        WExit += 5;
    }else if(WExit >17){
        WExit-= 5;
    }
    int NE = NExit;
    int WE = WExit;

    Map[0][NE] = Path;
    Map[WE][0] = Path;

    for(int i = 0; i<NE; i++){
        Map[WE][i] = Path;
    }
    for(int i = 0; i<=WE; i++){
        Map[i][NE] = Path;
    }

    int SExit = rand()%40;
    if(SExit <=5){
        SExit+=5;
    }else if(SExit >75){
        SExit-=5;
    }
    int SE = SExit;
    int EExit = rand()%10;
        if(EExit < 5){
        EExit +=5;
    }else if(EExit > 17){
        EExit-= 5;
    }
    int EE = EExit;
    Map[20][SE] = Path;
    Map[EE][79] = Path;


    for(int i = SE; i<80; i++){
        Map[EE][i] = Path;
    }
    for(int i = EE; i<21; i++){
        Map[i][SE] = Path;
    }
    Map[EE-1][SE] = PokeCenter;
    Map[EE-2][SE] = PokeCenter;
    Map[EE-1][SE+1] = PokeCenter;
    Map[EE-2][SE+1] = PokeCenter;
    Map[WE+1][NE] = PokeMart;
    Map[WE+2][NE] = PokeMart;
    Map[WE+1][NE+1] = PokeMart;
    Map[WE+2][NE+1] = PokeMart;
}

void addGrass(){
    int Xseek = rand()%75+2;
    int Yseek = rand()%17+2;
    int Xgrass = rand()%30 + 3;
    int Ygrass = rand()%10 + 3;
    for(int i=0; i< Ygrass; i++){
        for(int j=0; j<Xgrass; j++){
            if(Map[Yseek + i][Xseek + j] == ' '){
                Map[Yseek + i][Xseek + j] = TallGrass;
            }else{
                continue;
            }            
        }
    }    
}
void createGrass(){
    int grass_Number = rand()%10 + 4;
    for(int i = 0 ; i < grass_Number; i++){
        addGrass();
    }
    
}

void createClearing(){
    for(int i = 0; i< 21 ; i++){
        for(int j = 0; j<80; j++){
            if(Map[i][j] == ' '){
                Map[i][j] = Clearing;
            }else{
                continue;
            }
        }
    }
}

void addTree(){
    int Xseek = rand()%78+1;
    int Yseek = rand()%19+1;
    if(Map[Yseek][Xseek] != Path && Map[Yseek][Xseek] != PokeMart && Map[Yseek][Xseek] != PokeCenter){
        Map[Yseek][Xseek] = Tree;
    }else{
        addTree();
    }
}

void createTree(){
    int tree_Number = rand()%50 + 20;
    for(int i = 0 ; i< tree_Number ;i++){
        addTree();
    }
}

void addRock(){
        int Xseek = rand()%78+1;
    int Yseek = rand()%19+1;
    if(Map[Yseek][Xseek] != Path && Map[Yseek][Xseek] != PokeMart && Map[Yseek][Xseek] != PokeCenter && Map[Yseek][Xseek] != Tree){
        Map[Yseek][Xseek] = Boulder;
    }else{
        addRock();
    }
}
void createRock(){
    int Rock_Number = rand()%60 + 10;
    for(int i = 0 ; i< Rock_Number ;i++){
        addRock();
    }
}

void addWater(){
        int Xseek = rand()%78+1;
    int Yseek = rand()%19+1;
    if(Map[Yseek][Xseek] != Path && Map[Yseek][Xseek] != PokeMart && Map[Yseek][Xseek] != PokeCenter && Map[Yseek][Xseek] != Tree){
        Map[Yseek][Xseek] = Water;
    }else{
        addWater();
    }
}
void createWater(){
    int Water_Number = rand()%40 + 10;
    for(int i = 0 ; i< Water_Number ;i++){
        addWater();
    }
}

int main(){
    srand(time(NULL));
    createMap();
    createPaths();
    createGrass();
    createTree();
    createRock();
    createWater();
    createClearing();
    printMap();
    return 0;
}

