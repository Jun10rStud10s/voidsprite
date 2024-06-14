#pragma once
#include "globals.h"

class Pattern
{
public:
	SDL_Texture* cachedIcon;
	virtual std::string getIconPath() { return "assets/pattern_default.png"; }
	virtual std::string getName() { return "Pattern"; }
	virtual bool canDrawAt(XY position) { return true; }
};

class PatternFull : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_full.png"; }
	std::string getName() override { return ""; }
	bool canDrawAt(XY position) { return true; }
};

class PatternGrid : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_grid.png"; }
	std::string getName() override { return "1x1 grid"; }
	bool canDrawAt(XY position) { return (position.x + position.y) % 2 == 0; }
};
class PatternGridReverse : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_grid_reverse.png"; }
	std::string getName() override { return "1x1 grid (reversed)"; }
	bool canDrawAt(XY position) { return (position.x + position.y) % 2 == 1; }
};
class PatternDiag2px : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_2px.png"; }
	std::string getName() override { return "Diagonal - 2px dist."; }
	bool canDrawAt(XY position) { 
		if (position.x >= 0 && position.y >= 0) {
			return (position.x % 3) == (position.y % 3);
		}
		return false;
	}
};
class PatternDiag3px : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_3px.png"; }
	std::string getName() override { return "Diagonal - 3px dist."; }
	bool canDrawAt(XY position) { 
		if (position.x >= 0 && position.y >= 0) {
			return (position.x % 4) == (position.y % 4);
		}
		return false;
	}
};
class PatternDiag4px : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_4px.png"; }
	std::string getName() override { return "Diagonal - 4px dist."; }
	bool canDrawAt(XY position) { 
		if (position.x >= 0 && position.y >= 0) {
			return (position.x % 5) == (position.y % 5);
		}
		return false;
	}
};

class PatternDiag2pxReverse : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_2px_reverse.png"; }
	std::string getName() override { return "Diagonal (reversed) - 2px dist."; }
	bool canDrawAt(XY position) {
		if (position.x >= 0 && position.y >= 0) {
			return (2-(position.x % 3)) == (position.y % 3);
		}
		return false;
	}
};
class PatternDiag3pxReverse : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_3px_reverse.png"; }
	std::string getName() override { return "Diagonal (reversed) - 3px dist."; }
	bool canDrawAt(XY position) {
		if (position.x >= 0 && position.y >= 0) {
			return (3-(position.x % 4)) == (position.y % 4);
		}
		return false;
	}
};
class PatternDiag4pxReverse : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_diag_4px_reverse.png"; }
	std::string getName() override { return "Diagonal (reversed) - 4px dist."; }
	bool canDrawAt(XY position) {
		if (position.x >= 0 && position.y >= 0) {
			return (4-(position.x % 5)) == (position.y % 5);
		}
		return false;
	}
};

class PatternHorizontal1px : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_hor_1px.png"; }
	std::string getName() override { return "Horizontal - 1px dist."; }
	bool canDrawAt(XY position) { return position.y >= 0 ? (position.y % 2) == 0 : false; }
};
class PatternHorizontal2px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_hor_2px.png"; }
	std::string getName() override { return "Horizontal - 2px dist."; }
	bool canDrawAt(XY position) { return position.y >= 0 ? (position.y % 3) == 0 : false; }
};
class PatternHorizontal3px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_hor_3px.png"; }
	std::string getName() override { return "Horizontal - 3px dist."; }
	bool canDrawAt(XY position) { return position.y >= 0 ? (position.y % 4) == 0 : false; }
};
class PatternHorizontal4px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_hor_4px.png"; }
	std::string getName() override { return "Horizontal - 4px dist."; }
	bool canDrawAt(XY position) { return position.y >= 0 ? (position.y % 5) == 0 : false; }
};

class PatternVertical1px : public Pattern
{
public:
	std::string getIconPath() override { return "assets/pattern_vert_1px.png"; }
	std::string getName() override { return "Vertical - 1px dist."; }
	bool canDrawAt(XY position) { return position.x >= 0 ? (position.x % 2) == 0 : false; }
};
class PatternVertical2px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_vert_2px.png"; }
	std::string getName() override { return "Vertical - 2px dist."; }
	bool canDrawAt(XY position) { return position.x >= 0 ? (position.x % 3) == 0 : false; }
};
class PatternVertical3px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_vert_3px.png"; }
	std::string getName() override { return "Vertical - 3px dist."; }
	bool canDrawAt(XY position) { return position.x >= 0 ? (position.x % 4) == 0 : false; }
};
class PatternVertical4px : public Pattern
{
	std::string getIconPath() override { return "assets/pattern_vert_4px.png"; }
	std::string getName() override { return "Vertical - 4px dist."; }
	bool canDrawAt(XY position) { return position.x >= 0 ? (position.x % 5) == 0 : false; }
};