#include "maineditor.h"
#include "FontRenderer.h"
#include "EditorBrushPicker.h"
#include "EditorLayerPicker.h"
#include "GlobalNavBar.h"
#include "PopupYesNo.h"

MainEditor::MainEditor(XY dimensions) {
	SetUpWidgets();

	texW = dimensions.x;
	texH = dimensions.y;
	//canvasCenterPoint = XY{ texW / 2, texH / 2 };
	layers.push_back(new Layer(texW, texH));
	FillTexture();
	recenterCanvas();
	initLayers();
}
MainEditor::MainEditor(SDL_Surface* srf) {
	SetUpWidgets();

	//todo i mean just use MainEditor(Layer*) here
	texW = srf->w;
	texH = srf->h;

	Layer* nlayer = new Layer(texW, texH);
	layers.push_back(nlayer);
	SDL_ConvertPixels(srf->w, srf->h, srf->format->format, srf->pixels, srf->pitch, SDL_PIXELFORMAT_ARGB8888, nlayer->pixelData, texW*4);
	recenterCanvas();
	initLayers();
}

MainEditor::MainEditor(Layer* layer)
{
	SetUpWidgets();

	texW = layer->w;
	texH = layer->h;

	layers.push_back(layer);
	recenterCanvas();
	initLayers();
}

MainEditor::MainEditor(std::vector<Layer*> layers)
{
	SetUpWidgets();
	texW = layers[0]->w;
	texH = layers[0]->h;
	this->layers = layers;
	recenterCanvas();
	initLayers();
}

MainEditor::~MainEditor() {
	//printf("hello from destructor\n");
	wxsManager.freeAllDrawables();
	for (Layer*& imgLayer : layers) {
		delete imgLayer;
	}
}

void MainEditor::render() {
	SDL_SetRenderDrawColor(g_rd, backgroundColor.r/6*5, backgroundColor.g/6*5, backgroundColor.b/6*5, 255);
	SDL_RenderClear(g_rd);
	DrawBackground();

	SDL_Rect canvasRenderRect;
	canvasRenderRect.w = texW * scale;
	canvasRenderRect.h = texH * scale;
	canvasRenderRect.x = canvasCenterPoint.x;
	canvasRenderRect.y = canvasCenterPoint.y;

	for (Layer*& imgLayer : layers) {
		if (!imgLayer->hidden) {
			imgLayer->render(canvasRenderRect);
		}
	}

	if (tileDimensions.x != 0) {
		int dx = canvasRenderRect.x;
		while (dx < g_windowW && dx < canvasRenderRect.x + canvasRenderRect.w) {
			dx += tileDimensions.x * scale;
			if (dx >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, dx, canvasRenderRect.y, dx, canvasRenderRect.y + canvasRenderRect.h);
			}
		}
	}
	if (tileDimensions.y != 0) {
		int dy = canvasRenderRect.y;
		while (dy < g_windowH && dy < canvasRenderRect.y + canvasRenderRect.h) {
			dy += tileDimensions.y * scale;
			if (dy >= 0) {
				SDL_SetRenderDrawColor(g_rd, 0xff-backgroundColor.r, 0xff-backgroundColor.g, 0xff-backgroundColor.b, tileGridAlpha);
				SDL_RenderDrawLine(g_rd, canvasRenderRect.x, dy, canvasRenderRect.x + canvasRenderRect.w, dy);
			}
		}
	}
	if (currentBrush != NULL) {
		currentBrush->renderOnCanvas(XY{ canvasRenderRect.x, canvasRenderRect.y }, scale);
	}

	//g_fnt->RenderString(std::string("Scale: ") + std::to_string(scale), 0, 20);
	//g_fnt->RenderString(std::string("MousePixelPoint: ") + std::to_string(mousePixelTargetPoint.x) + std::string(":") + std::to_string(mousePixelTargetPoint.y), 0, 50);

	

	if (wxsManager.anyFocused()) {
		SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0x80);
		SDL_RenderFillRect(g_rd, NULL);
	}

	DrawForeground();

	wxsManager.renderAll();
}

void MainEditor::tick() {
	canvasCenterPoint = XY{
		iclamp(-texW * scale + 4, canvasCenterPoint.x, g_windowW - 4),
		iclamp(-texH * scale +4, canvasCenterPoint.y, g_windowH - 4)
	};

	//fuck it we ball
	layerPicker->position.x = g_windowW - 260;

	if (closeNextTick) {
		g_closeScreen(this);
	}
}

void MainEditor::DrawBackground()
{
	int lineX = 400;
	for (int x = 40 + (SDL_GetTicks64()%5000/5000.0 * 60); x < g_windowW + lineX; x += 60) {
		SDL_SetRenderDrawColor(g_rd, 0xff-backgroundColor.r, 0xff-backgroundColor.g, 0xff-backgroundColor.b, 0x40);
		SDL_RenderDrawLine(g_rd, x, 0, x - lineX, g_windowH);
		SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, 0x0d);
		SDL_RenderDrawLine(g_rd, g_windowW - x, 0, g_windowW - x + lineX/4*6, g_windowH);
	}


	int lw = texW * scale + 2;
	int lh = texH * scale + 2;
	SDL_Rect r = { canvasCenterPoint.x - 1, canvasCenterPoint.y - 1, lw, lh };
	uint8_t a = 0xff;
	SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
	SDL_RenderDrawRect(g_rd, &r);
	for (int x = 0; x < 6; x++) {
		r.w += 2;
		r.h += 2;
		r.x -= 1;
		r.y -= 1;
		a /= 2;
		SDL_SetRenderDrawColor(g_rd, 0xff - backgroundColor.r, 0xff - backgroundColor.g, 0xff - backgroundColor.b, a);
		SDL_RenderDrawRect(g_rd, &r);
	}
	
}

void MainEditor::DrawForeground()
{
	SDL_Rect r = { 0, g_windowH - 30, g_windowW, 30 };
	SDL_SetRenderDrawColor(g_rd, 0, 0, 0, 0xb0);
	SDL_RenderFillRect(g_rd, &r);

	g_fnt->RenderString(std::format("{}x{} ({}%)", texW, texH, scale * 100), 2, g_windowH - 28, SDL_Color{255,255,255,0xa0});

	g_fnt->RenderString(std::format("{}:{}", mousePixelTargetPoint.x, mousePixelTargetPoint.y), 200, g_windowH - 28, SDL_Color{255,255,255,0xd0});

	if (currentBrush != NULL) {
		g_fnt->RenderString(std::format("{} {}", currentBrush->getName(), eraserMode ? "(Erase)" : ""), 350, g_windowH - 28, SDL_Color{ 255,255,255,0xa0 });
	}
}

void MainEditor::initLayers()
{
	for (Layer*& l : layers) {
		l->commitStateToUndoStack();
	}

	layerPicker->updateLayers();
}

void MainEditor::SetUpWidgets()
{
	colorPicker = new EditorColorPicker(this);
	colorPicker->position.y = 80;
	colorPicker->position.x = 10;
	wxsManager.addDrawable(colorPicker);
	colorPicker->setMainEditorColorRGB(pickedColor);

	brushPicker = new EditorBrushPicker(this);
	brushPicker->position.y = 480;
	brushPicker->position.x = 10;
	wxsManager.addDrawable(brushPicker);

	layerPicker = new EditorLayerPicker(this);
	layerPicker->position = XY{ 440, 80 };
	layerPicker->anchor = XY{ 1,0 };
	wxsManager.addDrawable(layerPicker);

	navbar = new GlobalNavBar(this);
	wxsManager.addDrawable(navbar);
}

void MainEditor::RecalcMousePixelTargetPoint(int x, int y) {
	mousePixelTargetPoint =
		XY{
			(canvasCenterPoint.x - x) / -scale,
			(canvasCenterPoint.y - y) / -scale
	};
}

bool MainEditor::requestSafeClose() {
	if (!changesSinceLastSave) {
		closeNextTick = true;
		return true;
	}
	else {
		PopupYesNo* newPopup = new PopupYesNo("Confirm close", "You have unsaved changes");
		newPopup->setCallbackListener(EVENT_MAINEDITOR_CONFIRM_CLOSE, this);
		g_addPopup(newPopup);
	}
	return false;
}

void MainEditor::takeInput(SDL_Event evt) {

	if (evt.type == SDL_MOUSEBUTTONDOWN && evt.button.state) {
		wxsManager.tryFocusOnPoint(XY{ evt.button.x, evt.button.y });
	}
	if (evt.type == SDL_QUIT) {
		if (requestSafeClose()) {
			return;
		}
	}

	if (!wxsManager.anyFocused()) {
		switch (evt.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				if (evt.button.button == 1) {
					RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
					if (currentBrush != NULL) {
						if (evt.button.state) {
							if (!currentBrush->isReadOnly()) {
								commitStateToCurrentLayer();
							}
							currentBrush->clickPress(this, mousePixelTargetPoint);
						}
						else {
							currentBrush->clickRelease(this, mousePixelTargetPoint);
						}
					}
					mouseHoldPosition = mousePixelTargetPoint;
					leftMouseHold = evt.button.state;
				}
				else if (evt.button.button == 2) {
					middleMouseHold = evt.button.state;
				}
				else if (evt.button.button == 3) {
					RecalcMousePixelTargetPoint(evt.button.x, evt.button.y);
					if (currentBrush != NULL && currentBrush->overrideRightClick()) {
						if (evt.button.state) {
							currentBrush->rightClickPress(this, mousePixelTargetPoint);
						}
					}
					else {
						colorPicker->setMainEditorColorRGB(getCurrentLayer()->getPixelAt(mousePixelTargetPoint));
					}
				}
				break;
			case SDL_MOUSEMOTION:
				RecalcMousePixelTargetPoint(evt.motion.x, evt.motion.y);
				if (middleMouseHold) {
					canvasCenterPoint.x += evt.motion.xrel;
					canvasCenterPoint.y += evt.motion.yrel;
				}
				else if (leftMouseHold) {
					if (currentBrush != NULL) {
						currentBrush->clickDrag(this, mouseHoldPosition, mousePixelTargetPoint);
					}
					mouseHoldPosition = mousePixelTargetPoint;
				}
				if (currentBrush != NULL) {
					currentBrush->mouseMotion(this, mousePixelTargetPoint);
				}
				break;
			case SDL_MOUSEWHEEL:
				scale += evt.wheel.y;
				scale = scale < 1 ? 1 : scale;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
					case SDLK_LALT:
						wxsManager.forceFocusOn(navbar);
						break;
					case SDLK_e:
						colorPicker->toggleEraser();
						break;
					case SDLK_RCTRL:
						middleMouseHold = !middleMouseHold;
						break;
				}
				break;
		}
	}
	else {
		wxsManager.passInputToFocused(evt);
	}
}

void MainEditor::eventFileSaved(int evt_id, PlatformNativePathString name)
{
	if (evt_id == EVENT_MAINEDITOR_SAVEFILE) {
		printf("eventFileSaved: got file name %ls\n", name.c_str());

		size_t extStart = name.find_last_of(L".");
		std::wstring extension = name.substr(extStart);
		bool result = false;
		if (extension == L".voidsn") {
			result = writeVOIDSNv1(name, XY{ texW, texH }, layers);
		}
		else {
			Layer* flat = flattenImage();
			if (extension == L".xyz") {
				result = writeXYZ(name, flat);
			}
			else if (extension == L".bmp") {
				result = writeBMP(name, flat);
			}
			else if (extension == L".pbm") {
				result = writeCaveStoryPBM(name, flat);
			}
			else {
				result = writePNG(name, flat);
			}
			delete flat;
		}
		if (result) {
			g_addPopup(new PopupMessageBox("Saved", "Save successful"));
			lastConfirmedSave = true;
			lastConfirmedSavePath = name;
			changesSinceLastSave = false;
		}
		else {
			g_addPopup(new PopupMessageBox("Save", "Save failed"));
		}
		
	}
}

void MainEditor::eventPopupClosed(int evt_id, BasePopup* p)
{
	if (evt_id == EVENT_MAINEDITOR_CONFIRM_CLOSE) {
		if (((PopupYesNo*)p)->result) {
			g_closeScreen(this);
		}
	}
}

void MainEditor::FillTexture() {
	int* pixels = (int*)getCurrentLayer()->pixelData;
	//int pitch;
	//SDL_LockTexture(mainTexture, NULL, (void**)&pixels, &pitch);
	for (int x = 0; x < texW; x++) {
		for (int y = 0; y < texH; y++) {
			pixels[x + (y * texW)] = 0x00000000;
		}
	}
	//SDL_UnlockTexture(mainTexture);
}

void MainEditor::SetPixel(XY position, uint32_t color) {
	getCurrentLayer()->setPixel(position, color & (eraserMode ? 0xffffff : 0xffffffff));
}

void MainEditor::DrawLine(XY from, XY to, uint32_t color) {
	if (from.x == to.x) {
		int yMin = from.y > to.y ? to.y : from.y;
		int yMax = from.y > to.y ? from.y : to.y;
		for (int y = yMin; y <= yMax; y++) {
			SetPixel(XY{ from.x, y }, color);
		}
	}
	else {
		float a = (float)(from.y - to.y) / (from.x - to.x);
		float b = from.y - a * from.x;

		int xMin = from.x > to.x ? to.x : from.x;
		int xMax = from.x > to.x ? from.x : to.x;

		for (int x = xMin; x <= xMax; x++) {
			int yPos = (int)(a * x + b);
			SetPixel(XY{ x, yPos }, color);
		}
	}
}

void MainEditor::trySaveImage()
{
	if (!lastConfirmedSave) {
		trySaveAsImage();
	}
	else {
		eventFileSaved(EVENT_MAINEDITOR_SAVEFILE, lastConfirmedSavePath);
	}
}

void MainEditor::trySaveAsImage()
{
	platformTrySaveImageFile(this);
}

void MainEditor::recenterCanvas()
{
	canvasCenterPoint = XY{
		(g_windowW / 2) - (texW*scale)/2,
		(g_windowH / 2) - (texH*scale)/2
	};
}

void MainEditor::checkAndDiscardEndOfUndoStack()
{
	if (undoStack.size() > maxUndoHistory) {
		UndoStackElement l = undoStack[0];
		switch (l.type) {
			case UNDOSTACK_LAYER_DATA_MODIFIED:
				l.targetlayer->discardLastUndo();
				break;
			case UNDOSTACK_DELETE_LAYER:
				delete l.targetlayer;
				break;
		}
		
		undoStack.erase(undoStack.begin());
	}
}

void MainEditor::commitStateToCurrentLayer()
{
	discardRedoStack();

	//printf("commit undo state\n");
	getCurrentLayer()->commitStateToUndoStack();
	undoStack.push_back(UndoStackElement{ getCurrentLayer(), UNDOSTACK_LAYER_DATA_MODIFIED });
	checkAndDiscardEndOfUndoStack();
	changesSinceLastSave = true;
}

void MainEditor::discardRedoStack()
{
	for (Layer*& x : layers) {
		x->discardRedoStack();
	}

	//clear redo stack
	for (UndoStackElement& l : redoStack) {
		if (l.type == UNDOSTACK_CREATE_LAYER) {
			delete l.targetlayer;
		}
	}
	redoStack.clear();
}

void MainEditor::undo()
{
	if (!undoStack.empty()) {
		UndoStackElement l = undoStack[undoStack.size() - 1];
		undoStack.pop_back();
		redoStack.push_back(l);
		switch (l.type) {
			case UNDOSTACK_LAYER_DATA_MODIFIED:
				l.targetlayer->undo();
				break;
			case UNDOSTACK_CREATE_LAYER:
				//remove layer from list
				for (int x = 0; x < layers.size(); x++) {
					if (layers[x] == l.targetlayer) {
						layers.erase(layers.begin() + x);
						break;
					}
				}
				layerPicker->updateLayers();
				break;
			case UNDOSTACK_DELETE_LAYER:
				//add layer to list
				layers.insert(layers.begin() + l.extdata, l.targetlayer);
				layerPicker->updateLayers();
				break;
		}
	}
}

void MainEditor::redo()
{
	if (!redoStack.empty()) {
		UndoStackElement l = redoStack[redoStack.size() - 1];
		undoStack.push_back(l);
		redoStack.pop_back();
		switch (l.type) {
		case UNDOSTACK_LAYER_DATA_MODIFIED:
			l.targetlayer->redo();
			break;
		case UNDOSTACK_CREATE_LAYER:
			//add layer back to list
			layers.push_back(l.targetlayer);
			layerPicker->updateLayers();
			break;
		case UNDOSTACK_DELETE_LAYER:
			//add layer to list
			for (int x = 0; x < layers.size(); x++) {
				if (layers[x] == l.targetlayer) {
					layers.erase(layers.begin() + x);
					break;
				}
			}
			layerPicker->updateLayers();
			break;
		}
	}
}

void MainEditor::newLayer()
{
	Layer* nl = new Layer(texW, texH);
	nl->name = std::format("New Layer {}", layers.size()+1);
	layers.push_back(nl);

	UndoStackElement newUndoStack = {nl, UNDOSTACK_CREATE_LAYER};
	undoStack.push_back(newUndoStack);
	discardRedoStack();
	checkAndDiscardEndOfUndoStack();
	changesSinceLastSave = true;
}

void MainEditor::deleteLayer(int index) {
	if (layers.size() == 0) {
		return;
	}

	discardRedoStack();

	Layer* layerAtPos = layers[index];
	layers.erase(layers.begin() + index);
	if (selLayer >= layers.size()) {
		selLayer = layers.size() - 1;
	}

	UndoStackElement newUndoStack = { layerAtPos, UNDOSTACK_DELETE_LAYER, index };
	undoStack.push_back(newUndoStack);
	checkAndDiscardEndOfUndoStack();
	changesSinceLastSave = true;
}

void MainEditor::layer_flipHorizontally()
{
	commitStateToCurrentLayer();
	getCurrentLayer()->flipHorizontally();
}
void MainEditor::layer_flipVertically()
{
	commitStateToCurrentLayer();
	getCurrentLayer()->flipVertically();
}

void MainEditor::layer_swapLayerRGBtoBGR()
{
	commitStateToCurrentLayer();
	Layer* clayer = getCurrentLayer();
	uint8_t* convData = (uint8_t*)malloc(clayer->w * clayer->h * 4);
	SDL_ConvertPixels(clayer->w, clayer->h, SDL_PIXELFORMAT_ARGB8888, clayer->pixelData, clayer->w * 4, SDL_PIXELFORMAT_ABGR8888, convData, clayer->w * 4);
	free(clayer->pixelData);
	clayer->pixelData = convData;
	clayer->layerDirty = true;
}

uint32_t MainEditor::layer_getPixelAt(XY pos)
{
	return getCurrentLayer()->getPixelAt(pos);
}

Layer* MainEditor::flattenImage()
{
	Layer* ret = new Layer(texW, texH);
	int x = 0;
	for (Layer*& l : layers) {
		if (x++ == 0) {
			memcpy(ret->pixelData, l->pixelData, l->w * l->h * 4);
		}
		else {
			uint32_t* ppx = (uint32_t*)l->pixelData;
			uint32_t* retppx = (uint32_t*)ret->pixelData;
			for (uint64_t p = 0; p < l->w * l->h; p++) {
				uint32_t pixel = ppx[p];
				uint32_t srcPixel = retppx[p];
				retppx[p] = alphaBlend(srcPixel, pixel);
			}
		}
	}
	return ret;
}