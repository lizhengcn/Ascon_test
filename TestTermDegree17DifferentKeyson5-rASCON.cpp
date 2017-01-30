/*
TestTermDegree17DifferentKeyson5-rASCON.cpp;
The program is run in Visual Studio 2012 with x64 platform Release;
The time is about one minute for 1000 tests.
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long int UINT64;
#define nrRounds 5
#define random(x) (rand())%x
#define ROL64(a, offset) ((offset != 0) ? ((((UINT64)a) << offset) ^ (((UINT64)a) >> (64-offset))) : a)
#define ExpTimes 1000//times of experiment


void AsconPermutation(UINT64 *state);
void AdditionofConstants(UINT64 *A,unsigned int indexround);
void SubstitutionLayer(UINT64 *A);
void LinearDiffusionLayer(UINT64 *A);

UINT64 AsconRoundConstants[12]={
0x0f00000000000000,0x8700000000000000,
0x4b00000000000000,0xc300000000000000,
0x2d00000000000000,0xa500000000000000,
0x6900000000000000,0xe100000000000000,
0x1e00000000000000,0x9600000000000000,
0x5a00000000000000,0xd200000000000000
};
void AsconPermutation(UINT64 *state){
    unsigned int i;
    for(i=0;i<nrRounds;i++){
        AdditionofConstants(state,i);
        SubstitutionLayer(state);
        LinearDiffusionLayer(state);
    }
}

void AdditionofConstants(UINT64 *A,unsigned int indexround){
    A[2]^=AsconRoundConstants[indexround];
}

void SubstitutionLayer(UINT64 *A){
    unsigned int i;
    UINT64 C[5];
    C[0] = A[4]&A[1] ^ A[3] ^ A[2]&A[1] ^ A[2] ^ A[1]&A[0] ^ A[1] ^ A[0];
    C[1] = A[4] ^ A[3]&A[2] ^ A[3]&A[1] ^ A[3] ^ A[2]&A[1] ^ A[2] ^ A[1] ^ A[0];
    C[2] = A[4]&A[3] ^ A[4] ^ A[2] ^ A[1] ^ 1;
    C[3] = A[4]&A[0] ^ A[4] ^ A[3]&A[0] ^ A[3] ^ A[2] ^ A[1] ^ A[0];
    C[4] = A[4]&A[1] ^ A[4] ^ A[3] ^ A[1]&A[0] ^ A[1];
    for(i=0;i<5;i++){
        A[i]=C[i];
    }
}

void LinearDiffusionLayer(UINT64 *A){
    unsigned int i;
    unsigned int offsets[5][2]={19,28,61,39,1,6,10,17,7,41};
    for(i=0;i<5;i++){
        A[i]=A[i]^ROL64(A[i],offsets[i][0])^ROL64(A[i],offsets[i][1]);
    }
}

int main(int argc,char *argv[]){

    FILE *f;
    f=fopen("1randcube-keys-output.txt","a");
    srand(time(NULL));
    UINT64 Key[2]={0};

    UINT64 InitialState[5]={0,0,0,0,0};
    UINT64 TempState[5]={0};
    UINT64 FinalState[2];

    unsigned int temp,counter;
    unsigned char flag;
    unsigned int index[16];

    UINT64 i,j,k,aa,nonzero[64];

    unsigned int var01[64];
    for(i=0;i<64;i++){
        var01[i]=0;
    }



    for(i=0;i<16;i++){
        index[i]=0;
    }
    i=0;
    while(i<16){
        temp=random(64);
        flag=0;
        for(j=0;j<i;j++){
            if(index[j]==temp){
                flag=1;
                break;
            }
        }
        if(flag==0){
            index[i]=temp;
            var01[temp]=1;
            i++;
        }
    }

    for(i=0;i<64;i++){
        if(var01[i]==1){
            fprintf(f,"1\t");
        }
        else{
            fprintf(f,"0\t");
        }
    }
    fprintf(f,"\n");

    for(i=0;i<64;i++){
        nonzero[i]=0;
    }

    for(aa=0;aa<ExpTimes;aa++){
        Key[0]=0;Key[1]=0;
        for(i=0;i<2;i++){
            for(j=0;j<64;j++){
               temp=random(2);
               if(temp){
                   Key[i]|=((UINT64)0x1<<j);
               }
            }
        }
        InitialState[0]=0x0000000060300201;
        InitialState[1]=Key[0];
        InitialState[2]=Key[1];
        InitialState[3]=0;
        InitialState[4]=0;
        for(i=0;i<2;i++){
            for(j=0;j<64;j++){
                temp=random(2);
                if(temp){
                    InitialState[3+i]=((UINT64)0x1<<j);
                }
            }
        }

        FinalState[0]=0;
        for(i=0;i<(UINT64(1)<<17);i++){
            for(j=0;j<5;j++){
                TempState[j]=InitialState[j];
            }
            for(j=0;j<16;j++){
                if(((i>>j)&1)){
                    TempState[3]|=(UINT64(1)<<index[j]);

                }
                else{
                    TempState[3]&=(~(UINT64(1)<<index[j]));
                }
            }
            if((i>>16)&1){
                TempState[4]|=(UINT64(1)<<index[0]);
            }
            else{
                TempState[4]&=(~(UINT64(1)<<index[0]));
            }
            AsconPermutation(TempState);
            FinalState[0]^=TempState[0];
        }
        fprintf(f,"%I64d\t",aa+1);
        for(i=0;i<64;i++){
            if(((FinalState[0]>>i)&1)!=0){
                nonzero[i]++;
            } 		
        }
        printf("%I64d:\nnonzero[i]: ",aa+1);
        fprintf(f,"%I64d:\nnonzero[i]:\t",aa+1);
        for(i=0;i<64;i++){
            fprintf(f,"%I64d\t",nonzero[i]);
            printf("%I64d ",nonzero[i]);
        }
        fprintf(f,"\n");
        printf("\n");

        printf("nonzero[i]/aa+1: ");
        fprintf(f,"nonzero[i]/aa+1:\t");
        for(i=0;i<64;i++){
            fprintf(f,"%f\t",float(nonzero[i])/float(aa+1));
            printf("%f\t",float(nonzero[i])/float(aa+1));
        }
        fprintf(f,"\n");
        printf("\n");
    }
    fclose(f);
    printf("over");
    getchar();
    return 0;
}