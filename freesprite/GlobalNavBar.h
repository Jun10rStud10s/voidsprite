#pragma once
#include "globals.h"
#include "drawable.h"
#include "mathops.h"
#include "maineditor.h"
#include "FileIO.h"
#include "PopupMessageBox.h"
#include "PopupSetEditorPixelGrid.h"
#include "EventCallbackListener.h"

class GlobalNavBar : public Drawable, public EventCallbackListener
{
public:
	MainEditor* parent;
	int wxHeight = 30;
	DrawableManager wxs;
	DrawableManager subWxs;
	SDL_Keycode currentSubmenuOpen = -1;

	std::vector<SDL_Keycode> submenuOrder = {
		SDLK_f,
		SDLK_e,
		SDLK_l,
		SDLK_v,
	};

	std::map<SDL_Keycode, NavbarSection> keyBinds = 
	{ 
		{
			SDLK_f,
			{
				"File",
				{SDLK_s, SDLK_d, SDLK_c},
				{
					{SDLK_d, { "Save as",
							[](MainEditor* editor) {
								editor->trySaveAsImage();
							}
						}
					},
					{SDLK_s, { "Save",
							[](MainEditor* editor) {
								editor->trySaveImage();
							}
						}
					},
					{SDLK_c, { "Close",
							[](MainEditor* editor) {
								editor->requestSafeClose();
							}
						}
					},
				}
			}
		},
		{
			SDLK_e,
			{
				"Edit",
				{SDLK_z, SDLK_r},
				{
					{SDLK_z, { "Undo",
							[](MainEditor* editor) {
								editor->undo();
							}
						}
					},
					{SDLK_r, { "Redo",
							[](MainEditor* editor) {
								editor->redo();
							}
						}
					},
				}
			}
		},
		{
			SDLK_l,
			{
				"Layer",
				{},
				{
					{SDLK_f, { "Flip current layer: X axis",
							[](MainEditor* editor) {
								editor->layer_flipHorizontally();
							}
						}
					},
					{SDLK_g, { "Flip current layer: Y axis",
							[](MainEditor* editor) {
								editor->layer_flipVertically();
							}
						}
					},
					{SDLK_b, { "Swap channels RGB->BGR",
							[](MainEditor* editor) {
								editor->layer_swapLayerRGBtoBGR();
							}
						}
					},
					{SDLK_x, { "Print number of colors",
							[](MainEditor* editor) {
								printf("%i\n", editor->getCurrentLayer()->numUniqueColors(true));
							}
						}
					},
				}
			}
		},
		{
			SDLK_v,
			{
				"View",
				{},
				{
					{SDLK_r, { "Recenter canvas",
							[](MainEditor* editor) {
								editor->recenterCanvas();
							}
						}
					},
					{SDLK_b, { "Toggle background color",
							[](MainEditor* editor) {
								editor->backgroundColor.r = ~editor->backgroundColor.r;
								editor->backgroundColor.g = ~editor->backgroundColor.g;
								editor->backgroundColor.b = ~editor->backgroundColor.b;
							}
						}
					},
					{SDLK_g, { "Set pixel grid...",
							[](MainEditor* editor) {
								g_addPopup(new PopupSetEditorPixelGrid(editor, "Set pixel grid", "Enter grid size <w>x<h>:"));
							}
						}
					},
				}
			}
		}
	};

	GlobalNavBar(MainEditor* caller) {
		parent = caller;
		int x = 10;
		int xDist = 120;
		position = XY{ 0,0 };
		for (auto& editorSection : submenuOrder) {
			UIButton* sectionButton = new UIButton();
			sectionButton->position = { x, 1 };
			sectionButton->text = keyBinds[editorSection].name + std::format("({})", SDL_GetKeyName(editorSection));
			sectionButton->colorBGFocused = sectionButton->colorBGUnfocused = SDL_Color{ 0,0,0,0 };
			sectionButton->colorTextFocused = sectionButton->colorTextUnfocused = SDL_Color{ 255,255,255,0xa0 };
			sectionButton->wxWidth = xDist - 10;
			sectionButton->setCallbackListener(editorSection, this);
			wxs.addDrawable(sectionButton);
			x += xDist;
		}
	}

	bool isMouseIn(XY thisPositionOnScreen, XY mousePos) override
	{ 
		return mousePos.y <= wxHeight || subWxs.mouseInAny(thisPositionOnScreen, mousePos);
	}
	void render(XY position) override;
	void handleInput(SDL_Event evt, XY gPosOffset) override;
	void focusOut() override {
		Drawable::focusOut();
		openSubmenu(-1);
	}
	void eventButtonPressed(int evt_id) override;

	void tryPressHotkey(SDL_Keycode k);
	void openSubmenu(SDL_Keycode which);
	void doSubmenuAction(SDL_Keycode which);
	void updateCurrentSubmenu();
};

