#pragma once
#include <SFML/Graphics.hpp>

class Boid
{
public:
	// Constructors:
	Boid(float speed, float angular_velocity, sf::Color color, float search_radius);
	Boid();

	// Movement functions:
	void move();
	void rotate(float direction);
	void lin_accelerate(float acc_param);

	// Information functions (functions that return information about the boid)
	void print_stats();
	sf::Vector2f get_position();
	sf::ConvexShape get_boid();
	void set_position(float x, float y);
	void set_rotation(float rot);
	void look_around(Boid* flock, const int flock_size, int current_index);

	// Control function
	void steer();

private:
	sf::ConvexShape boid;
	float boidSpeed;
	float boidAngularVelocity;
	float searchRadius;
	float steer_angle;
};

