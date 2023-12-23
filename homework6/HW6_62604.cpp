#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <math.h>
#include <queue>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

/// @brief USED EXTERNAL SOURCES FOR CAREATING THIS SOURCE CODE 

vector<vector<string>> alldata;
vector<unordered_set<string>> allValues;
int minSampleSize = 50;

struct Node
{
    bool isRec;
    bool isLeaf = false;

    int errors = 0;

    int attribute;
    string value;

    Node* parent = nullptr;
	vector<Node*> children;
    unordered_set<int> prevAttributes;
};

struct DataSet
{
    vector<vector<string>> rows;
    vector<unordered_set<string>> attrValues;
    // 0 = no-rec; 1 = rec
    unordered_map<string, int> table[2][10];

    int datasize;
    int noRecSize = 0;
    int recSize = 0;

    DataSet(vector<vector<string>> rows)
    {
        this->rows = rows;
        getAttrValuesFromRows();
        fromRowsToTable();
    }

    void getAttrValuesFromRows()
    {
        attrValues.resize(10);

        for (int i = 0; i<rows.size(); i++)
        {
            for (int j = 1; j<rows[i].size(); j++) // skipping first attribute (it's the class itself)
            {
                attrValues[j].insert(rows[i][j]);
            }
        }
    }

    void fromRowsToTable()
    {
        bool isRec;

        for(int i = 0; i<rows.size(); i++)
        {
            for (int j = 0; j<rows[i].size(); j++)
            {
                if (j == 0)
                {
                    isRec = rows[i][j][0] == 'r';
                    isRec ? recSize++ : noRecSize++;
                }
                else
                {
                    table[isRec][j][rows[i][j]]++;
                }
            }
        }

        datasize = recSize + noRecSize;
    }

    void printRows()
    {
        for(int i = 0; i<rows.size(); i++)
        {
            for(int j = 0; j<rows[i].size(); j++)
            {
                cout << rows[i][j] << " ";
            }
            cout << endl;
        }
    }

    void printValues()
    {
        for(int i = 1; i<attrValues.size(); i++)
        {
            for (string x: attrValues[i]) std::cout << x << " ";
            cout << endl;
        }
    }

    double entropy(double a, double b)
    {
        if (a == 0 || b == 0)
        {
            return 0;
        }

        double pa = a/(a+b);
        double pb = b/(a+b);

        return -(pa)*log2(pa) - (pb)*log2(pb);
    }

    double entropyAttr(int attrIndex)
    {
        string key;

        double entr = 0;
        double noRec, rec;

        for ( auto it = table[0][attrIndex].begin(); it != table[0][attrIndex].end(); ++it )
        {
            key = it->first;
            noRec = it->second;
            rec = table[1][attrIndex][key];
            entr += ((noRec+rec)/datasize) * entropy (noRec, rec);
        }

        return entr;
    }

    double infoGain(int attrIndex)
    {
        return entropy(noRecSize, recSize) - entropyAttr(attrIndex);
    }

    int findBestAttribute(unordered_set<int> prevAttributes)
    {
        double maxInfoGain = INT_MIN;
        double curInfoGain;
        int bestAttr = 1;

        for (int curAttr = 1; curAttr < 10 ; curAttr++)
        {
            // it is missing in prev attributes => it will be processed
            if (prevAttributes.find(curAttr) == prevAttributes.end())
            {
                curInfoGain = infoGain(curAttr);
                if (curInfoGain > maxInfoGain)
                {
                    maxInfoGain = curInfoGain;
                    bestAttr = curAttr;
                }
            }
        }

        return bestAttr;
    }

    DataSet* filter(int attr, string value)
    {
        vector<vector<string>> newRows;

        for (int i = 0; i < rows.size(); i++)
        {
            if(value.compare(rows[i][attr]) == 0)
            {
                newRows.push_back(rows[i]);
            }
        }

        DataSet* newDataset = new DataSet(newRows);

        return newDataset;
    }

    bool isRecMoreCommon(Node* node)
    {
        if (recSize == noRecSize)
        {
            return node->parent->isRec;
        }
        return recSize > noRecSize;
    }

};

struct DecisionTree
{
    Node* root;

    void buildAttributeNode(DataSet* dataset, Node* parentNode)
    {
        Node* childNode;

        if (parentNode == nullptr)
        {
            root = new Node();
            childNode = root;
        }
        else
        {
            childNode = new Node();
            childNode->prevAttributes = parentNode->prevAttributes;
            parentNode->children.push_back(childNode);
        }

        int bestAttr = dataset->findBestAttribute(childNode->prevAttributes);

        childNode->attribute = bestAttr;
        childNode->prevAttributes.insert(bestAttr);
        childNode->parent = parentNode;
        childNode->isRec = dataset->isRecMoreCommon(childNode);

        unordered_set<string> attrValues = allValues[bestAttr];

        for (string value: attrValues)
        {
            buildValueNode(bestAttr, value, childNode, dataset);
        }
        return;
    }

    virtual void buildValueNode(int attr, string value, Node* parentNode, DataSet* dataset) = 0;

    bool classify(vector<string> row)
    {
        int attr;
        string value;
        Node* node = root;

        while(true)
        {
            attr = node->attribute;
            value = row[attr];

            for (int i = 0; i < node->children.size(); i++)
            {
                if(value.compare(node->children[i]->value) == 0)
                {
                    if (node->children[i]->isLeaf)
                    {
                        return (node->children[i]->isRec) ?
                        row[0].compare("recurrence-events") == 0 :
                        row[0].compare("no-recurrence-events") == 0 ;
                    }

                    node = node->children[i]->children[0];
                    break;
                }
            }
        }
    }

    bool isEntropyZero(DataSet* dataset) {
        return dataset->entropy(dataset->recSize, dataset->noRecSize) == 0;
    }

    void printTree()
    {
        queue<Node*> nodes;
        nodes.push(root);
        Node* curNode;

        while(!nodes.empty())
        {
            curNode = nodes.front();
            nodes.pop();
            if (curNode->isLeaf)
            {
                cout << curNode->isRec << endl;
            }
            for(int i = 0 ; i < curNode->children.size(); i++)
            {
                nodes.push(curNode->children[i]);
            }
        }
    }
};

struct DecisionTreePrePruning : public DecisionTree
{

    DecisionTreePrePruning(DataSet* dataset)
    {
        buildAttributeNode(dataset, nullptr);
    }

    void buildValueNode(int attr, string value, Node* parentNode, DataSet* dataset)
    {
        if (isEntropyZero(dataset) || parentNode->prevAttributes.size() == 9 || dataset->rows.size() < minSampleSize)
        {
            Node* leaf = new Node();
            leaf->value = value;
            leaf->isLeaf = true;
            leaf->parent = parentNode;
            leaf->isRec = dataset->isRecMoreCommon(leaf);
            parentNode->children.push_back(leaf);
            return;
        }

        Node* childNode = new Node();
        childNode->attribute = attr;
        childNode->value = value;
        childNode->prevAttributes = parentNode->prevAttributes;
        childNode->parent = parentNode;
        childNode->isRec = dataset->isRecMoreCommon(childNode);
        parentNode->children.push_back(childNode);
        DataSet* subset = dataset->filter(attr, value);
        buildAttributeNode(subset, childNode);

        delete subset;
    }
};



int myrandom (int i) { return std::rand()%i;}

 void readFromFile()
{
    string line, attribute;
    ifstream filein("breast-cancer.data", ios::in);

    allValues.resize(10);
    int counter = 0;

    while(getline(filein, line)){
        stringstream ss(line);
        vector<string> attributes;
        while(getline(ss, attribute, ',')){
            attributes.push_back(attribute);
            allValues[counter].insert(attribute);
            counter ++;
        }
        alldata.push_back(attributes);
        counter = 0;
    }

    std::srand ( unsigned ( std::time(0) ) );
    std::random_shuffle(alldata.begin(), alldata.end(), myrandom);

    filein.close();
}

double calculateAccuracyPrePruning(vector<vector<string>>trainset, vector<vector<string>>testset)
{
    DataSet* dataset = new DataSet(trainset);

    DecisionTreePrePruning tree(dataset);

    double sum = 0;

    for (int i = 0 ; i<testset.size(); i++)
    {
        sum += tree.classify(testset[i]);
    }

    delete dataset;

    return (sum * 100)/testset.size();
}


void tenFoldCrossValidate()
{
    // 10-fold cross-validation
    int testsize = alldata.size()/10;
    int testinit;

    double sumpercPrePruning = 0;
    double accuracyPrePruning;
  
    vector<vector<string>>trainset;
    vector<vector<string>>testset;

    cout << "Pre-pruning  |" << endl;
    cout << "---------------------------" << endl;

    for (int j = 0; j < 10; j++)
    {
        testinit = j * testsize;

        for (int i = 0 ; i<alldata.size(); i++)
        {
            if (i >= testinit && i< testinit + testsize)
            {
                testset.push_back(alldata[i]);
            }
            else
            {
                trainset.push_back(alldata[i]);
            }
        }

        accuracyPrePruning = calculateAccuracyPrePruning(trainset, testset);

        cout << setw(10) << accuracyPrePruning <<  "%" << "  |  " <<  endl;
        sumpercPrePruning += accuracyPrePruning;

        trainset.clear();
        testset.clear();
    }

    cout << "---------------------------" << endl;
    cout << setw(10) << sumpercPrePruning/10 << "%" << "  |  " << endl;
}

int main()
{
    readFromFile();
    tenFoldCrossValidate();

    return 0;
}
