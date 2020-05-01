#include <vector>
#include <map>
using namespace std;
class V_Instruction;
class URM{
	public:
	vector<int> *variables;
	vector<V_Instruction*> *instructions;
	map<int,int> varMap;
	int programCounter;
	URM(vector <V_Instruction*> *instructions,vector <int> *vars,map <int,int> varMap);
	void run();
};
