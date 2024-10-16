#include "NineSegmentPatternEditorScreen.h"
#include "maineditor.h"
#include "FileIO.h"
#include "Notification.h"

NineSegmentPatternEditorScreen::NineSegmentPatternEditorScreen(MainEditor* parent) {
    caller = parent;

    pointUL = { caller->texW / 3, caller->texH / 3 };
    pointUR = { caller->texW / 3 * 2, caller->texH / 3 * 2 };

    navbar = new ScreenWideNavBar<NineSegmentPatternEditorScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {},
                    {
                        {SDLK_c, { "Close",
                                [](NineSegmentPatternEditorScreen* screen) {
                                    screen->closeNextTick = true;
                                }
                            }
                        },
                        {SDLK_s, { "Export to 9-segment pattern",
                                [](NineSegmentPatternEditorScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".void9sp", "9-segment pattern file"} }, "save 9-segment pattern", EVENT_9SPEDITOR_SAVE);
                                }
                            }
                        },
                    },
                    g_iconNavbarTabFile
                }
            },
        }, { SDLK_f });
    wxsManager.addDrawable(navbar);
}

void NineSegmentPatternEditorScreen::render()
{
    drawBackground();

    SDL_Rect canvasRenderRect = { canvasDrawOrigin.x, canvasDrawOrigin.y, caller->texW * canvasZoom, caller->texH * canvasZoom };
    for (Layer*& l : caller->layers) {
        SDL_RenderCopy(g_rd, l->tex, NULL, &canvasRenderRect);
    }
    SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0xD0);
    SDL_Rect r2 = canvasRenderRect;
    r2.x -= 1;
    r2.y -= 1;
    r2.w += 2;
    r2.h += 2;
    SDL_RenderDrawRect(g_rd, &r2);

    SDL_SetRenderDrawColor(g_rd, 255,255,255, 0x80);
    //vlines
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x + pointUL.x * canvasZoom, canvasRenderRect.y,
        canvasRenderRect.x + pointUL.x * canvasZoom, canvasRenderRect.y + caller->texH * canvasZoom);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x + pointUR.x * canvasZoom, canvasRenderRect.y,
        canvasRenderRect.x + pointUR.x * canvasZoom, canvasRenderRect.y + caller->texH * canvasZoom);

    //hlines
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x, canvasRenderRect.y + pointUL.y * canvasZoom,
		canvasRenderRect.x + caller->texW * canvasZoom, canvasRenderRect.y + pointUL.y * canvasZoom);
    SDL_RenderDrawLine(g_rd, canvasRenderRect.x, canvasRenderRect.y + pointUR.y * canvasZoom,
        canvasRenderRect.x + caller->texW * canvasZoom, canvasRenderRect.y + pointUR.y * canvasZoom);

    BaseScreen::render();
}

void NineSegmentPatternEditorScreen::tick()
{
    if (closeNextTick) {
        g_closeScreen(this);
        return;
    }

    canvasDrawOrigin = XY{
        iclamp(-caller->texW * canvasZoom + 4, canvasDrawOrigin.x, g_windowW - 4),
        iclamp(-caller->texH * canvasZoom + 4, canvasDrawOrigin.y, g_windowH - 4)
    };
}

void NineSegmentPatternEditorScreen::takeInput(SDL_Event evt)
{
    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);

    if (evt.type == SDL_QUIT) {
        g_closeScreen(this);
        return;
    }

    LALT_TO_SUMMON_NAVBAR;

    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {
        switch (evt.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = true;
            }
            else if (evt.button.button == SDL_BUTTON_LEFT) {

            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (evt.button.button == SDL_BUTTON_MIDDLE) {
                scrollingCanvas = false;
            }
            break;
        case SDL_MOUSEMOTION:
            if (scrollingCanvas) {
                canvasDrawOrigin = xyAdd(canvasDrawOrigin, XY{ evt.motion.xrel, evt.motion.yrel });
            }
            break;
        case SDL_MOUSEWHEEL:
            canvasZoom += evt.wheel.y;
            canvasZoom = ixmax(1, canvasZoom);
            break;
        }
    }
}

BaseScreen* NineSegmentPatternEditorScreen::isSubscreenOf()
{
    return caller;
}

void NineSegmentPatternEditorScreen::eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex)
{
    if (evt_id == EVENT_9SPEDITOR_SAVE) {
        Layer* l = caller->flattenImage();
        if (l != NULL) {
            if (write9SegmentPattern(name, l, pointUL, pointUR)) {
                g_addNotification(SuccessNotification("Success", "9-segment pattern saved"));
            }
            else {
                g_addNotification(ErrorNotification("Error", "Failed to export pattern"));
            }
            delete l;
        }
        else {
            g_addNotification(ErrorNotification("Error", "Failed to flatten image"));
        }
    }
}

void NineSegmentPatternEditorScreen::drawBackground()
{
    if (g_config.animatedBackground) {
        uint64_t now = g_config.animatedBackground >= 3 ? 0 : SDL_GetTicks64();
        uint64_t progress = now % 120000;
        for (int y = -(1.0 - progress / 120000.0) * g_windowH; y < g_windowH; y += 50) {
            if (y >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x22);
                SDL_RenderDrawLine(g_rd, 0, y, g_windowW, y);
            }
        }

        for (int x = -(1.0 - (now % 100000) / 100000.0) * g_windowW; x < g_windowW; x += 30) {
            if (x >= 0) {
                SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x19);
                SDL_RenderDrawLine(g_rd, x, 0, x, g_windowH);
            }
        }
    }
}
