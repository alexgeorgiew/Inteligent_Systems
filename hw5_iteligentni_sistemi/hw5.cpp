#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;


struct PoliticianData
{ 
   //isRepublican;handicapped_infants;water_project_cost_sharing;adoption_of_the_budget_resolution;physician_fee_freeze;el_salvador_aid;religious_groups_in_schools;anti_satellite_test_ban;aid_to_nicaraguan_contras;mx_missile;immigration;synfuels_corporation_cutback;education_spending;superfund_right_to_sue;crime;duty_free_exports;export_administration_act_south_africa;
   bool values[17];

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
                if( segments[i] == "republican") (*data)[count].values[0] = true;
                else (*data)[count].values[0] = false;
            }
            else
            {
                bool isSet;
                if(segments[i][0] == '?')segments[i] = "n";
                if(segments[i][0] == 'n')isSet = false;
                else isSet = true;

                (*data)[count].values[i] = isSet;
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
    double result = double(k)/double(attr + number_classes*k);
    return result;
}
int main()
{
    int samples = getLinesOfFile();
    struct PoliticianData* data = NULL;

    AllocMemory(samples,&data);
    readFile(&data);

    int set10index[11];
    set10index[0] = 0;
    set10index[10] = samples-1;
    int samplesInSet = samples/10;
    for(int i = 1 ; i < 10 ; i++)set10index[i] = samplesInSet*i;

    for(int trainset = 0;trainset < 10; trainset++)
    {
        //Calculate prior probability for classRepublicans  P(attr1 | CLASS1) = ?
        double classRepublicansPriorProb[16],classDemocratePriorProb[16],classRepublicansPriorProbNeg[16],classDemocratePriorProbNeg[16];
        double probRepublican,probDemocrate;
        int lengthRep = 0,lengthDem = 0;
        int countLablesRepub[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int countLablesDemo[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int countLablesRepubNeg[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
        int countLablesDemoNeg[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

        cout << "IndexTOLearn FROM " << set10index[trainset] << " TO " << set10index[trainset + 1] << endl;

        for(int i = set10index[trainset] ; i<set10index[trainset + 1] ; i++)
        {
            if(data[i].values[0]) //IF  the current example is REPUBLICAN
            {
                for(int iterateBools = 1; iterateBools < 17; iterateBools++)
                {
                    if(data[i].values[iterateBools])countLablesRepub[iterateBools - 1]++;
                    else countLablesRepubNeg[iterateBools - 1]++;
                }

                lengthRep++;
            }
            else
            {
                for(int iterateBools = 1; iterateBools < 17; iterateBools++)
                {
                    if(data[i].values[iterateBools])countLablesDemo[iterateBools - 1]++;
                    else countLablesDemoNeg[iterateBools - 1]++;
                }

                lengthDem++;
            }
        }

        //Eval prop p( attr | Class)
        for(int i=0;i<16;i++) {
            classRepublicansPriorProb[i] = laplace(1,2,countLablesRepub[i] + countLablesDemo[i]);
            classDemocratePriorProb[i] = laplace(1,2,countLablesRepub[i]+ countLablesDemo[i]);
            classRepublicansPriorProbNeg[i] = laplace(1,2,countLablesRepubNeg[i] + countLablesDemoNeg[i]);
            classDemocratePriorProbNeg[i] = laplace(1,2,countLablesRepubNeg[i]+ countLablesDemoNeg[i]);
        }

        probRepublican = ( (double)lengthRep/(double)(set10index[trainset + 1] - set10index[trainset] + 1) );
        probDemocrate = 1.0f - probRepublican;
        
        int uspeh=0;
        //Here we will calc P(c1 | a1,a2) = p(a1,a2 | c1) = p(a1|c1)*p(a2|c1)*p(c1);
        for(int i = 0 ; i < samples ;i++)
        {
            if( i < set10index[trainset] || set10index[trainset + 1] < i ) //index outside the training set
            {
               double valueRep=0.0f, valueDem=0.0f;

                for(int iterateBools = 1; iterateBools < 17; iterateBools++)
                {
                    if(data[i].values[iterateBools])valueRep += log(classRepublicansPriorProb[iterateBools - 1]) ,valueDem+=log(classDemocratePriorProb[iterateBools - 1]);
                    else valueRep += log(classRepublicansPriorProbNeg[iterateBools - 1]) ,valueDem += log(classDemocratePriorProbNeg[iterateBools - 1]);
                }

                valueRep += log(probRepublican);
                valueDem += log(probDemocrate);
                //cout << data[i].isRepublican << " " << "Predicted value for Republican:" << valueRep << " Predicted value for Democrate:" << valueDem << endl;

                bool res = data[i].values[0] ? (valueRep >= valueDem) : (valueRep <= valueDem);
                if(res)uspeh++;
            }

        }

        cout << "Accuracy =" << (double)uspeh/(samples-(set10index[trainset+1] - set10index[trainset] + 1) ) << endl;
    }

    FreeMemory(&data);
}
