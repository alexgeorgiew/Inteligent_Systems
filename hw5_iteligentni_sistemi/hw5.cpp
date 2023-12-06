

struct PoliticianData
{
   string className;
   bool handicapped-infants;
   bool water-project-cost-sharing;
   bool adoption-of-the-budget-resolution;
   bool physician-fee-freeze;
   bool el-salvador-aid;
   bool religious-groups-in-schools;
   bool anti-satellite-test-ban;
   bool aid-to-nicaraguan-contras;
   bool mx-missile;
   bool immigration;
   bool synfuels-corporation-cutback;
   bool education-spending;
   bool superfund-right-to-sue;
   bool crime;
   bool duty-free-exports;
   bool export-administration-act-south-africa;
}

int getLineOfFile(string filename)
{
    int number_of_lines = 0;
    FILE *infile = fopen(filename, "r");
    int ch;

    while (EOF != (ch=getc(infile)))
        if ('\n' == ch)++number_of_lines;
        
    printf("%u\n", number_of_lines);
    return number_of_lines;
}
void readFile(string filename,data*)
void AllocMemory()
{
}
void FreeMemory()
{
}

int main()
{
	
}
