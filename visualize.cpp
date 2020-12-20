#include <iostream>
#include <SFML/Graphics.hpp>
#include "Boid.h"
#include <ctime>

constexpr auto PI = 3.14;


void move_flock(Boid* flock, const int flock_size)
{

    for (auto i = 0 ; i < flock_size ; i++)
    {
        flock[i].move();
    }
    return;
}

void draw_flock(Boid* flock, const int flock_size, sf::RenderWindow& window)
{
    for (auto i = 0; i < flock_size; i++)
    {
        window.draw(flock[i].get_boid());
    }
    return;
}

void keep_flock_in_frame(Boid* flock, const int flock_size)
{
    for (auto i = 0; i < flock_size; i++)
    {

	    sf::Vector2f pos = flock[i].get_position();

        if (pos.x >= 1530)
        {
            flock[i].set_position(-30.0f, pos.y);
        }
        else if (pos.x <= -30.0f)
        {
            flock[i].set_position(1530.0f, pos.y);
        }
        if (pos.y >= 1030)
        {
            flock[i].set_position(pos.x, -30.0f);
        }
        else if (pos.y <= -30.0f)
        {
            flock[i].set_position(pos.x, 1030.0f);
        }
    }
    return;
}

void flock_look_around(Boid* flock, const int flock_size)
{
    // Calculates steering angle for all boids
    for (auto i = 0; i < flock_size; i++)
    {
        flock[i].look_around(flock, flock_size, i);
    }

    return;
}

void steer_flock(Boid* flock, const int flock_size)
{
    // Steers every boid by the angle calculated
    for (auto i = 0; i < flock_size; i++)
    {
        flock[i].steer();
    }
    return;
}

void randomize_rotations(Boid* flock, const int flock_size)
{
    // In order to make it more interesting, by pressing R you can randomize all boid rotations and watch them disperse
    for (auto i = 0; i < flock_size; i++)
    {
        flock[i].set_rotation((float)(rand() % 361));
    }
    return;
}



int main()
{

    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(1500, 1000), "Flocking Behavior", sf::Style::Default);
    window.setFramerateLimit(60);


    // Hyperparameter controlling boid count
    const int boid_count = 50;

    Boid flock[boid_count];

	while (window.isOpen())
	{
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                // Press R to randomize rotations
                if (evnt.key.code == sf::Keyboard::R)
					randomize_rotations(flock, boid_count);
            }
        }
        // Move the flock according to their given velocity 
        move_flock(flock, boid_count);

        window.clear(sf::Color(135, 206, 235, 0));
        draw_flock(flock, boid_count, window);
        window.display();

        // Screen transfers for out of frame movement
        keep_flock_in_frame(flock, boid_count);

        // Calculate angle and steer boids
        flock_look_around(flock, boid_count);
        steer_flock(flock, boid_count);
    }
    return 0;
}