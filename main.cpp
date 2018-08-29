#include <stdio.h>
#include <SFML/Graphics.hpp>
#include <manymouse/manymouse.h>

using namespace std;

int main() {
    ManyMouseEvent mmevent;
    const int available_mice = ManyMouse_Init();
    int i;

    if (available_mice < 0) {
        printf("Error initializing ManyMouse!\n");
        ManyMouse_Quit();
        return 2;
    }
    printf("ManyMouse driver: %s\n", ManyMouse_DriverName());

    if (available_mice == 0) {
        printf("No mice detected!\n");
        ManyMouse_Quit();
        return 1;
    }

    for (i = 0; i < available_mice; i++) {
        printf("#%d: %s\n", i, ManyMouse_DeviceName(i));
    }
    printf("\n");

    sf::RenderWindow window(sf::VideoMode(800, 600), "MMSFML");
    vector<sf::CircleShape> circles;

    for(int i = 0; i < available_mice; i++) {
        sf::CircleShape circle;
        circle.setRadius(10);
        circle.setFillColor(sf::Color::Red);
        circle.setOrigin(10, 10);
        circle.setPosition(400, 300);
        circles.push_back(circle);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        while (ManyMouse_PollEvent(&mmevent)) {
            if (mmevent.type == MANYMOUSE_EVENT_RELMOTION) {
                printf("Mouse #%u relative motion %s %d\n", mmevent.device, mmevent.item == 0 ? "X" : "Y", mmevent.value);

                sf::CircleShape *circle = &circles[mmevent.device];
                sf::Vector2f vec = circle->getPosition();
                if (mmevent.item == 0) {
                    vec.x += mmevent.value;
                } else {
                    vec.y += mmevent.value;
                }
                circle->setPosition(vec);

            } else if (mmevent.type == MANYMOUSE_EVENT_ABSMOTION) { // I have never witnessed this event
                printf("Mouse #%u absolute motion %s %d\n", mmevent.device, mmevent.item == 0 ? "X" : "Y", mmevent.value);
            } else if (mmevent.type == MANYMOUSE_EVENT_BUTTON) {
                printf("Mouse #%u button %u %s\n", mmevent.device, mmevent.item, mmevent.value ? "down" : "up");
            } else if (mmevent.type == MANYMOUSE_EVENT_SCROLL) {
                const char *wheel;
                const char *direction;
                if (mmevent.item == 0) {
                    wheel = "vertical";
                    direction = ((mmevent.value > 0) ? "up" : "down");
                } else {
                    wheel = "horizontal";
                    direction = ((mmevent.value > 0) ? "right" : "left");
                }
                printf("Mouse #%u wheel %s %s\n", mmevent.device, wheel, direction);
            } else if (mmevent.type == MANYMOUSE_EVENT_DISCONNECT) {
                printf("Mouse #%u disconnect\n", mmevent.device);
            } else {
                printf("Mouse #%u unhandled event type %d\n", mmevent.device, mmevent.type);
            }
        }

        window.clear();
        for (int i = 0; i < available_mice; i++) {
            window.draw(circles[i]);
        }
        window.display();
    }

    return 0;
}
