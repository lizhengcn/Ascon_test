/*
TestTermsDegree33on6-rASCON.cpp;
The program is run in Visual Studio 2012 with x64 platform Release.
The time is about half an hour to calculate a 33-dimension cube sum, so to test 1000 cubes,
we parallel this program in 20 CPUs and get the test result in about 25 hours.
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long int UINT64;
#define nrRounds 6
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
    srand(time(NULL));
    UINT64 Key[2]={0};

    UINT64 InitialState[5]={0,0,0,0,0};
    UINT64 TempState[5]={0};
    UINT64 FinalState[2];

    unsigned int temp,counter;
    unsigned char flag;
    unsigned int index[16];

    UINT64 i,j,k,aa,nonzero;

    nonzero=0;
    for(aa=0;aa<ExpTimes;aa++){
        for(i=0;i<32;i++){
            index[i]=0;
        }
        i=0;
        while(i<32){
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
                i++;
            }
        }
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
                InitialState[3+i]|=((UINT64)0x1<<j);
               }
            }
        }
        FinalState[0]=0;
        for(i=0;i<(UINT64(1)<<33);i++){
            for(j=0;j<5;j++){
                TempState[j]=InitialState[j];
            }
            for(j=0;j<32;j++){
                if(((i>>j)&1)){
                    TempState[3]|=(UINT64(1)<<index[j]);

                }
                else{
                    TempState[3]&=(~(UINT64(1)<<index[j]));
                }
            }
            if((i>>32)&1){
                TempState[4]|=(UINT64(1)<<index[0]);
            }
            else{
                TempState[4]&=(~(UINT64(1)<<index[0]));
            }
            AsconPermutation(TempState);
            FinalState[0]^=TempState[0];
        }
        printf("cube sum on #%I64d random cube of degree 33:0x%I64x\n",aa+1,FinalState[0]);
        if(FinalState[0]!=0){
            nonzero=nonzero+1;
        }
        printf("#nonzero/#total:%I64d/%I64d,ratio:%f\n\n",nonzero,aa+1,float(nonzero)/float(aa+1));
    }
    printf("over");
    getchar();
    return 0;
}