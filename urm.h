#include <vector>
#include <map>
using namespace std;
class V_Instruction;
class URM{
	public:
	vector<int> *variables;
	vector<V_Instruction*> *instructions;
	map<string,int> varMap;
	int programCounter;
	URM(vector <V_Instruction*> *instructions,vector <int> *vars,map <string,int> varMap);
	void run();
};
