#include<stdio.h>
#include <stdbool.h>

#define N 5
int count = 0;

bool isOkToMove(int x, int y, int board[N][N]){

    if(board[x][y] == -1 && y >= 0 && y < N && x >= 0 && x < N) {
        return true;
    }else{
        return false;
    }

}

void printSol(int sol[N][N], int ref[N][N]){
    
    printf("\n");
    int flag = 1;
    while(flag <= N*N) {
        for(int x = 0; x < N; x++){
            for(int y = 0; y < N; y++){
                if(sol[x][y] == flag) {
                    printf("%d", ref[x][y]);
                    flag++;
                    if(flag <= 25) {
                        printf(",");
                    }
                }
            }
        }
    }
    //count++;
    
}
void findTours(int visited[N][N], int currentX, int currentY, int index, int ref[N][N]) {
    
    visited[currentX][currentY] = index;
    
    if(index >= N*N) {
        printSol(visited, ref);
        visited[currentX][currentY] = -1;
    }
    
    int x[8] = {2, 1, -1, -2, -2, -1,  1,  2};
    int y[8] = {1, 2,  2,  1, -1, -2, -2, -1};
    
    for(int i = 0; i < 8; i++){
        int try_X = currentX + x[i];
        int try_Y = currentY + y[i];
        
        if(isOkToMove(try_X, try_Y, visited))
        {
            findTours(visited, try_X, try_Y, index + 1, ref);
        }
    }
    visited[currentX][currentY] = -1;
}

int main(){
    int visited[N][N];
    for(int x = 0; x < N; x++){
        for(int y = 0; y < N; y++){
            visited[x][y] = -1;
        }
    }
    int ref[N][N];
    int index = 1;
    for(int x = 0; x < N; x++){
        for(int y = 0; y < N; y++){
            ref[x][y] = index;
            index++;
        }
    }
    for(int x = 0; x < N; x++) {
        for(int y = 0; y < N; y++) {
            findTours(visited, x, y, 1, ref);
        }
    }
    printf("\n");
    //printf("%d",count);
    return 0;
}