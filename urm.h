#include <vector>
using namespace std;
class Instruction;
class URM{
	public:
	vector<int> vars;
	vector<Instruction> instructions;
	int programCounter;
	URM();
};
