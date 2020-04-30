#include <vector>
using namespace std;
class V_Instruction;
class URM{
	public:
	vector<int> vars;
	vector<V_Instruction*> *instructions;
	int programCounter;
	URM(vector <V_Instruction*> *instructions);
};
