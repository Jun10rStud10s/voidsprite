#pragma once
#include "globals.h"
#include "mathops.h"
class Layer
{
public:
	uint8_t* pixelData;	//!!! THIS IS IN ARGB
	std::vector<uint8_t*> undoQueue;
	std::vector<uint8_t*> redoQueue;
	int w, h;
	SDL_Texture* tex = NULL;
	bool layerDirty = true;

	std::string name = "Layer";
	bool hidden = false;

	bool colorKeySet = false;
	uint32_t colorKey = 0;

	Layer(int width, int height) {
		w = width;
		h = height;
		pixelData = (uint8_t*)malloc(width * height * 4);
		if (pixelData != NULL) {
			memset(pixelData, 0, width * height * 4);
			tex = SDL_CreateTexture(g_rd, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
			SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
		}
	}
	Layer(SDL_Surface* from) : Layer(from->w, from->h) {
		//pixelData = (uint8_t*)malloc(w * h * 4);
		if (from->format->format == SDL_PIXELFORMAT_ARGB8888) {
			memcpy(pixelData, from->pixels, w * h * 4);
		}
		else {
			SDL_ConvertPixels(w, h, from->format->format, from->pixels, from->pitch, SDL_PIXELFORMAT_ARGB8888, pixelData, w * 4);
			SDL_FreeSurface(from);
		}
	}

	~Layer() {
		free(pixelData);
		for (uint8_t*& u : undoQueue) {
			free(u);
		}
		for (uint8_t*& r : redoQueue) {
			free(r);
		}
		SDL_DestroyTexture(tex);
	}

	void updateTexture() {
		uint8_t* pixels;
		int pitch;
		SDL_LockTexture(tex, NULL, (void**)&pixels, &pitch);
		memcpy(pixels, pixelData, w * h * 4);
		SDL_UnlockTexture(tex);
		layerDirty = false;
	}

	void render(SDL_Rect where) {
		if (layerDirty) {
			updateTexture();
		}
		SDL_RenderCopy(g_rd, tex, NULL, &where);
	}

	void blit(Layer* sourceLayer, XY position);

	void setPixel(XY position, uint32_t color) {
		uint32_t* intpxdata = (uint32_t*)pixelData;
		if (position.x >= 0 && position.x < w
			&& position.y >= 0 && position.y < h) {
			intpxdata[position.x + (position.y * w)] = color;
			layerDirty = true;
		}
	}
	void fillRect(XY from, XY to, uint32_t color) {
		int minx = ixmin(from.x, to.x);
		int maxx = ixmax(from.x, to.x);
		int miny = ixmin(from.y, to.y);
		int maxy = ixmax(from.y, to.y);

		for (int x = minx; x <= maxx; x++) {
			for (int y = miny; y <= maxy; y++) {
				setPixel(XY{ x,y }, color);
			}
		}
	}

	unsigned int getPixelAt(XY position) {
		if (position.x >= 0 && position.x < w
			&& position.y >= 0 && position.y < h) {
			uint32_t* intpxdata = (uint32_t*)pixelData;
			return intpxdata[position.x + (position.y * w)];
		}
		else {
			return 0xFF000000;
		}
	}

	void flipHorizontally() {
		uint32_t* px32 = (uint32_t*)pixelData;
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w / 2; x++) {
				uint32_t p = *(px32 + (y*w) + (w - 1 - x));
				*(px32 + (y*w) + (w - 1 - x)) = *(px32 + (y*w) + x);
				*(px32 + (y*w) + x) = p;
			}
		}
		layerDirty = true;
	}
	void flipVertically() {
		uint32_t* px32 = (uint32_t*)pixelData;
		for (int y = 0; y < h/2; y++) {
			for (int x = 0; x < w; x++) {
				uint32_t p = *(px32 + (y*w) + (w - 1 - x));
				uint32_t* p2 = px32 + ((h-1-y)*w) + (w - 1 - x);
				*(px32 + (y * w) + (w - 1 - x)) = *p2;
				*p2 = p;
			}
		}
		layerDirty = true;
	}

	void discardRedoStack() {
		for (uint8_t*& redoD : redoQueue) {
			free(redoD);
		}
		redoQueue.clear();
	}
	void discardLastUndo() {
		free(undoQueue[0]);
		undoQueue.erase(undoQueue.begin());
	}
	void commitStateToUndoStack() {
		discardRedoStack();
		uint8_t* copiedPixelData = (uint8_t*)malloc(w * h * 4);
		if (copiedPixelData != NULL) {
			memcpy(copiedPixelData, pixelData, w * h * 4);
			undoQueue.push_back(copiedPixelData);
		}
		else {
			printf("malloc FAILED we are FUCKED\n");
		}
	}
	void undo() {
		if (!undoQueue.empty()) {
			redoQueue.push_back(pixelData);
			pixelData = undoQueue[undoQueue.size() - 1];
			undoQueue.pop_back();
			layerDirty = true;
		}
	}
	void redo() {
		if (!redoQueue.empty()) {
			undoQueue.push_back(pixelData);
			pixelData = redoQueue[redoQueue.size()-1];
			redoQueue.pop_back();
			layerDirty = true;
		}
	}

	unsigned int numUniqueColors(bool onlyRGB = false) {
		return (unsigned int)getUniqueColors(onlyRGB).size();
	}

	std::vector<uint32_t> getUniqueColors(bool onlyRGB = false) {
		std::map<uint32_t, int> cols;
		uint32_t* pixels = (uint32_t*)pixelData;
		for (uint64_t x = 0; x < w * h; x++) {
			uint32_t px = pixels[x];
			if (onlyRGB) {
				px |= 0xff000000;
			}
			else {
				if ((px & 0xff000000) == 0) {
					px = 0;
				}
			}
			cols[px] = 1;
		}
		std::vector<uint32_t> ret;
		for (auto& a : cols) {
			ret.push_back(a.first);
		}
		return ret;
	}

	Layer* copyScaled(XY dimensions);

	void setAllAlpha255() {
		uint32_t* px32 = (uint32_t*)pixelData;
		for (uint64_t x = 0; x < w * h; x++) {
			px32[x] |= 0xff000000;
		}
		layerDirty = true;
	}
};

