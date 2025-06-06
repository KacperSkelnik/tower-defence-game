//
// Created by Kacper Skelnik on 25.01.2025.
//

#include "Screen.h"

#include "Settings.h"
#include <cassert>

namespace Screen {

    static Window* s_instance = nullptr;

    void Window::init(const unsigned initialWidth, const unsigned initialHeight) {
        assert(!s_instance);

        const auto width  = static_cast<float>(initialWidth);
        const auto height = static_cast<float>(initialHeight);

        auto calculateViewRectangle = [width, height](const sf::FloatRect viewport) {
            return sf::FloatRect(
                {
                    viewport.position.x * width,
                    viewport.position.y * height,
                },
                {
                    viewport.size.x * width,
                    viewport.size.y * height,
                }
            );
        };

        constexpr auto mainViewport   = sf::FloatRect({0, 0.}, {0.8, 0.9});
        constexpr auto bottomViewport = sf::FloatRect({0, 0.9}, {0.8, 0.1});
        constexpr auto rightViewport  = sf::FloatRect({0.8, 0}, {0.2, 1});

        s_instance = new Window();

        s_instance->window = sf::RenderWindow(sf::VideoMode({initialWidth, initialHeight}), "Tower Defence");

        s_instance->mainView = sf::View(calculateViewRectangle(mainViewport));
        s_instance->mainView.setViewport(mainViewport);

        s_instance->bottomView = sf::View(calculateViewRectangle(bottomViewport));
        s_instance->bottomView.setViewport(bottomViewport);

        s_instance->rightView = sf::View(calculateViewRectangle(rightViewport));
        s_instance->rightView.setViewport(rightViewport);

        s_instance->draggingViewClock.reset();
        s_instance->draggingViewClock.start();

        s_instance->draggingViewManualMousePosition = std::nullopt;

        s_instance->zoomsCnt = 0;
    }

    bool Window::isMouseOn(const sf::Vector2i& mousePosition, const sf::View& view) {
        const auto [xStart, yStart]                    = view.getViewport().position;
        const auto [windowWidth, windowHeight]         = get().getSize();
        const auto [widthProportion, heightProportion] = view.getViewport().size;
        const float width                              = widthProportion * static_cast<float>(windowWidth);
        const float height                             = heightProportion * static_cast<float>(windowHeight);

        const float x = static_cast<float>(windowWidth) * xStart;
        const float y = static_cast<float>(windowHeight) * yStart;

        if (x < static_cast<float>(mousePosition.x) && x + width > static_cast<float>(mousePosition.x) &&
            y < static_cast<float>(mousePosition.y) && y + height > static_cast<float>(mousePosition.y)) {
            return true;
        }
        return false;
    }

    void Window::shutDown() {
        assert(s_instance);

        delete s_instance;

        s_instance = nullptr;
    }

    sf::RenderWindow& Window::get() {
        assert(s_instance);

        return s_instance->window;
    }

    sf::View& Window::getMainView() {
        assert(s_instance);

        return s_instance->mainView;
    }

    void Window::mainViewFocus() {
        assert(s_instance);

        s_instance->window.setView(s_instance->mainView);
    }

    bool Window::isMouseOnMainView(const sf::Vector2i& mousePosition) {
        assert(s_instance);

        return isMouseOn(mousePosition, s_instance->mainView);
    }

    sf::View& Window::getBottomView() {
        assert(s_instance);

        return s_instance->bottomView;
    }

    void Window::bottomViewFocus() {
        assert(s_instance);

        s_instance->window.setView(s_instance->bottomView);
    }

    bool Window::isMouseOnBottomView(const sf::Vector2i& mousePosition) {
        assert(s_instance);

        return isMouseOn(mousePosition, s_instance->bottomView);
    }

    sf::View& Window::getRightView() {
        assert(s_instance);

        return s_instance->rightView;
    }

    void Window::rightViewFocus() {
        assert(s_instance);

        s_instance->window.setView(s_instance->rightView);
    }

    bool Window::isMouseOnRightView(const sf::Vector2i& mousePosition) {
        assert(s_instance);

        return isMouseOn(mousePosition, s_instance->rightView);
    }

    void Window::dragMainView() {
        assert(s_instance);
        using namespace Settings;

        auto move = [](const sf::Vector2f offset, const bool canMove) {
            if (canMove) {
                mainViewFocus();
                getMainView().move(offset);
            }
        };

        bool isMouseInsideDraggingArea = false;

        const auto [x, y] = sf::Mouse::getPosition(s_instance->window);
        if (isMouseOnMainView({x, y})) {

            const float draggingPart                   = Variables::getViewDraggingPart();
            const auto [viewportWidth, viewportHeight] = s_instance->mainView.getViewport().size;
            const float viewWidth      = viewportWidth * static_cast<float>(Variables::getWindowWidth());
            const float viewHeight     = viewportHeight * static_cast<float>(Variables::getWindowHeight());
            const float draggingWidth  = draggingPart * viewWidth;
            const float draggingHeight = draggingPart * viewHeight;

            bool canMove = false;
            if (s_instance->draggingViewClock.getElapsedTime() >= Variables::getViewDraggingTime()) {
                canMove = true;
            }

            if (static_cast<float>(x) < draggingWidth) {
                move({-Variables::getViewDraggingOffset(), 0}, canMove);
                isMouseInsideDraggingArea = true;
            }
            if (static_cast<float>(x) > viewWidth - draggingWidth) {
                move({Variables::getViewDraggingOffset(), 0}, canMove);
                isMouseInsideDraggingArea = true;
            }
            if (static_cast<float>(y) < draggingHeight) {
                move({0, -Variables::getViewDraggingOffset()}, canMove);
                isMouseInsideDraggingArea = true;
            }
            if (static_cast<float>(y) > viewHeight - draggingHeight) {
                move({0, Variables::getViewDraggingOffset()}, canMove);
                isMouseInsideDraggingArea = true;
            }
        }

        if (!isMouseInsideDraggingArea) {
            s_instance->draggingViewClock.restart();
        }
    }

    void Window::dragMainViewManually(const bool screenCanBeDragged) {
        assert(s_instance);

        if (screenCanBeDragged) {
            if (s_instance->draggingViewManualMousePosition) {
                const sf::Vector2i mousePosition = sf::Mouse::getPosition(s_instance->window);
                mainViewFocus();
                getMainView().move({
                    static_cast<float>(s_instance->draggingViewManualMousePosition->x - mousePosition.x),
                    static_cast<float>(s_instance->draggingViewManualMousePosition->y - mousePosition.y),
                });
                s_instance->draggingViewManualMousePosition = mousePosition;
            } else {
                s_instance->draggingViewManualMousePosition = sf::Mouse::getPosition(s_instance->window);
            }
        }
    }

    void Window::resetDraggingViewManualMousePosition() {
        assert(s_instance);

        s_instance->draggingViewManualMousePosition = std::nullopt;
    }

    int Window::getZoomsCnt() {
        assert(s_instance);

        return s_instance->zoomsCnt;
    }

    void Window::increaseZoomsCnt() {
        assert(s_instance);

        s_instance->zoomsCnt++;
    }

    void Window::decreaseZoomsCnt() {
        assert(s_instance);

        s_instance->zoomsCnt--;
    }
} // namespace Screen
