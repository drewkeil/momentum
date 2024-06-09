#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class gameMenu{
public:


private:
	enum class menustate:uint8_t {};
	std::string main[2]={"load level", "controls"};
	std::string controls={"up:", "down:", "left:", "right:", "shift:", "jump:", "back"};
	std::vector<std::string> lvlselect;
};
