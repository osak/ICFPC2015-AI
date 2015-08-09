#include "common.h"
#include "io.h"
#include "model/game.h"
#include "model/result.h"
#include "AI/lightningAI.h"
#include "calc.h"

using namespace std;

int main()
{
#ifdef _MSC_VER
	freopen("../../ICFPC2015/cpp_input/problem_23_0.txt", "r", stdin);
	//freopen("../../ICFPC2015/cpp_input/problem_4_0.txt", "r", stdin);		// 900
	//freopen("../../ICFPC2015/cpp_input/problem_1_0.txt", "r", stdin);		// 900
	//freopen("../../ICFPC2015/cpp_input/problem_17_0.txt", "r", stdin);	// 1400
	//freopen("../../ICFPC2015/cpp_input/problem_19_0.txt", "r", stdin);		// 1300
	//freopen("../../ICFPC2015/cpp_input/problem_21_0.txt", "r", stdin);	// 620
#endif

	Game g = createGameFromInput();

	// サイズとかでAI変えたりしたいよね
    
    LightningAI ai(g);
    
    Result res = ai.run();
    Result ans = Calc::calcScore(g, res.command);
    
    fprintf(stderr, "%d -> %d\n", res.score, ans.score);
    printf("%s", ans.command.c_str());
    
	return 0;
}
