#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;


struct PoliticianData
{
   bool isRepublican;
   bool handicapped_infants;
   bool water_project_cost_sharing;
   bool adoption_of_the_budget_resolution;
   bool physician_fee_freeze;
   bool el_salvador_aid;
   bool religious_groups_in_schools;
   bool anti_satellite_test_ban;
   bool aid_to_nicaraguan_contras;
   bool mx_missile;
   bool immigration;
   bool synfuels_corporation_cutback;
   bool education_spending;
   bool superfund_right_to_sue;
   bool crime;
   bool duty_free_exports;
   bool export_administration_act_south_africa;
   PoliticianData(bool x1,bool x2,bool x3,bool x4,bool x5,bool x6,bool x7,bool x8,bool x9,bool x10,bool x11,bool x12,bool x13,bool x14,bool x15,bool x16,bool x17)
            :   isRepublican(x1),handicapped_infants(x2),water_project_cost_sharing(x3),adoption_of_the_budget_resolution(x4),physician_fee_freeze(x5),el_salvador_aid(x6),
                religious_groups_in_schools(x7),anti_satellite_test_ban(x8),aid_to_nicaraguan_contras(x9),mx_missile(x10),immigration(x11),
                synfuels_corporation_cutback(x12),education_spending(x13),superfund_right_to_sue(x14),crime(x15),duty_free_exports(x16),export_administration_act_south_africa(x17){}
};

struct PoliticianData* data = NULL;
char* filename = "house-votes-84.data";
int getLinesOfFile()
{
    int number_of_lines = 0;
    FILE *infile = fopen(filename, "r");
    int ch;

    while (EOF != (ch=getc(infile)))
        if ('\n' == ch)++number_of_lines;
        
    printf("%u\n", number_of_lines);
    return number_of_lines;
}

vector<string> getWords(string s){
    vector<string> res;
    int pos = 0;
    while(pos <= s.size()){
        pos = s.find(",");
        res.push_back(s.substr(0,pos));
        s.erase(0,pos+1); // 1 is the length of the delimiter, ","
    }
    return res;
}

void readFile(struct PoliticianData** data)
{
    std::ifstream input ( filename );
    int count = 0;
    for( std::string line; getline(input, line ); )
    {
        vector<string> segments = getWords(line);
        for(int i=0 ; i<17;i++){
            if(i == 0)
            {
                if( segments[i] == "republican") (*data)[count].isRepublican = true;
                else (*data)[count].isRepublican = false;
            }
            else
            {
                 bool value;
                if(segments[i][0] == '?')segments[i] = "n";
                if(segments[i][0] == 'n')value = false;
                else value = true;

                if(i == 1)(*data)[count].handicapped_infants = value;
                if(i == 2)(*data)[count].water_project_cost_sharing = value;
                if(i == 3)(*data)[count].adoption_of_the_budget_resolution = value;
                if(i == 4)(*data)[count].physician_fee_freeze = value;
                if(i == 5)(*data)[count].el_salvador_aid = value;
                if(i == 6)(*data)[count].religious_groups_in_schools = value;
                if(i == 7)(*data)[count].anti_satellite_test_ban = value;
                if(i == 8)(*data)[count].aid_to_nicaraguan_contras = value;
                if(i == 9)(*data)[count].mx_missile = value;
                if(i == 10)(*data)[count].immigration = value;
                if(i == 11)(*data)[count].synfuels_corporation_cutback = value;
                if(i == 12)(*data)[count].education_spending = value;
                if(i == 13)(*data)[count].superfund_right_to_sue = value;
                if(i == 14)(*data)[count].crime = value;
                if(i == 15)(*data)[count].duty_free_exports = value;
                if(i == 16)(*data)[count].export_administration_act_south_africa = value;
            }
        }
        count++;
    }

}

void AllocMemory(int samples,struct PoliticianData** data)
{
    *data = (struct PoliticianData*)malloc(sizeof(struct PoliticianData) * samples);
}

void FreeMemory(struct PoliticianData** data)
{
    free(*data);
}

double laplace(int k, int number_classes,int attr)
{          
    cout<<"k="<<k<<" number_classes="<<number_classes<<" attrcount="<<attr<<endl;                                             
    double result = double(k)/double(attr + number_classes*k);
    return result;
}
int main()
{
    int samples = getLinesOfFile();
    struct PoliticianData* data = NULL;

    AllocMemory(samples,&data);
    readFile(&data);

    int set10index[10];
    set10index[0] = 0;
    int samplesInSet = samples/10;
    for(int i=1 ;i<10;i++)set10index[i] = samplesInSet*i;



    //Calculate prior probability for classRepublicans  P(attr1 | CLASS1) = ?
    double classRepublicansPriorProb[16],classDemocratePriorProb[16],classRepublicansPriorProbNeg[16],classDemocratePriorProbNeg[16];
    double probRepublican,probDemocrate;
    int lengthRep = 0,lengthDem = 0;
    int countLablesRepub[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int countLablesDemo[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int countLablesRepubNeg[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    int countLablesDemoNeg[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    cout << "IndexTOLearn"<<set10index[1] <<endl;
    for(int i=set10index[0] ; i<set10index[1] ; i++)
    {
        if(data[i].isRepublican)
        {
            if( data[i].handicapped_infants)countLablesRepub[0]++;
            else countLablesRepubNeg[0]++;
            if( data[i].water_project_cost_sharing)countLablesRepub[1]++;
            else countLablesRepubNeg[1]++;
            if( data[i].adoption_of_the_budget_resolution) countLablesRepub[2]++;
            else countLablesRepubNeg[2]++;
            if( data[i].physician_fee_freeze)countLablesRepub[3]++;
            else countLablesRepubNeg[3]++;
            if( data[i].el_salvador_aid)countLablesRepub[4]++;
            else countLablesRepubNeg[4]++;
            if( data[i].religious_groups_in_schools) countLablesRepub[5]++;
            else countLablesRepubNeg[5]++;
            if( data[i].anti_satellite_test_ban)countLablesRepub[6]++;
            else countLablesRepubNeg[6]++;
            if( data[i].aid_to_nicaraguan_contras)countLablesRepub[7]++;
            else countLablesRepubNeg[7]++;
            if( data[i].mx_missile) countLablesRepub[8]++;
            else countLablesRepubNeg[8]++;
            if( data[i].immigration) countLablesRepub[9]++;
            else countLablesRepubNeg[9]++;
            if( data[i].synfuels_corporation_cutback)countLablesRepub[10]++;
            else countLablesRepubNeg[10]++;
            if( data[i].education_spending)countLablesRepub[11]++;
            else countLablesRepubNeg[11]++;
            if( data[i].superfund_right_to_sue)countLablesRepub[12]++;
            else countLablesRepubNeg[12]++;
            if( data[i].crime)countLablesRepub[13]++;
            else countLablesRepubNeg[13]++;
            if( data[i].duty_free_exports)countLablesRepub[14]++;
            else countLablesRepubNeg[14]++;
            if( data[i].export_administration_act_south_africa )countLablesRepub[15]++;
            else countLablesRepubNeg[15]++;

            lengthRep++;
        }
        else
        {
            if( data[i].handicapped_infants)countLablesDemo[0]++;
            else countLablesDemoNeg[0]++;
            if( data[i].water_project_cost_sharing)countLablesDemo[1]++;
            else countLablesDemoNeg[1]++;
            if( data[i].adoption_of_the_budget_resolution) countLablesDemo[2]++;
            else countLablesDemoNeg[2]++;
            if( data[i].physician_fee_freeze)countLablesDemo[3]++;
            else countLablesDemoNeg[3]++;
            if( data[i].el_salvador_aid)countLablesDemo[4]++;
            else countLablesDemoNeg[4]++;
            if( data[i].religious_groups_in_schools) countLablesDemo[5]++;
            else countLablesDemoNeg[5]++;
            if( data[i].anti_satellite_test_ban)countLablesDemo[6]++;
            else countLablesDemoNeg[6]++;
            if( data[i].aid_to_nicaraguan_contras)countLablesDemo[7]++;
            else countLablesDemoNeg[7]++;
            if( data[i].mx_missile) countLablesDemo[8]++;
            else countLablesDemoNeg[8]++;
            if( data[i].immigration) countLablesDemo[9]++;
            else countLablesDemoNeg[9]++;
            if( data[i].synfuels_corporation_cutback)countLablesDemo[10]++;
            else countLablesDemoNeg[10]++;
            if( data[i].education_spending)countLablesDemo[11]++;
            else countLablesDemoNeg[11]++;
            if( data[i].superfund_right_to_sue)countLablesDemo[12]++;
            else countLablesDemoNeg[12]++;
            if( data[i].crime)countLablesDemo[13]++;
            else countLablesDemoNeg[13]++;
            if( data[i].duty_free_exports)countLablesDemo[14]++;
            else countLablesDemoNeg[14]++;
            if( data[i].export_administration_act_south_africa )countLablesDemo[15]++;
            else countLablesDemoNeg[15]++;

            lengthDem++;
        }
    }
    //Eval prop p(a1|republican)
    for(int i=0;i<16;i++) {
        //classRepublicansPriorProb[i] = (double)countLablesRepub[i] / (double)lengthRep;
        //classDemocratePriorProb[i] = (double)countLablesDemo[i] / (double)lengthDem;
        classRepublicansPriorProb[i] = laplace(1,2,countLablesRepub[i] + countLablesDemo[i]);
        classDemocratePriorProb[i] = laplace(1,2,countLablesRepub[i]+ countLablesDemo[i]);
        classRepublicansPriorProbNeg[i] = laplace(1,2,countLablesRepubNeg[i] + countLablesDemoNeg[i]);
        classDemocratePriorProbNeg[i] = laplace(1,2,countLablesRepubNeg[i]+ countLablesDemoNeg[i]);
        /*if(classRepublicansPriorProb[i] == 0.0f){
            classRepublicansPriorProb[i] = laplace(1,2,countLablesRepub[i] + countLablesDemo[i]);
            cout<<"INdex_rep " << i <<"now value "<<classRepublicansPriorProb[i]<<endl;
        }
        if(classDemocratePriorProb[i] == 0.0f){
            classDemocratePriorProb[i] = laplace(1,2,countLablesRepub[i]+ countLablesDemo[i]);
             cout<<"INdex_rep " << i <<"now value "<<classDemocratePriorProb[i]<<endl;
        }*/
    }

    probRepublican = ((double)lengthRep/(double)set10index[1]);
    probDemocrate = 1.0f - probRepublican;
    
    int uspeh=0;
    //Here we will calc P(c1 | a1,a2) = p(a1,a2 | c1) = p(a1|c1)*p(a2|c1)*p(c1);
    for(int i=set10index[1];i<samples;i++)
    {
       double valueRep=0.0f, valueDem=0.0f;
       
            if( data[i].handicapped_infants)valueRep += log(classRepublicansPriorProb[0]) ,valueDem+=log(classDemocratePriorProb[0]);
            else valueRep += log(classRepublicansPriorProbNeg[0]) ,valueDem += log(classDemocratePriorProbNeg[0]);

            if( data[i].water_project_cost_sharing)valueRep += log(classRepublicansPriorProb[1]) ,valueDem+=log(classDemocratePriorProb[1]);
            else valueRep += log(classRepublicansPriorProbNeg[1]) ,valueDem += log(classDemocratePriorProbNeg[1]);

            if( data[i].adoption_of_the_budget_resolution)valueRep += log(classRepublicansPriorProb[2]) ,valueDem+=log(classDemocratePriorProb[2]);
            else valueRep += log(classRepublicansPriorProbNeg[2]) ,valueDem += log(classDemocratePriorProbNeg[2]);

            if( data[i].physician_fee_freeze)valueRep += log(classRepublicansPriorProb[3]) ,valueDem+=log(classDemocratePriorProb[3]);
            else valueRep += log(classRepublicansPriorProbNeg[3]) ,valueDem += log(classDemocratePriorProbNeg[3]);

            if( data[i].el_salvador_aid) valueRep += log(classRepublicansPriorProb[4]) ,valueDem+=log(classDemocratePriorProb[4]);
            else valueRep += log(classRepublicansPriorProbNeg[4]) ,valueDem += log(classDemocratePriorProbNeg[4]);

            if( data[i].religious_groups_in_schools) valueRep += log(classRepublicansPriorProb[5]) ,valueDem+=log(classDemocratePriorProb[5]);
            else valueRep += log(classRepublicansPriorProbNeg[5]) ,valueDem += log(classDemocratePriorProbNeg[5]);

            if( data[i].anti_satellite_test_ban)valueRep += log(classRepublicansPriorProb[6]) ,valueDem+=log(classDemocratePriorProb[6]);
            else valueRep += log(classRepublicansPriorProbNeg[6]) ,valueDem += log(classDemocratePriorProbNeg[6]);

            if( data[i].aid_to_nicaraguan_contras)valueRep += log(classRepublicansPriorProb[7]) ,valueDem+=log(classDemocratePriorProb[7]);
            else valueRep += log(classRepublicansPriorProbNeg[7]) ,valueDem += log(classDemocratePriorProbNeg[7]);

            if( data[i].mx_missile) valueRep += log(classRepublicansPriorProb[8]) ,valueDem+=log(classDemocratePriorProb[8]);
            else valueRep += log(classRepublicansPriorProbNeg[8]) ,valueDem += log(classDemocratePriorProbNeg[8]);

            if( data[i].immigration) valueRep += log(classRepublicansPriorProb[9]) ,valueDem+=log(classDemocratePriorProb[9]);
            else valueRep += log(classRepublicansPriorProbNeg[9]) ,valueDem += log(classDemocratePriorProbNeg[9]);

            if( data[i].synfuels_corporation_cutback)valueRep += log(classRepublicansPriorProb[10]) ,valueDem+=log(classDemocratePriorProb[10]);
            else valueRep += log(classRepublicansPriorProbNeg[10]) ,valueDem += log(classDemocratePriorProbNeg[10]);

            if( data[i].education_spending)valueRep += log(classRepublicansPriorProb[11]) ,valueDem+=log(classDemocratePriorProb[11]);
            else valueRep += log(classRepublicansPriorProbNeg[11]) ,valueDem += log(classDemocratePriorProbNeg[11]);

            if( data[i].superfund_right_to_sue) valueRep += log(classRepublicansPriorProb[12]) ,valueDem+=log(classDemocratePriorProb[12]);
            else valueRep += log(classRepublicansPriorProbNeg[12]) ,valueDem += log(classDemocratePriorProbNeg[12]);

            if( data[i].crime) valueRep += log(classRepublicansPriorProb[13]) ,valueDem+=log(classDemocratePriorProb[13]);
            else valueRep += log(classRepublicansPriorProbNeg[13]) ,valueDem += log(classDemocratePriorProbNeg[13]);

            if( data[i].duty_free_exports)valueRep += log(classRepublicansPriorProb[14]) ,valueDem+=log(classDemocratePriorProb[14]);
            else valueRep += log(classRepublicansPriorProbNeg[14]) ,valueDem += log(classDemocratePriorProbNeg[14]);

            if( data[i].export_administration_act_south_africa )valueRep += log(classRepublicansPriorProb[15]) ,valueDem+=log(classDemocratePriorProb[15]);
            else valueRep += log(classRepublicansPriorProbNeg[15]) ,valueDem += log(classDemocratePriorProbNeg[15]);

        valueRep += log(probRepublican);
        valueDem += log(probDemocrate);
        cout << data[i].isRepublican << " " << "Predicted value for Republican:" << valueRep << " Predicted value for Democrate:" << valueDem << endl;

        bool res = data[i].isRepublican ? (valueRep >= valueDem) : (valueRep <= valueDem);
        if(res)uspeh++;

    }

    cout << "Accuracy=" << (double)uspeh/(samples-set10index[1]) << endl;

    FreeMemory(&data);
}
