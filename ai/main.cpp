#include "common.h"
#include "io.h"
#include "model/game.h"
#include "model/result.h"
#include "AI/lightningAI.h"

using namespace std;

int main()
{
#ifdef _MSC_VER
	//freopen("../../ICFPC2015/cpp_input/problem_1_0.txt", "r", stdin);		// 900
	//freopen("../../ICFPC2015/cpp_input/problem_17_0.txt", "r", stdin);	// 1400
	//freopen("../../ICFPC2015/cpp_input/problem_19_0.txt", "r", stdin);		// 1300
	freopen("../../ICFPC2015/cpp_input/problem_0_0.txt", "r", stdin);	// 620
#endif

	Game g = createGameFromInput();

	// サイズとかでAI変えたりしたいよね
    
    LightningAI ai(g);
    
    Result res = ai.run();
    
	printf("%s", res.command.c_str());
    fprintf(stderr, "%d\n", res.score);

	return 0;
}
