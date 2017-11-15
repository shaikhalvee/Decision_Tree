/*
* @author: SHAIKHALVEE
* @topic: Decision Tree
*
*/

#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/pb_ds/detail/standard_policies.hpp>
#define MIN(X,Y) X<Y?X:Y
#define MAX(X,Y) X>Y?X:Y
#define ISNUM(a) ('0'<=(a) && (a)<='9')
#define ISCAP(a) ('A'<=(a) && (a)<='Z')
#define ISSML(a) ('a'<=(a) && (a)<='z')
#define ISALP(a) (ISCAP(a) || ISSML(a))
#define MXX 10000000000
#define MNN -MXX
#define ISVALID(X,Y,N,M) ((X)>=1 && (X)<=(N) && (Y)>=1 && (Y)<=(M))
#define LLI long long int
#define VI vector<int>
#define VLLI vector<long long int>
#define MII map<int,int>
#define SI set<int>
#define PB push_back
#define MSI map<string,int>
#define PII pair<int,int>
#define PLLI pair<LLI,LLI>
#define FREP(i,I,N) for(int (i)=(I);(i)<=(N);(i)++)
#define eps 0.0000000001
#define RFREP(i,N,I) for(int (i)=(N);(i)>=(I);(i)--)
#define SORTV(VEC) sort(VEC.begin(),VEC.end())
#define SORTVCMP(VEC,cmp) sort(VEC.begin(),VEC.end(),cmp)
#define REVV(VEC) reverse(VEC.begin(),VEC.end())
using namespace std;
using namespace __gnu_pbds;

//int dx[]={1,0,-1,0};int dy[]={0,1,0,-1}; //4 Direction
//int dx[]={1,1,0,-1,-1,-1,0,1};int dy[]={0,1,1,1,0,-1,-1,-1};//8 direction
//int dx[]={2,1,-1,-2,-2,-1,1,2};int dy[]={1,2,2,1,-1,-2,-2,-1};//Knight Direction
//int dx[]={2,1,-1,-2,-1,1};int dy[]={0,1,1,0,-1,-1}; //Hexagonal Direction


typedef tree < int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update > ordered_set;

int ahnafcount=0;

int dump[100005][30];
int dataset[100005][30];
int predictset[100005][30];

int ROW,COL;

class DTNode {
public:
    // Initialize your data structure here.
    int value; int curfeature; int childofparent;
    bool leaf;
    DTNode* children[12];
    DTNode() {
        value = 0;
        curfeature = 0;
        leaf = false;
        FREP(i,1,10)children[i]=NULL;
    }
    void makeleaf(int decision){
        leaf=true;
        value=decision;
    }
    void selectfeature(int feature){
        leaf=false;
        curfeature=feature;
    }
};


double getHs(int tot, int pos, int neg){
    if(pos==0 || neg==0)return 0.0;
    double posfrac=(double)pos/(double)tot;double negfrac=(double)neg/(double)tot;
    double Hs=posfrac*log2(posfrac)+negfrac*log2(negfrac);
    return Hs*(-1.0);
}


//returns dataset for current node and its entropy, entropy 0 hoile kichu kaaj korte hoy
pair<double,SI> calcrootentropy(vector<PII>takendecisions, int r, int c){
    int total=0;int neg=0;int pos=0;
    SI validset;
    FREP(i,1,r){
        int f=1;
        int cond=takendecisions.size();
        FREP(k,0,(cond-1)){
            PII curcond=takendecisions[k];
            int feat=curcond.first;int featval=curcond.second;
            if(dataset[i][feat]!=featval){
                f=0;
                break;
            }
        }
        if(f){
            total++;
            validset.insert(i);
            if(dataset[i][c]==1)pos++;
            else neg++;
        }
    }
    if(total==0){
        return make_pair(-1.0,validset); //no examples
    }
    double Hs=getHs(total,pos,neg);
    return make_pair(Hs,validset);
}

double featureHs(SI curdataset, int r, int feauture, int c){
    double childval=0.0;
    int tot[12];int pos[12];
    memset(tot,0,sizeof(tot));memset(pos,0,sizeof(pos));

    SI:: iterator it;
    for(it=curdataset.begin();it!=curdataset.end();++it){
        int currow=*it;
        int val=dataset[currow][feauture];
        tot[val]++;
        if(dataset[currow][c]==1)pos[val]++;
    }

    int totsz=curdataset.size();
    FREP(i,1,10){
        double weight=(double)tot[i]/totsz; double Hshere=getHs(tot[i],pos[i],tot[i]-pos[i]);
        childval=childval+weight*Hshere;
    }
    return childval;
}


//returns available feature er moddhe most laav jonok
int getcurfeature(SI curdataset, SI donefeatures, int r, int c){
    double minentropy=100.00; int minfeature=0;
    FREP(i,1,(c-1)){
        if(donefeatures.find(i)==donefeatures.end()){
            double curentropy=featureHs(curdataset,r,i,c);
            //printf("entropy for feature %d is %.4lf\n",i,curentropy);
            if(curentropy<minentropy){
                minfeature=i; minentropy=curentropy;
            }
        }
    }
    return minfeature;
}

void dfs(DTNode* cur, vector<PII>takendecisions, int r, int c, int commonlabel){
    SI::iterator it;
    int pos=0; int neg=0; int featuredone=takendecisions.size();
    pair<double,SI> calc1=calcrootentropy(takendecisions,r,c);
    SI curdataset=calc1.second;
    for(it=curdataset.begin();it!=curdataset.end();++it){
            int currow=*it;
            if(dataset[currow][c]==1)pos++;
            else neg++;
    }
    if(calc1.first<0.0){
        cur->makeleaf(commonlabel);return;
    }
    if(fabs(calc1.first)<eps){
        if(neg==0)cur->makeleaf(1);
        else cur->makeleaf(0);
        return;
    }
    if(featuredone==(c-1)){
        //all features exhausted
        if(pos>=neg)cur->makeleaf(1); //na thakleo bolbo ache
        else if(neg>pos)cur->makeleaf(0);
        return;
    }
    SI donefeatures;
    FREP(i,0,featuredone-1)donefeatures.insert(takendecisions[i].first);
    int choosefeat=getcurfeature(curdataset,donefeatures,r,c);
    //cout<<"optimal feature in this node "<<choosefeat<<"\n";
    cur->selectfeature(choosefeat);
    int commonlabelnxt=(pos>=neg?1:0);
    FREP(i,1,10){
        ahnafcount++;
        vector<PII>newdecisions;
        newdecisions=takendecisions;
        newdecisions.PB(make_pair(choosefeat,i));
        cur->children[i]=new DTNode();
        dfs(cur->children[i],newdecisions,r,c,commonlabelnxt);
    }
    return;
}

void split(string instance, int row){
    int c=1;
    int l=instance.size();
    int curval=0;
    FREP(i,0,(l-1)){
        if(ISNUM(instance[i]))curval=curval*10+instance[i]-'0';
        else{
            dump[row][c++]=curval;
            curval=0;
        }
    }
    dump[row][c]=curval;
    COL=c;
}

void treetraverse(DTNode *cur, int level){
    printf("At level %d\n",level);
    //printf("%d\n",cur->value);
    if(cur->leaf){
        printf("label: %d\n",cur->value);
        return;
    }
    else{
        printf("feature: %d\n",cur->curfeature);
    }
    FREP(i,1,10){
        if(cur->children[i]){
            printf("If feature value: %d\n",i);
            treetraverse(cur->children[i],level+1);
        }
    }
}

int predict(DTNode *root, int row){
    DTNode *cur=root;
    while(true){
        if(cur->leaf){
           // cout<<"decision reached "<<cur->value<<"\n";
            return cur->value;
        }
        else{
            int nxtf=cur->curfeature;
            int featval=predictset[row][nxtf];
           // printf("checked feature %d and the value is %d\n",nxtf,featval);
            cur=cur->children[featval];
        }
    }
}

int main(){
    string s;
    //cin>>s;
    int currow=1;
    while(cin>>s){
        int l=s.size();
        int ok=1;
        FREP(k,0,(l-1)){
            if(ISALP(s[k])){
                ok=0;
                break;
            }
        }
        if(!ok)continue;
        //cout<<"string porlam:# ";
        //cout<<s<<" #etotuku\n";

        split(s,currow);
        /*FREP(i,1,COL){
            printf("%d ",dump[currow][i]);
        }
        printf("\n");*/
        currow++;
    }
    ROW=currow-1;
    //cout<<ROW<<"\n";
    /*printf("whole dump\n");
    FREP(i,1,ROW){
        FREP(j,1,COL){
            printf("%d ",dump[i][j]);
        }
        printf("\n");
    }*/
    int predictdatasize=ROW/5;
    printf("Iteration\tTPR\tTNR\tPPV\tNPV\tFPR\tFNR\tFDR\tF1\tAccuracy\n");
    double totTPR=0.0,totTNR=0.0,totPPV=0.0,totNPV=0.0,totFPR=0.0,totFNR=0.0,totFDR=0.0,totF1=0.0;
    double overallaccuracy=0.0;
    FREP(crossvalidateidx,1,5){
        //ahnafcount=0;
        //printf("cross validating at iteration: %d\n",crossvalidateidx);
        int forbiddenregionstart=predictdatasize*(crossvalidateidx-1)+1;
        int forbiddenregionend=min(ROW,predictdatasize*crossvalidateidx);
        int rowhere=0;
        int rowthere=0;
        FREP(dumpidx,1,ROW){
            if(dumpidx<forbiddenregionstart || dumpidx>forbiddenregionend){
                ++rowhere;
                FREP(cc,1,COL){
                    dataset[rowhere][cc]=dump[dumpidx][cc];
                }
            }
            else{
                ++rowthere;
                FREP(cc,1,COL){
                    predictset[rowthere][cc]=dump[dumpidx][cc];
                }
            }
        }
        //ekhon algo te row=rowhere ar col to COL e
        //erpor algo call korte hobe
        DTNode *root=new DTNode();
        //DTNode *head=root;
        int pos=0;
        int neg=0;
        FREP(i,1,rowhere){
            if(dataset[i][COL])pos++;
            else neg++;
        }
        int commonlab=(pos>=neg?1:0);
        vector<PII>taken;
        dfs(root,taken,rowhere,COL,commonlab);
        //treetraverse(head,0);
        int truepositive=0;
        int falsepositive=0;
        int truenegative=0;
        int falsenegative=0;
        double TPR,TNR,PPV,NPV,FPR,FNR,FDR,F1,accuracy;
        FREP(i,1,rowthere){
            //predict korar jonno
            //cout<<"predicting data "<<i<<"\n";
            int predictedans=predict(root,i);
            int trueans=predictset[i][COL];
            //cout<<"Actual value: "<<trueans<<"\n";
            if(trueans==1){
                if(predictedans==1){
                    truepositive++;
                }
                else{
                    falsenegative++;
                }
            }
            else{
                if(predictedans==0){
                    truenegative++;
                }
                else{
                    falsepositive++;
                }
            }
        }
        TPR=(double)truepositive/(double)(truepositive+falsenegative);
        TNR=(double)truenegative/(double)(truenegative+falsepositive);
        PPV=(double)truepositive/(double)(truepositive+falsepositive);
        NPV=(double)truenegative/(double)(truenegative+falsenegative);
        FPR=1.00-TNR;
        FNR=1.00-TPR;
        FDR=1.00-PPV;
        F1=(double)(2.00*truepositive)/(double)(2.00*truepositive+falsenegative+falsepositive);
        accuracy= (double) (truepositive+truenegative)/(double)(truenegative+truepositive+falsenegative+falsepositive);
        //printf("Result for this cross-validation\n");
        printf("%d\t",crossvalidateidx);
        printf("%.2lf\t",TPR*100.00);
        printf("%.2lf\t",TNR*100.00);
        printf("%.2lf\t",PPV*100.00);
        printf("%.2lf\t",NPV*100.00);
        printf("%.2lf\t",FPR*100.00);
        printf("%.2lf\t",FNR*100.00);
        printf("%.2lf\t",FDR*100.00);
        printf("%.2lf\t",F1*100.00);
        printf("%.2lf\n",accuracy*100.0);
        //cout<<ahnafcount<<"\n";
        overallaccuracy+=(accuracy*100.0);
        totTPR+=TPR*100.00;
        totTNR+=TNR*100.00;
        totPPV+=PPV*100.00;
        totNPV+=NPV*100.00;
        totFPR+=FPR*100.00;
        totFNR+=FNR*100.00;
        totFDR+=FDR*100.00;
        totF1+=F1*100.00;
    }
    printf("\n\n\n");
    printf("Average\t");
    printf("%.2lf\t",totTPR/5.0);
    printf("%.2lf\t",totTNR/5.0);
    printf("%.2lf\t",totPPV/5.0);
    printf("%.2lf\t",totNPV/5.0);
    printf("%.2lf\t",totFPR/5.0);
    printf("%.2lf\t",totFNR/5.0);
    printf("%.2lf\t",totFDR/5.0);
    printf("%.2lf\t",totF1/5.0);
    printf("%.2lf\n",overallaccuracy/5.0);
    return 0;
}
