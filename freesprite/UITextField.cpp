#include "UITextField.h"
#include "FontRenderer.h"

void UITextField::render(XY pos)
{
	SDL_Rect drawrect = { pos.x, pos.y, wxWidth, wxHeight };
	SDL_SetRenderDrawColor(g_rd, bgColor.r, bgColor.g, bgColor.b, focused ? 0xff : 0x80);
	SDL_RenderFillRect(g_rd, &drawrect);
	SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
	SDL_RenderDrawRect(g_rd, &drawrect);

	if (focused) {
		double lineAnimPercent = XM1PW3P1(focusTimer.percentElapsedTime(500));
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x80);
		drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
		drawLine(XY{ drawrect.x, drawrect.y }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
		drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x, drawrect.y + drawrect.h }, lineAnimPercent);
		drawLine(XY{ drawrect.x + drawrect.w, drawrect.y + drawrect.h }, XY{ drawrect.x + drawrect.w, drawrect.y }, lineAnimPercent);
	}
	
	g_fnt->RenderString(text + (focused ? "_" : ""), pos.x + 2, pos.y + 2, SDL_Color{0xff,0xff,0xff,(unsigned char)(focused ? 0xff : 0xa0)});
}

void UITextField::handleInput(SDL_Event evt, XY gPosOffset)
{
	if (evt.type == SDL_KEYDOWN) {
		switch (evt.key.keysym.sym) {
			case SDLK_TAB:
				break;
			case SDLK_RETURN:
				if (callback != NULL) {
					callback->eventTextInputConfirm(callback_id, text);
				}
				break;
			case SDLK_BACKSPACE:
				if (!text.empty()) {
					text = text.substr(0, text.size() - 1);
				}
				break;
			case SDLK_DELETE:
				text = "";
				break;
		}
	}
	else if (evt.type == SDL_TEXTINPUT) {
		bool textAdded = false;
		for (char& c : evt.text.text) {
			if (c == '\0') {
				break;
			}
			if ((numeric && c >= '0' && c <= '9')
				|| !numeric) {
				text += c;
				textAdded = true;
			}
		}
		if (textAdded && callback != NULL) {
			callback->eventTextInput(callback_id, text);
		}
	}
}

bool UITextField::isValidOrPartialColor()
{
	for (int x = 0; x < text.size(); x++) {
		char c = tolower(text[x]);
		if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c == '#' && x == 0))) {
			return false;
		}
	}
	return true;
}
