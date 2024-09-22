#include "DraggablePanel.h"

bool DraggablePanel::isMouseIn(XY thisPositionOnScreen, XY mousePos)
{
    return pointInBox(mousePos, SDL_Rect{ thisPositionOnScreen.x, thisPositionOnScreen.y, wxWidth, wxHeight }) || subWidgets.mouseInAny(thisPositionOnScreen, mousePos);
}

void DraggablePanel::render(XY position)
{
    subWidgets.renderAll(position);
}

void DraggablePanel::handleInput(SDL_Event evt, XY gPosOffset)
{
    if (!DrawableManager::processInputEventInMultiple({ subWidgets }, evt, gPosOffset)) {
        processDrag(evt);
    }
}

void DraggablePanel::mouseHoverMotion(XY mousePos, XY gPosOffset)
{
    subWidgets.processHoverEvent(xyAdd(gPosOffset, position), mousePos);
}

void DraggablePanel::processDrag(SDL_Event evt)
{
    switch (evt.type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if (evt.button.button == SDL_BUTTON_LEFT) {
            dragging = evt.button.state;
        }
        break;
    case SDL_MOUSEMOTION:
        if (dragging) {
            position.x += evt.motion.xrel;
            position.y += evt.motion.yrel;

            if (position.x < 0) {
                position.x = 0;
            }
            if (position.y < 0) {
                position.y = 0;
            }
            if (position.x + wxWidth > g_windowW) {
                position.x = g_windowW - wxWidth;
            }
            if (position.y + wxHeight > g_windowH) {
                position.y = g_windowH - wxHeight;
            }
        }
        break;
    }
}
