#include "Boid.h"
#include <iostream>
#include <SFML/Graphics.hpp>

constexpr auto PI = 3.14;

// Calculates distance between two points
float vector_distance(sf::Vector2f v1, sf::Vector2f v2)
{
	return sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
}

// Calculates length of a vector
float vector_len(sf::Vector2f vec)
{
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2));
}
// Normalizes vector passed in by reference
void normalize_vector(sf::Vector2f& vec)
{
	float len = vector_len(vec);
	vec.x /= len;
	vec.y /= len;
	return;
}

Boid::Boid(float speed, float angular_velocity, sf::Color color, float search_radius)
{
	// Initialization of ConvexShape to have 3 points (triangle) and initially point right
	boid.setPointCount(3);
    boid.setPoint(0, sf::Vector2f(30.0, 0.0));
    boid.setPoint(1, sf::Vector2f(0.0, 10.0));
    boid.setPoint(2, sf::Vector2f(0.0, -10.0));

    boid.setOrigin(10.0, 0.0);
    boid.setFillColor(color);

	// Randomly set boids position and rotation
	boid.setPosition((float)(rand() % 1531), (float)(rand() % 1031));
	boid.setRotation((float)(rand() % 361));
	
	// Initialize variables of the boid 
	boidSpeed = speed;
	boidAngularVelocity = angular_velocity;
	searchRadius = search_radius;
	
	// Default steer_angle is 0 because no change is needed
	steer_angle = 0.0;
}

Boid::Boid()
{
	boid.setPointCount(3);
    boid.setPoint(0, sf::Vector2f(30.0, 0.0));
    boid.setPoint(1, sf::Vector2f(0.0, 10.0));
    boid.setPoint(2, sf::Vector2f(0.0, -10.0));

    boid.setOrigin(10.0, 0.0);
    boid.setFillColor(sf::Color::Black);

	boid.setPosition((float)(rand() % 1531), (float)(rand() % 1031));
	boid.setRotation((float)(rand() % 361));

	// Default parameters of boid:
	boidSpeed = 4.0;
	boidAngularVelocity = 2.0;
	searchRadius = 100.0;

	steer_angle = 0.0;

}

void Boid::move() 
{
	// Moves boid according to its velocity
	float dx = boidSpeed * cos(boid.getRotation() * PI / 180.0);
	float dy = boidSpeed * sin(boid.getRotation() * PI / 180.0);
	boid.move(dx, dy);
}

void Boid::rotate(float angle)
{
	// Rotates boid by given angle (in degrees)
	boid.rotate(angle);
	return;
}

void Boid::print_stats()
{
	sf::Vector2f pos = boid.getPosition();
	float rot_angle = boid.getRotation();
	std::cout << "Boids position: " << pos.x << " " << pos.y << std::endl;
	std::cout << "Speed: " << boidSpeed << std::endl;
	std::cout << "Rotation angle: " << rot_angle << std::endl;
}

void Boid::lin_accelerate(float acc_param)
{
	// Not used but could be interesting,
	// Just multiplies speed by given acc_param
	boidSpeed *= acc_param;
	return;
}

sf::ConvexShape Boid::get_boid()
{
	// Function that allows access to private variable
	return boid;
}

sf::Vector2f Boid::get_position()
{
	// Returns boids current position
	return boid.getPosition();
}

void Boid::set_position(float x, float y)
{
	// Allows changing private variables x, y
	boid.setPosition(x, y);
	return;
}

void Boid::set_rotation(float rot)
{
	// Allows changing private variable rot
	boid.setRotation(rot);
	return;
}


float steering_angle(sf::Vector2f from, sf::Vector2f to, float angular_velocity)
{
	// A little bit of trigonometry to calculate directional (takes into account if it's to the left or to the right) angle from vector "from" to vector "to"
	float dot_product = from.x * to.x + from.y * to.y;
	float det = from.x * to.y - from.y * to.x;
	float angle = atan2(det, dot_product);
	// Conversion to degrees
	angle = angle * 180.0 / PI;

	// If positive, then the second vector is to the left of the first vector
	if (angle > 0.0)
	{
		// Min function so we don't over-rotate, since angular velocity is an integer.
		// F.e. if the angle between the vectors was 1.2 degrees, and we wanted to rotate right by angular_velocity = 3 degrees we would end up with
		// a net angle of -1.8 degrees
		// In this case (if the angle is smaller than the angular velocity) we should just rotate by the angle
		angle = fmin(angle, angular_velocity);
		return angle;
	}
	// If negative, then the second vector is to the right of the first vector
	else
	{
		// Same thing here except the angle is now negative (because the second vector is to the right of the first vector) so we take fmax
		angle = fmax(angle, -angular_velocity);
		return angle;
	}

}

void Boid::steer()
{
	// Rotates boid by the established steering angle between desired velocity and current velocity
	boid.rotate(steer_angle);
	return;
}

void Boid::look_around(Boid* flock, const int flock_size, int current_index)
{
	// MOST IMPORTANT FUNCTION:
	// This function sets the steer_angle parameter which tells the boid how to steer in order to create
	// optimal flocking behavior

	// Takes in:
	// Array of all the other boids, the amount of boids, and what index it is in the array

	// Get the position of the current boid (going to be useful for position vectors later)
	sf::Vector2f current_boid_pos = boid.getPosition();

	// Initializing in order to calculate important vectors later
	sf::Vector2f average_velocity(0.0, 0.0);
	sf::Vector2f average_position(0.0, 0.0);
	sf::Vector2f average_separation_vector(0.0, 0.0);
	int boids_in_range_count = 0;

	// Get  rotation of current boid (going to be important for calculating the boids velocity)
	float current_boid_rotation = boid.getRotation();
	// Change to radians (because cos, and sin take the angle in radians)
	current_boid_rotation = current_boid_rotation * PI / 180.0;

	// Calculate the velocity of the current boid (the vector)
	sf::Vector2f current_boid_velocity_vector = sf::Vector2f(cos(current_boid_rotation), sin(current_boid_rotation));


	// Loop through all boids in the flock
	for (auto i = 0; i < flock_size; i++)
	{
		if (i == current_index)
			// Because we don't want to make observation based off of our own position and out own velocity but the positions and velocities of our fellow flock mates 
			continue;

		// Get the position of the other boid
		sf::Vector2f other_boid_pos = flock[i].get_position();
		// Vector from current boids position to flock[i]'s position
		sf::Vector2f position_difference_vector = sf::Vector2f(other_boid_pos.x - current_boid_pos.x, other_boid_pos.y - current_boid_pos.y);

		// Calculate the distance between the two boids (in order to see if it is in range)
		float dist = vector_distance(current_boid_pos, other_boid_pos);

		// Get other boids rotation so we can calculat it's velocity
		float other_boid_rotation = flock[i].get_boid().getRotation();
		// Change to radians
		other_boid_rotation = other_boid_rotation * PI / 180.0;

		sf::Vector2f other_boid_velocity_vector = sf::Vector2f(cos(other_boid_rotation), sin(other_boid_rotation));

		// Calculating angle between current boid velocity vector and position_difference_vector to see if it's in the boids "peripheral vision" (basically so the boid doesn't see behind itself)
		float dot_product = current_boid_velocity_vector.x * position_difference_vector.x + current_boid_velocity_vector.y * position_difference_vector.y;
		dot_product /= dist;
		// Angle between current boids velocity vector and vector connecting the two boids
		float beta = acos(dot_product);

		beta = beta * 180.0 / PI;

		// If the other boid is within the search radius and it is in the visual cone of the current boid, we take it into account
		if ((dist <= searchRadius)&&(beta <= 100.0))
		{
			// Calculate average position of boids in range
			average_position += other_boid_pos;
			// Calculate average velocity vector of boids in range
			average_velocity += other_boid_velocity_vector;
			// Calculate desired velocity (sum of vectors from other to current)
			average_separation_vector -= position_difference_vector / dist;

			boids_in_range_count++;
		}
	}

	// Calculate angles:
	if (boids_in_range_count > 0)
	{
		// Alignment part:
		// We need the average velocity vector of all boids in range because we want our boid to align with it
		average_velocity.x = average_velocity.x / boids_in_range_count;
		average_velocity.y = average_velocity.y / boids_in_range_count;
		// Average velocity vector is already normalized because it is the average of the sum of unit vectors

		// Cohesion part:
		// We want our boid to also steer toward the center of mass of all of the boids in range (a kind of clustering)
		average_position.x = average_position.x / boids_in_range_count;
		average_position.y = average_position.y / boids_in_range_count;
		// The vector from out position to the center of mass
		sf::Vector2f current_boid_to_center(average_position.x - current_boid_pos.x, average_position.y - current_boid_pos.y);
		normalize_vector(current_boid_to_center);

		// Separation part:
		// We take the average of the negative (because we want to steer away from them) vectors from current boid to all other boids and normalize it
		average_separation_vector.x = average_separation_vector.x / boids_in_range_count;
		average_separation_vector.y = average_separation_vector.y / boids_in_range_count;
		normalize_vector(average_separation_vector);
		
		// Finally we sum all of the vectors and normalize them in order to get a vector that takes into account alignment, cohesion, and separation
		// Now we need to steer toward this vector
		sf::Vector2f net_vector(0.0, 0.0);
		net_vector += average_velocity + current_boid_to_center + average_separation_vector;
		normalize_vector(net_vector);
		
		// Calculate the angle to rotate towards this desired vector
		steer_angle = steering_angle(current_boid_velocity_vector, net_vector, boidAngularVelocity);
	}
	else
	{
		// If no boids in range, dont change your velocity (don't rotate)
		steer_angle = 0.0;
	}

	return;
}
